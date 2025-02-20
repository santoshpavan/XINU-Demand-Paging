/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <paging.h>

unsigned long currSP;	/* REAL sp of current process */

SYSCALL read_currpid_frames(int);
SYSCALL dirty_frames_handler(int);
void update_frame_dirty(int);

/*------------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int	resched()
{
	STATWORD		PS;
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	register int i;

	disable(PS);
	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		restore(PS);
		return(OK);
	}
	
#ifdef STKCHK
	/* make sure current stack has room for ctsw */
	asm("movl	%esp, currSP");
	if (currSP - optr->plimit < 48) {
		kprintf("Bad SP current process, pid=%d (%s), lim=0x%lx, currently 0x%lx\n",
			currpid, optr->pname,
			(unsigned long) optr->plimit,
			(unsigned long) currSP);
		panic("current process stack overflow");
	}
#endif	

	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
    int oldpid = currpid;
	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef notdef
#ifdef	STKCHK
	if ( *( (int *)nptr->pbase  ) != MAGIC ) {
		kprintf("Bad magic pid=%d value=0x%lx, at 0x%lx\n",
			currpid,
			(unsigned long) *( (int *)nptr->pbase ),
			(unsigned long) nptr->pbase);
		panic("stack corrupted");
	}
	/*
	 * need ~16 longs of stack space below, so include that in check
	 *	below.
	 */
	if (nptr->pesp - nptr->plimit < 48) {
		kprintf("Bad SP pid=%d (%s), lim=0x%lx will be 0x%lx\n",
			currpid, nptr->pname,
			(unsigned long) nptr->plimit,
			(unsigned long) nptr->pesp);
		panic("stack overflow");
	}
#endif	/* STKCHK */
#endif	/* notdef */
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
#ifdef	DEBUG
	PrintSaved(nptr);
#endif
    
    /* PSP: things before context switch */
    // read the frames of currpid

    if (currpid != NULLPROC && currpid != 49) {
        if (read_currpid_frames(currpid) == SYSERR) {
            return SYSERR;
        }
    }
    // write the dirty frames back of oldpid
    if (oldpid != NULLPROC && oldpid != 49) {
        if (dirty_frames_handler(oldpid) == SYSERR) {
            return SYSERR;
        }
    }
	write_cr3(nptr->pdbr);

	ctxsw(&optr->pesp, optr->pirmask, &nptr->pesp, nptr->pirmask);

#ifdef	DEBUG
	PrintSaved(nptr);
#endif
	
	/* The OLD process returns here when resumed. */
	restore(PS);
	return OK;
}


#ifdef DEBUG
/* passed the pointer to the regs in the process entry */
PrintSaved(ptr)
    struct pentry *ptr;
{
    unsigned int i;

    if (ptr->pname[0] != 'm') return;
    
    kprintf("\nSaved context listing for process '%s'\n",ptr->pname);
    for (i=0; i<8; ++i) {
	kprintf("     D%d: 0x%08lx	",i,(unsigned long) ptr->pregs[i]);
	kprintf("A%d: 0x%08lx\n",i,(unsigned long) ptr->pregs[i+8]);
    }
    kprintf("         PC: 0x%lx",(unsigned long) ptr->pregs[PC]);
    kprintf("  SP: 0x%lx",(unsigned long) ptr->pregs[SSP]);
    kprintf("  PS: 0x%lx\n",(unsigned long) ptr->pregs[PS]);
}
#endif

void update_frame_dirty(int frm_id) {
    pt_t *pte = (pt_t *) get_pteaddr(frm_id);
    if (pte->pt_dirty == DIRTY) {
        frm_tab[frm_id].fr_dirty = DIRTY;
    }
    else {
        frm_tab[frm_id].fr_dirty = NOT_DIRTY;
    }
}

SYSCALL read_currpid_frames(int pid) {
    int bs_id = proctab[pid].store;
        int i = 0;
        for (; i < NFRAMES; i++) {
            if (frm_tab[i].fr_pid == pid && frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_status == FRM_MAPPED) {
                unsigned int page = frm_tab[i].fr_vpno & 0x000003ff;
                read_bs((char *)((i + FRAME0) * NBPG), (bsd_t)bs_id, page);
            }
        }
    return OK;
}

SYSCALL dirty_frames_handler(int pid) {
    /*
    traverse frm_tab
    update dirty of frm_tab if pt_t is dirty
    if frm_tab belongs to this processes and is dirty then write it
    */
    int i = 0;
    for (; i < NFRAMES; i++) {
        if (frm_tab[i].fr_pid == pid && frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_status == FRM_MAPPED) {    
            update_frame_dirty(i);
            if (frm_tab[i].fr_status == DIRTY) {
                if (write_dirty_frame(i) == SYSERR)
                    return SYSERR;
            }
        }
    }
    return OK;
}
