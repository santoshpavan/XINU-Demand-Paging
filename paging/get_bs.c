#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
  	/* checks if a new mapping of npages is available in bs_id*/
	/* no mapping to be performed here */
	/* mapping done in xmmap */
	
    /*
    perform sanity checks
    if mapped and pvt then error
    if mapped and shared then bs_npages
    if unmapped then npages
    */
    //kprintf("\n---get_bs! pid:%d, bs_id:%d (status: %d), npages:%d\n", currpid, bs_id, bsm_tab[bs_id].bs_status ,npages);
	int bs_ind = (int)bs_id;
	if (npages <= 0 || npages > MAX_BST_SIZE || bs_ind < 0 || bs_ind >= NBSM ) {
		//kprintf("failed sanity check\n");
        return SYSERR;
	}
    bs_map_t *bsmap = &bsm_tab[bs_ind];
    if (bsmap->bs_status == BS_MAPPED) {
        if (bsmap->pvt == IS_PRIVATE) {
            //kprintf("failed - bs is private!\n");
            return SYSERR;
        }
        else {
            //kprintf("returning bs_npages:%d\n", bsmap->bs_npages);
            return bsmap->bs_npages;//assgined
        }
    }
    else {
        //kprintf("returning npages:%d\n", npages);
        return npages;
    }
	return SYSERR;
}
