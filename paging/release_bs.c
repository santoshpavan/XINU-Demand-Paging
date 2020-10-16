#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
	/* release the backing store with ID bs_id */
	kprintf("---release_bs!\n");
	
	/*
 	* depends on shared or private
 	* if private call free_bsm and clear everything
 	* if shared only reset the pages with the calling process
 	*/
	int bs_ind = (int)bs_id;
	if (bsm_tab[bs_ind].pvt == IS_PRIVATE) {
		if (bsm_tab[bs_ind].bs_pid == currpid)
			return free_bsm(bs_ind);
		else
			return SYSERR;
	}
	else {
		//TODO:might need vmemlist here
	}

   	return OK;
}

