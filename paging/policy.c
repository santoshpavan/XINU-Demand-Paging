/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
	kprintf("---setting policy!\n");
    if (policy != SC || policy != AGING)
        return SYSERR;
	page_replace_policy = policy;
  	return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  	return page_replace_policy;
}

/* PSP: paging policy functions */
void init_policy_lists() {
    // SC Policy
    sc_head.ind = -1;
    sc_head.next = NULL;
    sc_tail.ind = -2;
    sc_tail.next = NULL;
    clock_hand = NULL;

    // Aging Policy
    ag_head.ind = -1;
    ag_head.next = NULL;
    ag_tail.ind = -2;
    ag_tail.next = NULL;
}

void add_sc_list(int frame_ind) {
    /* head -> new_ele -> first_ele -> ... ->last_ele -> new_ele <- tail*/
    sc_list node;
    node.ind = frame_ind;
    if (sc_head.next == NULL) {
        sc_head.next = &node;
        node.next = &node;
    }
    else {
        node.next = (sc_tail.next)->next;
    }
    sc_tail.next = &node;
}

void add_ag_list(int frame_ind) {
    /* tail --> new_ele ->...-> first_ele <- head */
    ag_list node;
    node.ind = frame_ind;
    node.next = ag_tail.next;
    node.age = MAX_AGE;
    if (head.next == NULL)
        head.next = &node;
    ag_tail.next = &node;
}
/* removing elements implemented in the replace_page function */
