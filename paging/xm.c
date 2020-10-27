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
	kprintf("---\nxmmap called! source:%d, npages%d pid:%d\n", source, npages, currpid);
	
	int bs_ind = (int) source;
	// checking mapping availability here
	int check = get_bs(source, npages);
	if (check == SYSERR || check < npages) {
		//kprintf("check failed! check:%d\n", check);
        return SYSERR;
    }
	// mapping here
	if (bsm_tab[bs_ind].bs_status == BS_UNMAPPED) {
        //kprintf("unmapped bs\n");
		return bsm_map(currpid, virtpage, bs_ind, npages);
	}
    //TODO: not sure
	else if (proctab[currpid].pvt == NOT_PRIVATE) {
		//bsm_tab[bs_ind].bs_npages += npages;
        //bsm_map(currpid, virtpage, bs_ind, npages);
        //kprintf("shared bs\n");
        proctab[currpid].store = bs_ind;
        proctab[currpid].vhpno = virtpage;
        return OK;
	}
    
    //kprintf("debug- xmmap failed!\n");
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
    //TODO: unmapping for shared stores => partial unmapping
	kprintf("---xmunmap! with pid:%d\n", currpid);
	/*
    int i = 0;
	for (; i < NBSM; i++) {
		if (bsm_tab[i].bs_vpno == virtpage) {
			return bsm_unmap(currpid, virtpage, BS_UNMAPPED);
		}
	}
    */
    return bsm_unmap(currpid, virtpage, BS_UNMAPPED);
	//return SYSERR;
}
