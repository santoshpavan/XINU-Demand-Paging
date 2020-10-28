/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

STATWORD ps;
/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
    disable(ps);
    /* only for non-private */
	
	int bs_ind = (int) source;
	// checking mapping availability here
	int check = get_bs(source, npages);
	if (check == SYSERR || check < npages) {
        return SYSERR;
    }
	// mapping here
	if (bsm_tab[bs_ind].bs_status == BS_UNMAPPED) {
		return bsm_map(currpid, virtpage, bs_ind, npages);
	}
    //TODO: not sure
	else if (proctab[currpid].pvt == NOT_PRIVATE) {
        proctab[currpid].store = bs_ind;
        proctab[currpid].vhpno = virtpage;
        return OK;
	}
    
    restore(ps);
	return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
    /* only for non-private */
    return bsm_unmap(currpid, virtpage, BS_UNMAPPED);
}
