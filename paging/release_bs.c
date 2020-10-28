#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
STATWORD ps;
SYSCALL release_bs(bsd_t bs_id) {
    disable(ps);
	/* release the backing store with ID bs_id */
	/*
 	* depends on shared or private
 	* if private call free_bsm and clear everything
 	* if shared only reset the pages with the calling process
 	*/
	int bs_ind = (int)bs_id;
	if (bsm_tab[bs_ind].pvt == IS_PRIVATE) {
		if (bsm_tab[bs_ind].bs_pid == currpid) {
    restore(ps);
            return free_bsm(bs_ind);
        }
		else {
            restore(ps);
            return SYSERR;
        }
	}
	else {
		//bsm_unmap(currpid, 0, 0);
        int i = 1; //ignore the NULLPROC
        int count = 0;      
        for (; i < NPROC; i++) {
            if (proctab[i].store == bs_ind && i != currpid) {
                count++;                                                               
            }                        
        }
        if ((count == 0 && currpid != 49) || (currpid == 49 && count == 1))
            free_bsm(bs_ind);                                                        
	}
    restore(ps);
   	return SYSERR;
}
