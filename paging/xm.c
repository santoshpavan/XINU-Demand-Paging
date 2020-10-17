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
	int bs_ind = (int) source;
	// checking mapping availability here
	int check = get_bs(source, npages);
	if (check == SYSERR || check < npages || 
		bsm_tab[bs_ind].pvt == IS_PRIVATE)
		return SYSERR;
	// mapping here
	if (bsm_tab[bs_ind].bs_status == BS_UNMAPPED) {
		bsm_map(currpid, virtpage, bs_ind, npages);
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
		if (bsm_tab[i].bs_vpno == virtpage) {
			//bsm_tab[i].bs_status = BSM_UNMAPPED;
			bsm_unmap(currpid, virtpage, BSM_UNMAPPED);
			return OK;
		}
	}

	return SYSERR;
}
