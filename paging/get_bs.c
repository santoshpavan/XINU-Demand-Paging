#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
  	/* checks if a new mapping of npages is available in bs_id*/
	/* no mapping to be performed here */
	/* mapping done in xmmap */
	kprintf("---get_bs!\n");
	
	if (	npages == 0 || npages > MAX_BST_SIZE ||
		bs_id < 0 || bs_id >= NBSM ) {
		return SYSERR
	}
	int bs_ind = (int)bs_id;
	if (bsm_tab[bs_ind].bs_status == BS_MAPPED) {
		if (bsm_tab[bs_ind].pvt == PRIVATE)
			return SYSERR;
		//TODO: else statement doesn't make sense
		else
			return bsm_tab[bs_ind].bs_npages;
	}
	return npages;
}

