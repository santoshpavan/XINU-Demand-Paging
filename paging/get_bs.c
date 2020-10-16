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
		/*if (bsm_tab[bs_id].bs_pid == currpid &&
			bsm_tab[bs_id].bs_npages + npages <= MAX_BST_SIZE) {
			bsm_tab[bs_npages] += npages;
		}
		else
			return SYSERR;
		*/
		if (	bsm_tab[bs_ind].bs_pid != currpid ||
                        bsm_tab[bs_ind].bs_npages + npages > MAX_BST_SIZE)
			return SYSERR;
	}
	else {
		return bsm_tab[bs_ind].bs_npages;
		/*
		bsm_tab[bs_id].bs_status = BS_MAPPED;
		bsm_tab[bs_id].bs_pid = currpid;
		bsm_tab[bs_id].bs_npages = npages;
		*/
	}
	
	// if already mapped
	/*
	if (bsm_tab[bs_id].bs_status == BS_MAPPED) {
		if (bsm_tab[bs_id].pvt == PRIVATE) {
			if (bsm_tab[bs_id].bs_pid != currpid)
				return SYSERR;
			else
				update_bsm(bs_id, npages);
		}
		// not private but not enough pages
		else if (npages > MAX_BST_SIZE - bsm_tab[bs_id].bs_npages) {
			return SYSERR;
		}
		else {
			update_bsm_tab(bs_id, npages); 
		}
	}
	else {
		
	}
	*/
	return npages;
}

