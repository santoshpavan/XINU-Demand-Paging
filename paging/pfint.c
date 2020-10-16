/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
	kprintf("!--pfint\n");
	set_evec(PF_INTERRUPT, (void (* isr)(void)));
  	return OK;
}


