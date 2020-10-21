/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>


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
  //TODO: which address is this?
  /*
  unsigned long fault_addr = read_cr2();
  //TODO: Need to check the legality of fault_addr (PA2 description)
  unsigned long pt_no = fault_addr>>22;
  unsigned long pg_no = (fault_addr>>12)&003FF;
  unsigned long offset = fault_addr & 0x00000FFF;
  */
  struct virt_addr_t fault_addr = (virt_addr_t) read_cr2();
  unsigned int pt_no = fault_addr.pd_offset;
  unsigned int pg_no = fault_addr.pt_offset;
  unsigned int offset = fault_addr.pg_offset;
  
  struct pd_t *fault_pd_addr = (struct pd_t *) fault_addr;
  if (fault_pd_addr->pd_pres == 0) {
    /*
    assign a frame and update the directory with its base value
    */
    int freeframe_ind = get_frm(NULL);
    if (freeframe_ind != SYSERR) {
      struct fr_map_t *avail_frame = &frm_tab[freeframe_ind];
      avail_frame->fr_status = FRM_MAPPED;
      //avail_frame->fr_pid
      //avaid_frame->fr_vpno
      //avail_frame->fr_refcnt
      avail_frame->fr_dirty = NOT_DIRTY;
      avail_frame->fr_type = FR_TBL;
    }
    //TODO: page replacement not considered here?
    else {
      // perform page replacement to get the frame
    }
    // update the directory with the table's base value
    fault_pd_addr->pd_base = avail_frame;
  }
  
  /*
  locate bs_id of the faulted page - use bsm_tab with procid and vaddr - bsm_lookup
  find free frame
  if no free frame, perform page replacement
  */
  
	return OK;
}

