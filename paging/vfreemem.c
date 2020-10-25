/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	kprintf("---FREEMEM!\n");
	
	STATWORD ps;
    struct  mblock  *p, *q;
    unsigned top;
	
	/*
 	* TODO: Change the maxaddr here as freemem deals with physical 
 	* address and vfreemem is dealing with virtual memory
 	*/	
    /*
    if (size==0 || (unsigned)block>(unsigned)maxaddr
        || ((unsigned)block)<((unsigned) &end))
    */
    unsigned long base_addr = BACKING_STORE_BASE + BACKING_STORE_UNIT_SIZE * proctab[currpid].store;
    unsigned long max_virt_addr = base_addr + proctab[currpid].vhpnpages*NBPG;
    if (size == 0 || (unsigned)block > max_virt_addr || (unsigned)block < base_addr)
            return(SYSERR);
    size = (unsigned)roundmb(size);
    disable(ps);
        
	struct mblock *v_heap_memlist = proctab[currpid].vmemlist;
	for( p=v_heap_memlist->mnext,q=v_heap_memlist;
             p != (struct mblock *) NULL && p < block ;
             q=p,p=p->mnext )
                ;
        if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= v_heap_memlist) ||
            (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
                restore(ps);
                return(SYSERR);
        }
        if ( q!= v_heap_memlist && top == (unsigned)block )
                        q->mlen += size;
        else {
                block->mlen = size;
                block->mnext = p;
                q->mnext = block;
                q = block;
        }
        if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
                q->mlen += p->mlen;
                q->mnext = p->mnext;
        }
        restore(ps);

	return(OK);
}
