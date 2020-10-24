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
	fr_map_t frm_tab[NFRAMES];
	int i = 0;
	for(; i < NFRAMES; i++) {
		frm_tab[i].status = FRM_UNMAPPED;
        frm_tab[i].fr_pid = -1;			
        frm_tab[i].fr_vpno = -1;			
        frm_tab[i].fr_refcnt = 0;			
        //frm_tab[i].fr_type;				
        frm_tab[i].fr_dirty = NOT_DIRTY;
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
    int i = 0;
    for (; i < NFRAMES; i++) {
        if (frm_tab[i] == UNMAPPED) {
            *avail = i;
            if (grpolicy() == SC) {
                add_sc_list(i);
            }
            else {
                add_ag_list(i);
            }
            return OK;
        }
    }
    return SYSERR;
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
