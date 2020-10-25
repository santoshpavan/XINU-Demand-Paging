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
		/*
        traverse through the shared_list in bsm_tab
        if found the mapping with this procid, then unmap it
        */
        shared_list *ptr = bsm_tab[bs_ind].bs_pids;
        shared_list *prev = NULL;
        while (ptr != NULL) {
            if (ptr->bs_pid == currpid) {
                // unmapping
                prev->next = ptr->next;
                return OK;
            }
            prev = ptr;
            ptr = ptr->next;
        }
	}
   	return SYSERR;
}
