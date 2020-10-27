/* pfintr.S - pfintr */


    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:
	pop pferrcode
	pushfl
	cli
	pushal
	call pfint
	popal
	;sti
	popfl
	iret
