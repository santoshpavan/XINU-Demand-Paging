#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
  	/* requests a new mapping of npages with ID map_id */
	kprintf("---get_bs!\n");
	
	if (npages == 0 || bs_id < 0 || bs_id >= NBSM) {
		return SYSERR
	}
	int i = 0;
	for (; i < NBSM; i++) {
		
	}
		
	return npages;

}


