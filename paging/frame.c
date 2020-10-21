/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
	kprintf("---init frame!\n");
    // pointing to the 1024th page beginning
	struct fr_map_t *frm_tab = (fr_map_t *)(1024 * 4096 + 1);
	//struct fr_map_t frm_tab[NFRAMES];
	//frm_tab = fr_base_ptr;
	int i = 0;
	for(; i < NFRAMES; i++) {
		//frm_tab[i] = fr_base_ptr;
		frm_tab[i].status = FRM_UNMAPPED;
		fr_base_ptr++;
	}
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  kprintf("---getting frame!!!\n");
  
  /*
  perform a linear search in the frm_tab to look for unmapped.
  after getting add to the global data structure based on the replacement policy
  */
  // flag to check the need for replacement
  // int found_empty = 0;
  for (int i = 0; i < NFRAMES; i++) {
    if (frm_tab[i] == UNMAPPED) {
      // available
      // found_empty = 1;
      if (page_replace_policy == SC) {
        // add to the circular queue
      }
      else {
        // add to FIFO queue
      }
      return i;
    }
  }
  // if not found
  return SYSERR;
  //return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
  kprintf("---Freeing frame!\n");
  frm_tab[i] = FRM_UNMAPPED;
  return OK;
}

