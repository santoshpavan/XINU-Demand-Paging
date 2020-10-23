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
    /* only for non-private */
	kprintf("---xmmap!\n");
	
	int bs_ind = (int) source;
	// checking mapping availability here
	int check = get_bs(source, npages);
	if (bsm_tab[bs_ind].pvt == IS_PRIVATE || check == SYSERR || check < npages)
		return SYSERR;
	// mapping here
	if (bsm_tab[bs_ind].bs_status == BS_UNMAPPED) {
		return bsm_map(currpid, virtpage, bs_ind, npages);
	}
	else {
		return bsm_tab[bs_ind].bs_npages += npages;
	}
	return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
    /* only for non-private */
    //TODO: unmapping for shared stores => partial unmapping
	kprintf("---xmunmap!\n");
	int i = 0;
	for (; i < NBSM; i++) {
		if (bsm_tab[i].bs_vpno == virtpage) {
			return bsm_unmap(currpid, virtpage, BSM_UNMAPPED);
		}
	}

	return SYSERR;
}
