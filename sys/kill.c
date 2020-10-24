/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
void free_frames_on_kill(int);

SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
		
    /*PSP: clearing things belonging to the process before death */
    /*
    clear bs: i.e. frm_tab unmapping
    clear paging lists - unmap those frames
    */
    release_bs(proctab[currpid].store);
    free_frames_on_kill(currpid);
    
    switch (pptr->pstate) {
    	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
    			resched();
    
    	case PRWAIT:	semaph[pptr->psem].semcnt++;
    
    	case PRREADY:	dequeue(pid);
    			pptr->pstate = PRFREE;
    			break;
    
    	case PRSLEEP:
    	case PRTRECV:	unsleep(pid);
    						/* fall through	*/
    	default:	pptr->pstate = PRFREE;
	}
    
    restore(ps);
	return(OK);
}

void free_frames_on_kill(int pid) {
    /*
    traverse through the list acc to the paging policy
    delete if pid == currpid
    reset frame for that same frameid
    */
    if (grpolicy() == AGING) {
        ag_list *hand = ag_tail.next;
        ag_list *prev = &ag_tail;
        while(hand != NULL) {
            if (frm_tab[hand->ind].fr_pid == pid) {
                prev->next = hand->next;
                free_frm(hand->ind);
                hand = hand->next;
            }
            else {
                prev = hand;
                hand = hand->next;
            }
        }
    }
    else {
        sc_list *clock_hand = sc_head.next;
        sc_list *prev = &sc_head;
        while (clock_hand != sc_tail.nextp) {
            if (frm_tab[clock_hand->ind].fr_pid == pid) {   
                prev->next = head->next;
                free_frm(hand->ind);
                clock_hand = clock_hand->next;
            }
            else {
                prev = prev->next;
                clock_hand = clock_hand->next;
            }        
        }
    }
}
