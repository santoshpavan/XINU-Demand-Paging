/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
static unsigned long *reg; 

SYSCALL pfint()
{
      /*
      get the addr from the c2
      get the information from the above
      check if directory has the page 
      */
      unsigned long fault_addr = read_cr2();
      unsigned long pt_no = fault_addr>>22;
      unsigned long vpno = fault_addr>>12;
      
      unsigned long pg_no = vpno & 0x000003ff;
      unsigned long offset = fault_addr & 0x00000FFF;

      int store, page;
      // checking if fault_addr is legal
      if (bsm_lookup(currpid, fault_addr, &store, &page) == SYSERR) {
          kill(currpid);
          return SYSERR;
      }

      unsigned long proc_pdbr = proctab[currpid].pdbr;

      pd_t *fault_pde = (pd_t *)(proc_pdbr + pt_no*4);
      /* case a: second level table DNE */
      if (fault_pde->pd_pres == 0) {
            int freeframe_ind;
            if (get_frm(&freeframe_ind) == SYSERR) {
                // perform page replacement to get the frame
                freeframe_ind = replace_page();
            }
            fr_map_t *avail_frame = &frm_tab[freeframe_ind];
            avail_frame->fr_status = FRM_MAPPED;
            avail_frame->fr_pid = currpid;
            avail_frame->fr_type = FR_TBL;
            avail_frame->fr_dirty = NOT_DIRTY;
            // update the directory with the table's base value
            fault_pde->pd_base = freeframe_ind + FRAME0;
            fault_pde->pd_pres = PRESENT;
      }
      /*
      case b: PTE DNE
      locate bs_id of the faulted page - use bsm_tab with procid and vaddr - bsm_lookup
      find free frame
      if no free frame, perform page replacement
      */
      pt_t *fault_pte = (pt_t *)((fault_pde->pd_base) * NBPG + pg_no*4);
      fault_pte->pt_pres = 1;
      fault_pte->pt_write = 1;
      fault_pte->pt_dirty = 1;
      
      int freeframe_ind;
      if (get_frm(&freeframe_ind) == SYSERR) {
              // perform page replacement to get the frame
              freeframe_ind = replace_page();
              // adding to the policy data structure as this is a page
              add_to_policy_list(freeframe_ind);
              if (write_dirty_frame(freeframe_ind) == SYSERR)
                  return SYSERR;
      }
      fr_map_t *avail_frame = &frm_tab[freeframe_ind];
      avail_frame->fr_status = FRM_MAPPED;
      avail_frame->fr_pid = currpid;
      avail_frame->fr_vpno = vpno;
      avail_frame->fr_dirty = DIRTY;
      avail_frame->fr_type = FR_PAGE;
      avail_frame->fr_refcnt++;
      read_bs((char *)((freeframe_ind + FRAME0) * NBPG), (bsd_t)store, page);
      // update the directory with the table's base value
      fault_pte->pt_base = freeframe_ind + FRAME0;
      fault_pte->pt_pres = PRESENT;
      
      // writing just to make sure
      write_cr3(proc_pdbr);
      return OK;
}

/* page replacement and get free frame ID */
int replace_page() {
    int frame_ind;
    if (grpolicy() == AGING) {
        int min_age = MAX_AGE;
        ag_list *hand = ag_tail.next;
        ag_list *prev = &ag_tail;
        ag_list *prev_minind = NULL;
        // traversing from tail to head
        while (hand != NULL) {
            hand->age = hand->age>>1;
            if (check_acc(hand->ind) == 1) {
                hand->age += 128;
                if (hand->age > MAX_AGE)
                    hand->age = MAX_AGE;
            }
            if (hand->age <= min_age) {
                frame_ind = hand->ind;
                prev_minind = prev;
                min_age = hand->age;
            }
            prev = prev->next;
            hand = hand->next;
        }
        // delete from the list
        prev_minind->next = (prev_minind->next)->next;
    }
    else {
        sc_list *clock_hand = sc_head.next;
        sc_list *prev = &sc_head;
        while (1) {
            // frame with acc = 0 found
            if (check_acc(clock_hand->ind) == 0)
                break;
            prev = prev->next;
            clock_hand = clock_hand->next;            
        }
        frame_ind = clock_hand->ind;
        // delete from the list
        prev->next = clock_hand->next;
        clock_hand->next = NULL;
    }

    pt_t *pte = (pt_t *) get_pteaddr(frame_ind);
    pte->pt_pres = NOT_PRESENT;
    if (currpid == frm_tab[frame_ind].fr_pid) {
        *reg = (frm_tab[frame_ind].fr_vpno*NBPG);
        // invalidating the TLB entry
        asm("invlpg reg");
    }
    frm_tab[frame_ind].fr_refcnt--;
    if (frm_tab[frame_ind].fr_refcnt == 0) {
        int vpno = frm_tab[frame_ind].fr_vpno;
        unsigned long pd_offset = vpno & 0xFFC00;
        unsigned long pdbr = proctab[frm_tab[frame_ind].fr_pid].pdbr;
        pd_t *pde = (pd_t *) (pdbr + (pd_offset*4));
        pde->pd_pres = NOT_PRESENT;
    }
    kprintf("The frame index (form 0) replaced: %d\n", frame_ind+FRAME0);
    return frame_ind;
}

int check_acc(int frame_ind) {
    // check pt_acc and put 0
    pt_t *pte = (pt_t *) get_pteaddr(frame_ind);
    if (pte->pt_acc == 0)
        return 0;
    else {
        pte->pt_acc = 0;
        return 1;
    }
}

unsigned long get_pteaddr(int frame_ind) {
    int vpno = frm_tab[frame_ind].fr_vpno;
    unsigned long pd_offset = vpno & 0xFFC00;
    unsigned long pt_offset = vpno & 0x003FF;
    unsigned long pdbr = proctab[frm_tab[frame_ind].fr_pid].pdbr;
    pd_t *pde = (pd_t *) (pdbr + (pd_offset*4));
    return (((pde->pd_base) * NBPG) + (pt_offset*4));
}

SYSCALL write_dirty_frame(int frame_ind) {
    unsigned long vpno = frm_tab[frame_ind].fr_vpno;
    pt_t *pte = (pt_t *) get_pteaddr(frame_ind);
    int pid = frm_tab[frame_ind].fr_pid;

    // set by the hardware
    if (pte->pt_dirty == DIRTY) {
        int store, page;
        if (bsm_lookup(pid, vpno * NBPG, &store, &page) == SYSERR) {
            return SYSERR;
        }
        unsigned long pdbr = proctab[currpid].pdbr;
        unsigned int pd_offset = vpno & 0xFFC00;
        pd_t *pde = (pd_t *) (pdbr + (pd_offset*4));
        write_bs((char *)((pde->pd_base) * NBPG), (bsd_t)store, page);
    }
    return OK;
}
