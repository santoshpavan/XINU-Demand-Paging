/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
      kprintf("!--pfint\n");	
      /*
      get the addr from the c2
      get the information from the above
      check if directory has the page 
      */
      unsigned long fault_addr = read_cr2();
      //TODO: Need to check the legality of fault_addr (PA2 description)
      unsigned long pt_no = fault_addr>>22;
      unsigned long vpno = fault_addr>>12;
      unsigned long pg_no = vpno & 003FF;
      unsigned long offset = fault_addr & 0x00000FFF;
      /*
      struct virt_addr_t fault_addr = (virt_addr_t) read_cr2();
      unsigned int pt_no = fault_addr.pd_offset;
      unsigned int pg_no = fault_addr.pt_offset;
      unsigned int vpno = ;
      unsigned int offset = fault_addr.pg_offset;
      */
      if (check_valid(vpno) == -1) {
          kill(currpid);
          return SYSERR;
      }
      
      unsigned long proc_pdbr = proctab[currpid].pdbr;

      struct pd_t *fault_pde = (struct pd_t *) (proc_pdbr + (long)pt_no);
      /* case a: second level table DNE */
      if (fault_pde->pd_pres == 0) {
            int freeframe_ind;
            if (get_frm(&freeframe_ind) != SYSERR) {
                struct fr_map_t *avail_frame = &frm_tab[freeframe_ind];
                avail_frame->fr_status = FRM_MAPPED;
                avail_frame->fr_pid = currpid;
                avail_frame->fr_type = FR_TBL;
                // update the directory with the table's base value
                fault_pde->pd_base = avail_frame;
                fault_pde->pd_pres = ;
            }
            //TODO: page replacement not considered here?
            else {
                // perform page replacement to get the frame
            }
            struct fr_map_t *avail_frame = &frm_tab[freeframe_ind];
      }
      
      /*
      case b: PTE DNE
      locate bs_id of the faulted page - use bsm_tab with procid and vaddr - bsm_lookup
      find free frame
      if no free frame, perform page replacement
      */
      int store, page;
      if (bsm_lookup(currpid, virtaddr, &store, &page) == SYSERR) {
          return SYSERR;
      }
      struct pt_t *fault_pte = (struct pt_t *) ();
      if (fault_pte->pt_pres == 0) {
          int freeframe_ind;
            if (get_frm(&freeframe_ind) != SYSERR) {
                struct fr_map_t *avail_frame = &frm_tab[freeframe_ind];
                avail_frame->fr_status = FRM_MAPPED;
                avail_frame->fr_pid = currpid;
                avaid_frame->fr_vpno = (int)vpno;
                avail_frame->fr_refcnt = 1;
                avail_frame->fr_dirty = NOT_DIRTY;
                avail_frame->fr_type = FR_PAGE;
                // update the directory with the table's base value
                fault_pte->pt_base = avail_frame;
            }
            else {
                // perform page replacement to get the frame
                freeframe_ind = replace_page(store, page);
                if (freeframe_ind == SYSERR) {
                    return SYSERR;
                }
            }
            struct fr_map_t *avail_frame = &frm_tab[freeframe_ind];
      }
      return OK;
}

/* page replacement and get free frame ID */
int replace_page(int store_id, int page_id) {
    if (page_replace_policy == SC) {
        clock_hand = sc_head.next;
        while (check_pres(clock_hand->ind) == 1) {
            clock_hand = clock_hand->next;
        }
        // swap if 0
        int vpno = frm_tab[clock_hand->ind];
        //int bs_read = read_bs(  ,(bsd_t)bs_id, page_id);
        if (bs_read == SYSERR) {
            return SYSERR;
        }
    }
    else {
        
    }
}

int check_pres(int frame_ind) {
    // check pt_acc and put 0
}
