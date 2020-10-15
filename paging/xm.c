/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
	kprintf("---xmmap!\n");
	
	/*
  	if (	npages <= 0 || npages > MAX_BST_SIZE || 
		source >= NBSM || bsm_tab[source].bs_pid != currpid ||
		bsm_tab[source].status == BSM_MAPPED) {
		return SYSERR;
	}
	*/

	// checking mapping availability here
	if (get_bs(source, npages) == SYSERR)
		return SYSERR;
	// mapping here
	if (bsm_tab[source].bs_status == BS_UNMAPPED) {
		bsm_map(currpid, virtpage, source, npages);
		/*
		bsm_tab[source].bs_status = BS_MAPPED;
		bsm_tab[source].bs_pid = currpid;
		bsm_tab[source].bs_npages = npages;
		bsm_tab[source].bs_vpno = virtpage;
		*/
	}
	else {
		bsm_tab[source].bs_npages += npages;
	}
	return OK;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	kprintf("---xmunmap!\n");

	int i = 0;
	for (; i < NBSM; i++) {
		if (bsm_tab[i].bs_vpno == virtpage && bsm_tab[i].bs_pid == currpid) {
			//bsm_tab[i].bs_status = BSM_UNMAPPED;
			bsm_unmap(currpid, virtpage, BSM_UNMAPPED);
			return OK;
		}
	}

	return SYSERR;
}
