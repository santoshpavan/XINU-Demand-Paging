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
    kprintf("---init bsm\n");
	int i = 0;
	for (; i < NBSM; i++) {
		bsm_tab[i].bs_status = BS_UNMAPPED;
		bsm_tab[i].bs_pid = -1; //used when private
        //bsm_tab[i].bs_pids = NULL; //used when shared
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
 	* interate through the bsm_tab
 	* the first one that is available take it.
 	*/
    kprintf("getting bsm\n");
	int i = 0;
	for (; i < NBSM; i++) {
		if (bsm_tab[i].bs_status == BS_UNMAPPED) {
			*avail = i;
            return OK;
		}
	}
    kprintf("getting bsm failed!!!\n");
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	// called when the BS is private and reset everything
    kprintf("free bsm %d\n", i);
	bsm_tab[i].bs_status = BS_UNMAPPED;
    bsm_tab[i].bs_pid = -1;
    bsm_tab[i].bs_npages = 0;
    bsm_tab[i].pvt = NOT_PRIVATE;
    bsm_tab[i].bs_vpno = -1;
    proctab[currpid].store = -1;
    return OK;    
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
    if (vaddr < 0)
        return SYSERR;
    int bs_id = proctab[pid].store;
    // if processes has been unmapped
    if (bs_id == -1)
        return SYSERR;
    //if (vaddr <= bsm_tab[bs_id].bs_npages) {
        *store = bs_id;
        // getting vpno from the vaddr
        //*pageth = vaddr>>12;
        *pageth = (int)(vaddr/NBPG) - bsm_tab[bs_id].bs_vpno;
    kprintf("bsm lookup! pid:%d store:%d pageth:%d\n", pid, bs_id, *pageth);                
        return OK;
    //}
    //kprintf("bsm lookup failed!!\n");
    //return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
    //kprintf("bsm mapping pid%d, vpno:%d, bs:%d, pages:%d\n", pid, vpno, source, npages);
	
    if (pid != currpid)
        return SYSERR;
    
    bsm_tab[source].bs_status = BS_MAPPED;
    //kprintf("1*********\n");
    bsm_tab[source].bs_npages = npages;
    //kprintf("2*********\n");
    bsm_tab[source].bs_vpno = vpno;
    //kprintf("3*********\n");
    proctab[pid].store = source;
    proctab[pid].vhpno = vpno;
    //kprintf("4*********\n");
    //if (proctab[pid].pvt == IS_PRIVATE) {
        bsm_tab[source].bs_pid = pid;
        
    //kprintf("1*********\n");        
        //bsm_tab[source].pvt = IS_PRIVATE;
    /*}
    else {
        shared_list *ptr = bsm_tab[source].bs_pids;
        //shared_list *prev = NULL;
        while (ptr->next != NULL) {
            //prev = ptr;
            ptr->next = ptr;
        }
        shared_list newbs_pid;
        newbs_pid.bs_pid = pid;
        newbs_pid.next = NULL;
        if (ptr == NULL) {
            bsm_tab[source].bs_pids = &newbs_pid;
        }
        else {
            ptr->next = &newbs_pid;
        }
    }*/
    //kprintf("bsm mapping done!\n");
	return OK;
}


/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
    kprintf("bsm unmapping pid: %d\n", currpid);
	
    if (pid != currpid)
        return SYSERR;
	int bs_ind = proctab[pid].store;
    proctab[pid].store = -1;
    proctab[pid].vhpno = -1;
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
                if (count >= 1) {
                    break;
                }
            }
        }
        if (count == 0) {
            // the last proc in this shared store
            free_bsm(bs_ind);
        }
                                
        /*shared_list *ptr = bsm_tab[bs_ind].bs_pids;
        shared_list *prev = NULL;
        while (ptr != NULL) {
            if (ptr->bs_pid == pid) {
                // unmapping
                prev->next = ptr->next;
                return OK;
            }
            prev = ptr;
            ptr = ptr->next;
        }*/
    }
	return OK;
}
