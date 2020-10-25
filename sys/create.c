/* create.c - create, newpid */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

LOCAL int newpid();
void create_directory(int);

/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL create(procaddr,ssize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	unsigned long	savsp, *pushsp;
	STATWORD 	ps;    
	int		pid;		/* stores new process id	*/
	struct	pentry	*pptr;		/* pointer to proc. table entry */
	int		i;
	unsigned long	*a;		/* points to list of args	*/
	unsigned long	*saddr;		/* stack address		*/
	int		INITRET();

	disable(ps);
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (int) roundew(ssize);
	if (((saddr = (unsigned long *)getstk(ssize)) ==
	    (unsigned long *)SYSERR ) ||
	    (pid=newpid()) == SYSERR || priority < 1 ) {
		restore(ps);
		return(SYSERR);
	}

	numproc++;
	pptr = &proctab[pid];

	pptr->fildes[0] = 0;	/* stdin set to console */
	pptr->fildes[1] = 0;	/* stdout set to console */
	pptr->fildes[2] = 0;	/* stderr set to console */

	for (i=3; i < _NFILE; i++)	/* others set to unused */
		pptr->fildes[i] = FDFREE;

	pptr->pstate = PRSUSP;
	for (i=0 ; i<PNMLEN && (int)(pptr->pname[i]=name[i])!=0 ; i++)
		;
	pptr->pprio = priority;
	pptr->pbase = (long) saddr;
	pptr->pstklen = ssize;
	pptr->psem = 0;
	pptr->phasmsg = FALSE;
	pptr->plimit = pptr->pbase - ssize + sizeof (long);	
	pptr->pirmask[0] = 0;
	pptr->pnxtkin = BADPID;
	pptr->pdevs[0] = pptr->pdevs[1] = pptr->ppagedev = BADDEV;

	create_directory(pid);

	/* Bottom of stack */
	*saddr = MAGIC;
	savsp = (unsigned long)saddr;

	/* push arguments */
	pptr->pargs = nargs;
	a = (unsigned long *)(&args) + (nargs-1); /* last argument	*/
	for ( ; nargs > 0 ; nargs--)	/* machine dependent; copy args	*/
		*--saddr = *a--;	/* onto created process' stack	*/
	*--saddr = (long)INITRET;	/* push on return address	*/

	*--saddr = pptr->paddr = (long)procaddr; /* where we "ret" to	*/
	*--saddr = savsp;		/* fake frame ptr for procaddr	*/
	savsp = (unsigned long) saddr;

/* this must match what ctxsw expects: flags, regs, old SP */
/* emulate 386 "pushal" instruction */
	*--saddr = 0;
	*--saddr = 0;	/* %eax */
	*--saddr = 0;	/* %ecx */
	*--saddr = 0;	/* %edx */
	*--saddr = 0;	/* %ebx */
	*--saddr = 0;	/* %esp; fill in below */
	pushsp = saddr;
	*--saddr = savsp;	/* %ebp */
	*--saddr = 0;		/* %esi */
	*--saddr = 0;		/* %edi */
	*pushsp = pptr->pesp = (unsigned long)saddr;

	restore(ps);

	return(pid);
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL int newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}

/* PSP: Create directory */
void create_directory(int pid) {
    /*
 	* Find empty page frame
 	* put directory there
 	* frame mapping
 	*/
	int freeframe_ind;
    if (get_frm(freeframe_ind) == SYSERR) {
        freeframe_ind = replace_page();
    }
    fr_map_t *frm_ptr = &frm_tab[freeframe_ind];
	frm_ptr->fr_status = FRM_MAPPED;
	frm_ptr->fr_pid = pid;
	frm_ptr->fr_refcnt = 0;
	frm_ptr->fr_type = FR_DIR;
	frm_ptr->fr_dirty = NOT_DIRTY;
    
    struct	pentry	*pptr = &proctab[pid];
    pptr->pdbr = (unsigned long) ((1024 * freeframe_ind) * 4096);
    pptr->ppolicy = grpolicy();
    /*
    struct virt_addr_t = (struct virt_addr_t) (0);
    frm_ptr->vpnp = (int) virt_addr_t;
    */
    unsigned int pte_ind = 0;
    for (; pte_ind < MAX_FRAME_SIZE; pte_ind++) {
        pd_t *pd_ptr = (pd_t *) (pptr->pdbr + (pte_ind * sizeof(pd_t)));
        pd_ptr->pd_pres = 1;
        pd_ptr->pd_write = 1;
        pd_ptr->pd_user = 0;
        pd_ptr->pd_pwt = 0;
        pd_ptr->pd_pcd = 0;
        pd_ptr->pd_acc = 0;
        pd_ptr->pd_mbz = 0;
        pd_ptr->pd_fmb = 0;
        pd_ptr->pd_global = 0;
        pd_ptr->pd_avail = 0;
        if (pte_ind < 4) {
            pd_ptr->pd_pres = 1;
            pd_ptr->pd_base = (unsigned int)((1025 + pte_ind) * 4096);
        }
   	}
}
