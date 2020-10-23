/* paging.h */

typedef unsigned int	 bsd_t;

/* Structure for a page directory entry */

typedef struct {
  unsigned int pd_pres	: 1;		/* page table present?		*/
  unsigned int pd_write : 1;		/* page is writable?		*/
  unsigned int pd_user	: 1;		/* is use level protection?	*/
  unsigned int pd_pwt	: 1;		/* write through cachine for pt?*/
  unsigned int pd_pcd	: 1;		/* cache disable for this pt?	*/
  unsigned int pd_acc	: 1;		/* page table was accessed?	*/
  unsigned int pd_mbz	: 1;		/* must be zero			*/
  unsigned int pd_fmb	: 1;		/* four MB pages?		*/
  unsigned int pd_global: 1;		/* global (ignored)		*/
  unsigned int pd_avail : 3;		/* for programmer's use		*/
  unsigned int pd_base	: 20;		/* location of page table?	*/
} pd_t;

/* Structure for a page table entry */

typedef struct {
  unsigned int pt_pres	: 1;		/* page is present?		*/
  unsigned int pt_write : 1;		/* page is writable?		*/
  unsigned int pt_user	: 1;		/* is use level protection?	*/
  unsigned int pt_pwt	: 1;		/* write through for this page? */
  unsigned int pt_pcd	: 1;		/* cache disable for this page? */
  unsigned int pt_acc	: 1;		/* page was accessed?		*/
  unsigned int pt_dirty : 1;		/* page was written?		m - get a free entry from bsm_tab*/
  unsigned int pt_mbz	: 1;		/* must be zero			*/
  unsigned int pt_global: 1;		/* should be zero in 586	*/
  unsigned int pt_avail : 3;		/* for programmer's use		*/
  unsigned int pt_base	: 20;		/* location of page?		*/
} pt_t;

typedef struct{
  unsigned int pg_offset : 12;		/* page offset			*/
  unsigned int pt_offset : 10;		/* page table offset		*/
  unsigned int pd_offset : 10;		/* page directory offset	*/
} virt_addr_t;

typedef struct{
  int bs_status;			/* MAPPED or UNMAPPED		*/
  int bs_pid;				/* process id using this slot   */
  int bs_vpno;				/* starting virtual page number */
  int bs_npages;			/* no of pages in the store used*/
  int bs_sem;				/* semaphore mechanism ?	*/
  int pvt;				    /* PSP:BS used for private heap?*/
} bs_map_t;

typedef struct{
  int fr_status;			/* MAPPED or UNMAPPED		*/
  int fr_pid;				/* process id using this frame  */
  int fr_vpno;				/* corresponding virtual page no*/
  int fr_refcnt;			/* reference count		*/
  int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
  int fr_dirty;
} fr_map_t;

extern bs_map_t bsm_tab[];
extern fr_map_t frm_tab[];

/* Prototypes for required API calls */
SYSCALL xmmap(int, bsd_t, int);
SYSCALL xunmap(int);

/* given calls for dealing with backing store */
int get_bs(bsd_t, unsigned int);
SYSCALL release_bs(bsd_t);
SYSCALL read_bs(char *, bsd_t, int);
SYSCALL write_bs(char *, bsd_t, int);

#define NBPG		    4096	/* number of bytes per page	*/
#define FRAME0		    1024	/* zero-th frame		*/
#define NFRAMES 	    1024	/* number of frames		*/

#define FRM_UNMAPPED	0
#define FRM_MAPPED	    1

#define FR_PAGE		    0
#define FR_TBL		    1
#define FR_DIR		    2

#define SC 		        3
#define AGING 		    4

#define BACKING_STORE_BASE	0x00800000
#define BACKING_STORE_UNIT_SIZE 0x00100000

/* PSP: more constants used */
#define NBSM            8       /* number of bsm tables         */
#define MAX_BST_SIZE    256     /* BST has max 256 entries      */
#define MAX_FRAME_SIZE  1024    /* Page has max 1024 entries    */
#define N_GLOBAL_PT     4       /* number of global page tables */
#define BS_UNMAPPED	    0
#define BS_MAPPED	    1
#define DIRTY		    1
#define NOT_DIRTY	    0
#define PF_INTERRUPT	14

#ifndef NOT_PRIVATE
#define NOT_PRIVATE	    0
#endif

#ifndef IS_PRIVATE
#define IS_PRIVATE	    1
#endif

/* PSP: system calls for frames */
SYSCALL init_frm();
SYSCALL free_frm(int *);
SYSCALL get_frm(int);

/* PSP: syscalls for bsm */
SYSCALL init_bsm();
SYSCALL get_bsm(int *);
SYSCALL free_bsm(int);
SYSCALL bsm_lookup(int, long, int *, int *);
SYSCALL bsm_map(int, int, int, int);
SYSCALL bsm_unmap(int, int, int);

/* PSP: Control Registers functions */
unsigned long read_cr0(void);
unsigned long read_cr2(void);
unsigned long read_cr3(void);
unsigned long read_cr4(void);
void write_cr0(unsigned long);
void write_cr3(unsigned long);
void write_cr4(unsigned long);
void enable_paging();

/* PSP: ISR - Page Fault Handler */
void pfintr(void);
extern long pferrcode;

/* PSP: the data structures for page replacement */
SYSCALL srpolicy(int);
SYSCALL grpolicy(void);
int replace_page(void);

// for SC policy
typedef struct {
  int ind;
  struct sc_list* next;
} sc_list;
struct sc_list sc_head;
struct sc_list sc_tail;
struct sc_list *clock_hand;

// for Aging policy
typedef struct {
  int ind;
  int age;
  struct ag_list* next;
} ag_list;
struct ag_list ag_head;
struct ag_list ag_tail;

void init_policy_lists(void);
void add_sc_list(int frame_ind);
void add_ag_list(int frame_ind);
