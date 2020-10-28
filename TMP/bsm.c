/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
STATWORD ps;
SYSCALL init_bsm()
{
    disable(ps);
	int i = 0;
	for (; i < NBSM; i++) {
		bsm_tab[i].bs_status = BS_UNMAPPED;
		bsm_tab[i].bs_pid = -1; //used when private
		bsm_tab[i].bs_npages = 0;
        bsm_tab[i].bs_vpno = -1;
		bsm_tab[i].pvt = NOT_PRIVATE;
	}
    restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
    disable(ps);
	/* 
 	* interate through the bsm_tab
 	* the first one that is available take it.
 	*/
	int i = 0;
	for (; i < NBSM; i++) {
		if (bsm_tab[i].bs_status == BS_UNMAPPED) {
			*avail = i;
            return OK;
		}
	}
    restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
    disable(ps);
	// called when the BS is private and reset everything
	bsm_tab[i].bs_status = BS_UNMAPPED;
    bsm_tab[i].bs_pid = -1;
    bsm_tab[i].bs_npages = 0;
    bsm_tab[i].pvt = NOT_PRIVATE;
    bsm_tab[i].bs_vpno = -1;
    proctab[currpid].store = -1;
    restore(ps);
    return OK;    
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, unsigned long vaddr, int* store, int* pageth)
{
    disable(ps);
    /*
    get storeid from the proctab
    check if that is valid
    check if vaddr is valid
    assign store and pageth values    
    */
    if (vaddr < 0) {
        
    restore(ps);
        return SYSERR;
    }
    int bs_id = proctab[pid].store;
    // if processes has been unmapped
    if (bs_id == -1) {        
    restore(ps);
        return SYSERR;
    }
    *store = bs_id;
    *pageth = (unsigned int)(vaddr/NBPG) - (unsigned int)proctab[pid].vhpno;              
        
    restore(ps);
        return OK;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
    disable(ps);
	
    if (pid != currpid) {
    restore(ps);
        return SYSERR;
    }
    
    bsm_tab[source].bs_status = BS_MAPPED;
    bsm_tab[source].bs_npages = npages;
    bsm_tab[source].bs_vpno = vpno;
    proctab[pid].store = source;
    proctab[pid].vhpno = vpno;
    bsm_tab[source].bs_pid = pid;
	
    restore(ps);
    return OK;
}


/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
    disable(ps);
    int bs_ind = proctab[pid].store;
    if (bsm_tab[bs_ind].pvt == IS_PRIVATE) {
    	bsm_tab[bs_ind].bs_status = BS_UNMAPPED;
    	bsm_tab[bs_ind].pvt = NOT_PRIVATE;
    }
    else{
        // check if any other process is mapped to this store
        int proc_id = 1; //ignore NULLPROC and MAIN
        int count = 0;
        for (; proc_id < NPROC; proc_id++) {
            if (proc_id != pid && proctab[proc_id].store == bs_ind) {
                count++;
            }
        }
        if ((count == 0 && currpid != 49) || (currpid == 49 && count == 1)) {
            // the last proc in this shared store
            free_bsm(bs_ind);
        }
    }
    proctab[pid].store = -1;
    proctab[pid].vhpno = -1;
    
    restore(ps);
	return OK;
}
