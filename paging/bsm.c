/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
    //struct bs_map_t *bsm_tab = (struct bs_map_t *)BACKING_STORE_BASE;
	// there are 8 entries in bsm_tab
	struct bs_map_t bsm_tab[NBSM];
	int i = 0;
	for (; i < NBSM; i++) {
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_npages = 0;
        bsm_tab[i].bs_vpno = -1;
		bsm_tab[i].pvt = NOT_PRIVATE;
	}
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	/* 
 	* intereate through the bsm_tab
 	* the first one that is available take it.
 	*/
	int i = 0;
	for (; i < NBSM; i++) {
		if (get_bs((bsd_t) i, *avail) != SYSERR) {
			return i;
		}
	}
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	// called when the BS is private and reset everything
	bsm_tab[i].bs_status = BSM_UNMAPPED;
    bsm_tab[i].bs_pid = -1;
    bsm_tab[i].bs_npages = 0;
    bsm_tab[i].pvt = NOT_PRIVATE;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
    /*
    traverse through the bsmtab
    if the proc is private only consider private bs with the same pid
    else consider shared bs and traverse all of their entries
    */
    if (proctab[currpid].pvtproc == IS_PRIVATE) {
        int i = 0;
        for (; i < NBSM; i++) {
            if (bsm_tab[i].bs_status == BS_MAPPED && bsm_tab[i].pvt == IS_PRIVATE && bsm_tab[i].bs_pid == pid) {
                
            }
        }
    }
    else {
        int i = 0;
        for (; i < NBSM; i++) {
            if (bsm_tab[i].bs_status == BS_MAPPED && bsm_tab[i].pvt == NOT_PRIVATE && bsm_tab[i].bs_pid == pid) {
                
            }
        }
    }
    
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
    if (pid != currpid)
        return SYSERR;
	bsm_tab[source].bs_status = BS_MAPPED;
    bsm_tab[source].bs_pid = pid;
    bsm_tab[source].bs_npages = npages;
    bsm_tab[source].bs_vpno = vpno;
	return OK;
}


/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
    if (pid != currpid)
        return SYSERR;
	int bs_ind = proctab[pid].store;
	proctab[pid].store = -1;
	bsm_tab[bs_ind].bs_status = BSM_UNMAPPED;
	bsm_tab[bs_ind].pvt = NOT_PRIVATE;
	return OK;
}
