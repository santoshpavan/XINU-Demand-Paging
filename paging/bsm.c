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
	// there are 8 entries in bsm_tab
	bs_map_t bsm_tab[NBSM];
	int i = 0;
	for (; i < NBSM; i++) {
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_npages = 0;
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
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	bsm_tab[source].bs_status = BS_MAPPED;
        bsm_tab[source].bs_pid = currpid;
        bsm_tab[source].bs_npages = npages;
        bsm_tab[source].bs_vpno = virtpage;
	return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	int i = 0;
	for (; i < NBMS; i++) {
		if (bsm_tab[i].bs_pid == currpid && bsm_tab[i].bs_vpno == vpno) {
		bsm_tab[i].bs_status = BSM_UNMAPPED;
	}
	}
	return OK;
}

