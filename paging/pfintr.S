/* pfintr.S - pfintr */


    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:
	pop pferrcode
	pushfd
	cli
	pushad
	pfintr
	popad
	sti
	popfd
	ret
