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
	set_evec(PF_INTERRUPT + IRQBASE, (void (* isr)(void)));
  	return OK;
}


