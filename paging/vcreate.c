/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	kprintf("----CREATING PROCESS - vcreate\n");
    int pid = create(procaddr,ssize,priority,name,nargs,args);
    if (pid == SYSERR)
        return SYSERR;
    STATWORD        ps;
	disable(ps);
	/*
 	* get bs from get_bsm
	* make it private
	* map that bs using bsm_map
	* do not call xmmap here
	*/
	proctab[pid].pvtproc = IS_PRIVATE;
	int bs_id;
    get_bsm(bs_id);
	bsm_tab[bs_id].pvt = IS_PRIVATE;
	proctab[pid].store = bs_id;
	proctab[pid].vhpnpages = hsize;
    proctab[pid].vmemlist->mlen = hsize*NBPG;
    proctab[pid].vmemlist->mnext = (mblock*) BACKING_STORE_BASE + bs_id*BACKING_STORE_UNIT_SIZE;
	//proctab[pid].vhpno = ;// starting pageno for heap-impl in get_bsm	
	bsm_map(pid, procaddr>>12, bs_id, hsize);

	restore(ps);	
	return OK;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
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
