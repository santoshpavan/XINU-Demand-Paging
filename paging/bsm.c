/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
    //bs_map_t *bsm_tab = (bs_map_t *)BACKING_STORE_BASE;
	// there are 8 entries in bsm_tab
	//bs_map_t bsm_tab[NBSM];
	int i = 0;
	for (; i < NBSM; i++) {
		bsm_tab[i].bs_status = BS_UNMAPPED;
		bsm_tab[i].bs_pid = -1; //used when private
        bsm_tab[i].bs_pids = NULL; //used when shared
		bsm_tab[i].bs_npages = 0;
        bsm_tab[i].bs_vpno = -1;
		bsm_tab[i].pvt = NOT_PRIVATE;
	}
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	/* 
 	* intereate through the bsm_tab
 	* the first one that is available take it.
 	*/
	int i = 0;
	for (; i < NBSM; i++) {
		if (get_bs((bsd_t) i, *avail) != SYSERR) {
			return i;
		}
	}
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	// called when the BS is private and reset everything
	bsm_tab[i].bs_status = BS_UNMAPPED;
    bsm_tab[i].bs_pid = -1;
    bsm_tab[i].bs_npages = 0;
    bsm_tab[i].pvt = NOT_PRIVATE;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
    /*
    get storeid from the proctab
    check if that is valid
    check if vaddr is valid
    assign store and pageth values    
    */
    //TODO: multiple store mappings for a process
    if (vaddr < 0)
        return SYSERR;
    int bs_id = proctab[pid].store;
    // if processes has been unmapped
    if (bs_id == -1)
        return SYSERR;
    if (vaddr <= bsm_tab[bs_id].bs_npages) {
        *store = bs_id;
        // getting vpno from the vaddr
        *pageth = vaddr>>12;
        return OK;
    }
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
    if (pid != currpid)
        return SYSERR;
    
    bsm_tab[source].bs_status = BS_MAPPED;
    bsm_tab[source].bs_npages = npages;
    bsm_tab[source].bs_vpno = vpno;
    proctab[pid].store = source;
    if (proctab[pid].pvt == IS_PRIVATE) {
        bsm_tab[source].bs_pid = pid;
    }
    else {
        shared_list *ptr = bsm_tab[source].bs_pids;
        shared_list *prev = NULL;
        while (ptr != NULL) {
            prev = ptr;
            ptr->next = ptr;
        }
        shared_list newbs_pid;
        newbs_pid.bs_pid = pid;
        newbs_pid.next = NULL;
        prev->next = &newbs_pid;
    }
	return OK;
}


/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
    if (pid != currpid)
        return SYSERR;
	int bs_ind = proctab[pid].store;
    if (bsm_tab[bs_ind].pvt == IS_PRIVATE) {
    	proctab[pid].store = -1;
    	bsm_tab[bs_ind].bs_status = BS_UNMAPPED;
    	bsm_tab[bs_ind].pvt = NOT_PRIVATE;
    }
    else{
        // shared bs unmapping
        shared_list *ptr = bsm_tab[bs_ind].bs_pids;
        shared_list *prev = NULL;
        while (ptr != NULL) {
            if (ptr->bs_pid == pid) {
                // unmapping
                prev->next = ptr->next;
                return OK;
            }
            prev = ptr;
            ptr = ptr->next;
        }
    }
	return OK;
}
