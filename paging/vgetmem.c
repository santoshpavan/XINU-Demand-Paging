/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{
	STATWORD ps;
    struct  mblock  *p, *q, *leftover;
    
    disable(ps);
    struct mblock *v_heap_memlist = proctab[currpid].vmemlist;
	if (nbytes == 0 || nbytes > proctab[currpid].vhpnpages*4096 ||
		v_heap_memlist->mnext == (struct mblock *) NULL) {
        restore(ps);
        
        return( (WORD *)SYSERR);
    }
    nbytes = (unsigned int) roundmb(nbytes);
    for (q = v_heap_memlist, p = v_heap_memlist->mnext ;
        p != (struct mblock *) NULL ;
        q = p, p = p->mnext)
        if ( p->mlen == nbytes) {
            q->mnext = p->mnext;
            restore(ps);
            return( (WORD *)p );
        } else if ( p->mlen > nbytes ) {
            leftover = (struct mblock *)( (unsigned)p + nbytes );
            q->mnext = leftover;
            leftover->mnext = p->mnext;
            leftover->mlen = p->mlen - nbytes;
            restore(ps);
            return( (WORD *)p );
    }
    restore(ps);
    return( (WORD *)SYSERR );
}


