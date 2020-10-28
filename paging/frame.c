/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
STATWORD ps;
SYSCALL init_frm()
{
    disable(ps);
    // pointing to the 1024th page beginning
	int i = 0;
	for(; i < NFRAMES; i++) {
		frm_tab[i].fr_status = FRM_UNMAPPED;
        frm_tab[i].fr_pid = -1;
        frm_tab[i].fr_vpno = -1;
        frm_tab[i].fr_refcnt = 0;
        frm_tab[i].fr_refcnt = 0;
        //frm_tab[i].fr_type;
        frm_tab[i].fr_dirty = NOT_DIRTY;
	}
    restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
    disable(ps);
    /*
    perform a linear search in the frm_tab to look for unmapped.
    after getting add to the global data structure based on the replacement policy
    */
    int i = 0;
    for (; i < NFRAMES; i++) {
        if (frm_tab[i].fr_status == FRM_UNMAPPED) {
            *avail = i;
            //add_to_policy_list(i); impl in pfint!
            restore(ps);
            return OK;
        }
    }
    restore(ps);
    return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
    disable(ps);
    write_dirty_frame(i);
    frm_tab[i].fr_status = FRM_UNMAPPED;
    frm_tab[i].fr_vpno = -1;
    frm_tab[i].fr_pid = -1;
    
    restore(ps);
    return OK;
}
