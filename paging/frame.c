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
	struct fr_map_t *fr_base_ptr = (fr_map_t *)(1023 * 4096 + 1);
	struct fr_map_t frm_tab[NFRAMES];
	//frm_tab = fr_base_ptr;
	int i = 0;
	for(; i < NFRAMES; i++) {
		frm_tab[i] = fr_base_ptr;
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
  kprintf("To be implemented!\n");
  return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{

  kprintf("To be implemented!\n");
  return OK;
}

