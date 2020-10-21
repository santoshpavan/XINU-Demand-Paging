#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
  	/* checks if a new mapping of npages is available in bs_id*/
	/* no mapping to be performed here */
	/* mapping done in xmmap */
	kprintf("---get_bs!\n");
	int bs_ind = (int)bs_id;
	if (npages <= 0 || npages > MAX_BST_SIZE || bs_ind < 0 || bs_ind >= NBSM ) {
		return SYSERR
	}
	if (proctab[currpid].pvtproc == IS_PRIVATE) {
        //if (bsm_tab[bs_ind].bs_status == BS_MAPPED)
        return SYSERR;
    }
	//int bs_ind = (int)bs_id;
	if (bsm_tab[bs_ind].bs_status == BS_MAPPED) {
		return (MAX_BST_SIZE - bsm_tab[bs_ind].bs_npages);
	}
	return npages;
}

