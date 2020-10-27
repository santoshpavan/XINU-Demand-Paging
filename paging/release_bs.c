#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
	/* release the backing store with ID bs_id */
	kprintf("---release_bs! pid:%d bs_id(%d):%d\n", currpid, bs_id, bsm_tab[bs_id].bs_status);
	
	/*
 	* depends on shared or private
 	* if private call free_bsm and clear everything
 	* if shared only reset the pages with the calling process
 	*/
	int bs_ind = (int)bs_id;
	if (bsm_tab[bs_ind].pvt == IS_PRIVATE) {
		if (bsm_tab[bs_ind].bs_pid == currpid)
			return free_bsm(bs_ind);
		else {
		    //proctab[currpid].store = -1;
            kprintf("\nPVT BSMTAB cannot be cleared by other procs!\n");
            return SYSERR;
        }
	}
    else {
		//bsm_unmap(currpid, 0, 0);
        int i = 1; //ignore the NULLPROC
        int count = 0;
        for (; i < NPROC; i++) {
            if (proctab[i].store == bs_ind) {
                count++;
                //kprintf("iiiii---procid: %d\n", i);
            }
        }
        //kprintf("----count: %d", count);
        if (count <= 1)
            free_bsm(bs_ind);
        /*
        //traverse through the shared_list in bsm_tab
        //if found the mapping with this procid, then unmap it
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
        */
	}
   	return SYSERR;
}
