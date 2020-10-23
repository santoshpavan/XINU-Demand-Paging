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
		return SYSERR;
	}
    struct bs_map_t *pmap = &bsm_tab[bs_ind];
    struct pentry *pproc = &proctab[currpid];    
	if (pproc->pvt == IS_PRIVATE) {
        if (pmap->status == BS_MAPPED && pmap->bs_pid == currpid && pmap->pvt == IS_PRIVATE) {
            return MAX_BST_SIZE - pmap->bs_npages;
        }
        else if (pmap->status == BS_UNMAPPED) {
            return npages;
        }
    }
    else {
    	if (pmap->pvt == NOT_PRIVATE) {
            if (pmap->status == BS_MAPPED)
    		    return (MAX_BST_SIZE - bsm_tab[bs_ind].bs_npages) >= npages ? npages : SYSERR;
            else
                return npages;
    	}
    }
	return SYSERR;
}
