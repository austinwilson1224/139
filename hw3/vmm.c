//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////                                                ///////////
///////////     M a c h i n e - I n d e p e n d e n t      ///////////
///////////           S i m u l a t o r   o f              ///////////
///////////     V i r t u a l   M e m o r y   M g r .      ///////////
///////////                                                ///////////
///////////                                                ///////////
///////////  Austin Wilson, 10/21/2020, Sac State etc.     ///////////
///////////                                                ///////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//  Purpose: Simulate Demand-Paged Virtual Memory Manager

#include <stdio.h>   // for printf()
// #include <malloc.h>  // for malloc()
#include <stdlib.h>  // for exit()

//      S t a r t   D a t a    D e c l a r a t i o n s

#define NULL                   0 // for null pointer 
#define BpW                    4 // old computer-architecture 
#define P_SZ                1024 // entries per page table, 10 bits to index
#define MAX_PAGES    P_SZ * P_SZ // P_SZ per PT * P_SZ Page Tables
#define P_OFF_MASK         0xfff // 1111_1111_1111 page; any other way OK too!
#define PT_X_MASK       0x3ff000 // 0011_1111_1111_0000_0000_0000 PT
#define PD_X_MASK     0xffc00000 // 1111_1111_1100_0000_0000_0000_0000_0000 PD
#define TWO32         0xffffffff // max integer number -1 
#define BITS22                22 // for count of >>
#define BITS12                12 // for count of >>

#define FALSE                  0
#define TRUE                   1
#define NO_VALUE              -1  // dummy for uninitialized

#define BOOL            unsigned  // my :-) boolean type 

/*  A few magic numbers specifying the memory attributes of
 *  the target machine.
 */

#define CYC_MEM_ACC           10		// brute force, simple estimate
#define CYC_PAGE_FAULT_R    5000		// brute force, simple estimate
#define CYC_PAGE_FAULT_W    5000		// brute force, simple estimate

typedef struct page_e_tp			// one single entry, need 1024
{
    int                         v;            // 4 bytes, int ptr to user-page
    unsigned                    lru;          // mem access step of last access
    BOOL                        dirty;        // BIT clear if no write
    BOOL                        present;      // BIT clear if on disk, and not in mem.
} struct_page_e_tp;

typedef struct_page_e_tp page_tp[ P_SZ ];
#define PAGE_SIZE sizeof( page_tp )

typedef page_tp * page_ptr_tp; // should be page_e_tp 

page_tp pd;                                   /* Page Directory, yet uninitialized */

/*  The FE of the vmm simulator reads the memory requests
 *  from  stdin.  They are generated by  another program.
 *  The FE reads the memory access requests, one at a time,
 *  creates triple access, address, and data if applicable
 */

typedef struct triple_tp {
    char                   action;
    int                    m_addr;
    int                    m_val;             /* is dummy if action != 'w' */
} struct_triple_tp;

#define TRIPLE_SIZE sizeof( struct_triple_tp )

struct_triple_tp triple;

unsigned        access_cnt      =  0;         // number of memory accesses
int             triple_cnt      = -1;         // account for EPS; num triples 1 greater 
unsigned        swap_out_cnt    =  0;         // times swapped out 
unsigned        malloc_page_cnt =  0;         // pages malloced 
unsigned        page_fault_cnt  =  0;         // that many page faults 

unsigned        cycles          =  0;         // cycles actually taken 
unsigned        cycles_no_vmm   =  0;         // cycles had there been no vmm 

unsigned        max_phys_pages  =  0;         // dictated by max physical mem 
unsigned        max_pt_pages    =  0;         // pages for page tables 
unsigned        max_real_pages  =  0;         // pages for real data 
unsigned        max_log_pages   =  MAX_PAGES; // dictated by architecture 
unsigned        ws              =  0;         // working set <= max_phys_pages 
unsigned        max_ws          =  0;         // working set got this big 

BOOL            want_trace      =  FALSE;     //
BOOL            want_swap       =  FALSE;     
BOOL            want_dump       =  FALSE; 

int             indent          =  0;

#define TRACE_CALL
#ifdef TRACE_CALL
void trace_in( fu_name )
char * fu_name;
{ /* trace_in */
    int i;

    indent++;
    for ( i = 0; i < indent; i++ ) {
        if ( ( i % 10 ) == 0 ) {
            printf( "." );
        }else{
            printf( " " );
        } //end if
    } /*end for*/
    printf( " > %d %s\n", access_cnt, fu_name );
    fflush( stdout );
} /*end trace_in*/

void trace_out( fu_name )
char * fu_name;
{ /* trace_out */
    int i;

    for ( i = 0; i < indent; i++ ) {
        if ( ( i % 10 ) == 0 ) {
            printf( "." );
        }else{
            printf( " " );
        } //end if
    } /*end for*/
    printf( " < %d %s\n", access_cnt, fu_name );
    indent--;
    fflush( stdout );
} /*end trace_out*/

#else

#define trace_in
#define trace_out

#endif /* TRACE_CALL */
void error( msg )
char * msg;
{ /* error */
    printf( " **ERROR** %s. \n", msg );
} /*end error*/

void assert( cond, msg )
int    cond;
char * msg;
{ /* assert */
    if ( ! cond ) {
        printf( "\n * * * Assertion error! * * *\n" );
        error( msg );
    } //end if
} /*end assert*/

//  No pages in use yet. Initially every memory access causes a page
//  fault. Then one page after another is brought in. Demand Paging.
//
void init_page( page )
page_tp page;
{ /* init_page */
    int p_x;

    for ( p_x = 0; p_x < P_SZ; p_x++ ) {
        page[ p_x ].v       = NO_VALUE;
        page[ p_x ].lru     = 0;
        page[ p_x ].dirty   = FALSE;
        page[ p_x ].present = FALSE;
    } /*end for*/
} /*end init_page*/

FILE * swap;

void init_vm_system( )
{ /* init_vm_system */

    trace_in( "init_vm_system" );
    if ( ! ( swap = fopen( "swap", "w" ) ) ) {
        error( "Cannot open file 'swap'" );
    } //end if
    fprintf( swap, "swap information.\n" );

    init_page( pd );
    trace_out( "init_vm_system" );
} /*end init_vm_system*/

void put_triple( )
{ /* put_triple */
    if ( triple_cnt % 5 == 0 ) {
        printf( "\n" );
    } //end if
    printf( "%c ", triple.action );
    printf( "%5d ", triple.m_addr );
    if ( triple.action == 'w' ) {
        printf( "%4d  ", triple.m_val );
    }else{
        printf( "      " );
    } //end if
} /*end put_triple*/

/*  Get one of the following:
 *
 *  w address value
 *  r address
 *  p max_phys_page_frames
 */

unsigned get_triple( )
{ /* get_triple */
    char     action = ' ';
    int      length;
    int      value;

#   define   DONE 0
#   define   MORE 1

    trace_in( "get_triple" );
    triple_cnt++;
    while ( action == ' ' ) {
        length = scanf( "%s", & action );
        if ( length == EOF ) {
            trace_out( "get_triple" );
            return DONE;
        }else{
            assert( ( length == 1 ), "Wrong number of bytes read" );
            switch ( action ) {
              case 'r':
              case 'w':
              case 'p':
              case ' ':
                break;
              case '\t':
                action = ' ';
                break;
              default:
                printf( "Illegal action. Found '%c'. Only p r w allowed.\n", action );
            } /*end switch*/
        } //end if
    } //end while

    triple.action = action;
    length = scanf( "%d", & value );
    if ( length == EOF ) {
        trace_out( "get_triple" );
        return DONE;
    } //end if

    /* first int has been read, may be last int */
    triple.m_addr = value;
    triple.m_val  = NO_VALUE;

    if ( action == 'w' ) {
        length = scanf( "%d", & value );
        if (  length == EOF ) {
            trace_out( "get_triple" );
            return DONE;
        } //end if
        triple.m_val = value;
    }else if ( action == 'p' ) {
        if ( 0 == max_phys_pages ) {
            max_phys_pages = value;
        }else{
            printf( "physical pages already set to %d\n", max_phys_pages );
        } //end if
    } //end if
    trace_out( "get_triple" );
    return MORE;
} /*end get_triple*/

// get heap space, private function
//
char * my_malloc( size )
unsigned size;
{ /* my_malloc */
    char * ptr = (char *)malloc( size );

    assert( ( size % BpW == 0 ), "Non-aligned memory request" );
    assert( ( ptr != NULL ), "No more heap space" );
    return ptr;
} /*end my_malloc*/

void statistics( )
{ /* statistics */

    printf( "\n\n * * * Paging Activity Statistics * * *\n\n" );

    printf( " number of memory accesses       = %d\n", access_cnt );
    printf( " number of triples (1 + access)  = %d\n", triple_cnt );
    printf( " number of swap ins (faults)     = %d\n", page_fault_cnt );
    printf( " number of swap outs             = %d\n", swap_out_cnt );
    printf( " total number of pages malloced  = %d\n", malloc_page_cnt );
    printf( " number of pages for Tape Tables = %d\n", max_pt_pages );
    printf( " number of real pages for user   = %d\n", max_real_pages );
    printf( " total memory cycles             = %d\n", cycles );
    printf( " cycles w/o vmm                  = %d\n", cycles_no_vmm );
    printf( " 2 * cycles w/o vmm would be     = %d\n", 2 * cycles_no_vmm );
    printf( " cycles per swap_in              = %d\n", CYC_PAGE_FAULT_R );
    printf( " cycles per swap_out             = %d\n", CYC_PAGE_FAULT_W );
    printf( " last working set size           = %d\n", ws );
    printf( " max working set size ever       = %d\n", max_ws );
    printf( " max physical pages              = %d\n", max_phys_pages );
    printf( " page size                       = 0x%x     = %d\n", BpW * P_SZ, BpW * P_SZ );
    printf( " Address range                   = 0x%x = %u\n", TWO32, TWO32 );
} /*end statistics*/



//////////////////-------------------- hint 3

//
// part 3 of 4, sketching a SW solution, how you might implement hw 3
//

//
//      S t a r t   S i m u l a t o r   P r o p e r
//

/*   3 2   B i t s   C o n s t i t u t e   L o g i c a l   A d d r e s s   l a
 *
 *   10 bits Page Dir Index  10 Bits Page Table Index  12 Bits Byte offset
 *  +-----------------------+------------------------+-------------------------+
 *  |                       |                        |                         |
 *  |   o                   |   o                    |    o                    |
 *  |   |                   |   |                    |    |                    |
 *  +---v-------------------+---v--------------------+----v--------------------+
 *      |                       |                         |
 *      |                       |                         |      User Page
 *      |                       |                         |  +----------------+
 *      |                       |                         |  |                |
 *      |                       |                   p_off |  |                |
 *      |                       |                         +->|  Real Element  |
 *      |                       |      Page Table            |                |
 *      |                       |  +----------------+        |                |
 *      |                       |  |                |        |                |
 *      |                       |  |                |        |                |
 *      |                  pt_x |  |                |        +----------------+
 *      |                       |  |                |                 ^
 *      |   Page Directory      |  |                |                 |
 *      |  +----------------+   +->|       o-------->-----------------+ p
 *      |  |                |      |                |
 *      |  |                |      |                |
 *      |  |                |      +----------------+
 *      |  |                |              ^
 *      |  |                |              |
 *      +->|        o------->--------------+ pt
 *    pd_x |                |
 *         |                |
 *         +----------------+
 *
 *   let pd be statically allocated == in special HW register/cache
 */

void inc( new_cycles )
int new_cycles;
{ /* inc */
    cycles += new_cycles;
} /*end inc*/

void show_page_structure( msg1, msg2 )
char * msg1;
char * msg2;
{ /* show_page_structure */
    page_ptr_tp pt    = NULL;
    page_ptr_tp p     = NULL;
    unsigned    pd_x;
    unsigned    pt_x;

    trace_in( "show_page_structure" );
    fprintf( swap, "\n  * * * %s %s * * *\n\n", msg1, msg2 );

    /*  Sweep though all page tables, by running through page directory.
     *  Then  sweep  through all  entries in each  page table and search
     *  for resident pages.  Find the one  with the  smallest LRU entry.
     *  That one will be swapped out.
     */

    /* dump page directory */
    fprintf( swap, "Dumping Page Directory:\n" );
    for ( pd_x = 0; pd_x < P_SZ; pd_x++ ) {
        if ( pd[ pd_x ].present ) {
            pt = (page_ptr_tp) pd[ pd_x ].v;
            fprintf( swap, "pd[%d].v   = 0x%x\n", pd_x, (unsigned) pt );
            fprintf( swap, "pd[%d].lru = %d\n",   pd_x, pd[ pd_x ].lru );
        } //end if
    } /*end for*/
    fflush( swap );

    /* then dump page tables */
    fprintf( swap, "\nDumping Page Tables:\n" );
    for ( pd_x = 0; pd_x < P_SZ; pd_x++ ) {
        if ( pd[ pd_x ].present ) {
            pt = (page_ptr_tp) pd[ pd_x ].v;
            fprintf( swap, "Page Dir entry %d:\n", pd_x );
            for ( pt_x = 0; pt_x < P_SZ; pt_x++ ) {
                if ( (*pt)[ pt_x ].present ) {
                    p = (page_ptr_tp) (*pt)[ pt_x ].v;
                    fprintf( swap, "pt[%d].v = 0x%07x, ", pt_x, (unsigned) p );
                    fprintf( swap, "lru = %d", (*pt)[ pt_x ].lru );
                    if ( (*pt)[ pt_x ].dirty ) {
                        fprintf( swap, ", written" );
                    } //end if
                    fprintf( swap, "\n" );
                    fflush( swap );
                } //end if
            } /*end for*/
        } //end if
    } /*end for*/
    fflush( swap );
    trace_out( "show_page_structure" );
} /*end show_page_structure*/

void swap_out( msg )
char * msg;
{ /* swap_out */
    unsigned    lru     = TWO32;
    unsigned    x       = 0;
    unsigned    pd_x    = 0;
    unsigned    pt_x    = 0;
    BOOL        initial = TRUE;
    page_ptr_tp pt      = NULL;
    page_ptr_tp ptr;

    trace_in( "swap_out" );
    swap_out_cnt++;
    assert( ws >= max_phys_pages, "Why swap out page if room in working set?" );

    /*  Sweep though all page tables, by running through page directory.
     *  Then  sweep through  all entries  in each page  table and search
     *  for resident pages.  Find the one  with the  smallest LRU entry.
     *  That one will be swapped out.
     *
     *  In case we find nuttin, better initialize the page to be swapped
     *  out to be the first of all. At least we have good ptr value.
     */

    ptr = (page_ptr_tp )pd[ 0 ].v;
    /* initial ptr points only to page table frame address */

    for ( pd_x = 0; pd_x < P_SZ; pd_x++ ) {
        if ( pd[ pd_x ].present ) {
            pt = (page_ptr_tp) pd[ pd_x ].v;
            for ( pt_x = 0; pt_x < P_SZ; pt_x++ ) {
                if ( (*pt)[ pt_x ].present ) {
                    assert( (*pt)[ pt_x ].v != NO_VALUE, "pt has no value" );

                    /*  <= important, cos candidate found may be present.
                     *  while initial ptr value is just random.
                     *
                     *  Remember attributes of page that can be swapped out
                     */
                    if ( (*pt)[ pt_x ].lru <= lru ) {
                  /* remember all data for LRU page */
                        initial = FALSE;
                        lru = (*pt)[ pt_x ].lru;
                        ptr = pt;
                        x   = pt_x;
                    } //end if
                } //end if
            } /*end for*/
        } //end if
    } /*end for*/

    assert( !initial, "Never found a real page to be swapped out." );
    ws--;
    assert( ws < max_phys_pages, "There must be room in working set" );

    /* here we SWAP page OUT */
    assert( x >= 0, "X is <= 0" );
    assert( x < P_SZ, "X is >= P_SZ" );
    assert( ptr != NULL, "Null ptr" );
    assert( (*ptr)[ x ].v > 0, "Null (*ptr)[x].v" );
    assert( (*ptr)[ x ].v != NO_VALUE, "(*ptr)[ x ].v == NO_VALUE" );
    assert( (*ptr)[ x ].present, "How swap out if NOT present?" );

    (*ptr)[ x ].present = FALSE;

    /*  Only  if  page  is dirty  it must  physically  be  written.
     *  Otherwise we can save the write operation, cos the original
     *  page is still on disk.
     */
    if ( (*ptr)[ x ].dirty ) {
        inc( CYC_PAGE_FAULT_W );
    } //end if
    if ( want_swap ) {
        show_page_structure( " swapped out a page", msg );
    } //end if
    trace_out( "swap_out" );
} /*end swap_out*/

/*
 */
void swap_in( msg, p_frame, p_x )
char      * msg;
page_ptr_tp p_frame;
int         p_x;
{ /* swap_in */
    page_ptr_tp ptr = NULL;

    trace_in( "swap_in" );
    page_fault_cnt++;
    assert( p_x >= 0, "p_x must be >= 0" );
    assert( p_x < P_SZ, "p_x must be < P_SZ" );
    assert( ! (*p_frame)[ p_x ].present, "resident page cannot be swapped in" );
    assert( ws <= max_phys_pages, "Working set size exceeded" );
    if ( ws >= max_phys_pages ) {
        swap_out( "ws full in 'swap_in'; must 'swap_out'" );
        assert( ws < max_phys_pages, "After swap out no room in working set" );
    } //end if

    ws++;

    (*p_frame)[ p_x ].present = TRUE;
    (*p_frame)[ p_x ].lru     = 0;
    (*p_frame)[ p_x ].dirty   = FALSE;
    if ( max_ws < ws ) {
        max_ws = ws;
    } //end if
    inc( CYC_PAGE_FAULT_R );
    if ( want_swap ) {
        show_page_structure( " swapped in a page", msg );
    } //end if
    trace_out( "swap_in" );
} /*end swap_in*/




// Sample address generator, for CSc 139 homework 3
// using matrix mult. as basis to gen. referenced addresses
// actual matrix value NOT interesting; is ignored
 
#include <stdio.h>
#define DEBUG 1

// initial size of matrix is 4*4 to see program output = behavior
// for larger data sets, increase MAX 
#define MAX 4
#define BPW 4

// assumed start addresses of 3 matrices a[][] b[][] and c[][]
#define A_A 0
#define A_B ( A_A + sizeof( a ) )
#define A_C ( A_B + sizeof( b ) )

// output one store, AKA 'w'
#define S( a, v ) printf( "w %5d %5d  ", a, v );

// output one load, AKA memory read, 'r'
#define L( a    ) printf( "r %5d        ", a )
 
typedef int mat_tp[ MAX ][ MAX ];
// a and b are source, c is destination matrix
mat_tp a, b, c; // MATRIX TYPE??? WTF IS THIS

void init()
{ // init
        for( int row = 0; row < MAX; ++row ) {
                for( int col = 0; col < MAX; ++col ) {
                        a[ row ][ col ] = row + 10 * col;
                        b[ row ][ col ] = row + col + 77;
                        c[ row ][ col ] = 0;
                } //end for
        } //end for
} //end init

void print( mat_tp m, char * msg )
{ // print
        printf( "%s\n", msg );
        for( int row = 0; row < MAX; ++row ) {
                for( int col = 0; col < MAX; ++col ) {
                        printf( "%5d ", m[ row ][ col ] );
                } //end for
                printf( "\n" );
        } //end for
        printf( "\n" );
} //end for

#ifdef DEBUG
void print_all()
{ // print_all
        print( a, "matrix a[][]" );
        print( b, "matrix b[][]" );
        print( c, "matrix c[][]" );
} //end print_all
#endif

void mat_mult( mat_tp m )
{ // mat_mult
        for( int row = 0; row < MAX; ++row ) {
                for( int col = 0; col < MAX; ++col ) {
                        m[ row ][ col ] = 0;
                        for( int x = 0; x < MAX; ++x ) {
                                m[ row ][ col ] += a[ row ][ x ] * b[ x ][ col ];
                        } //end for
                } //end for
        } //end for
} //end mat_mult

void gen( )
{ // gen
        for( int row = 0; row < MAX; ++row ) {
                for( int col = 0; col < MAX; ++col ) {
                        for( int i = 0; i < MAX; ++ i ) {
                                if( ( i % 2 ) == 0 ) printf( "\n" );
                                S( A_C + BPW * ( row * MAX + col ), i );
                                L( A_C + BPW * ( row * MAX + col ) );
                                L( A_A + BPW * ( row * MAX + i ) );
                                L( A_B + BPW * ( i * MAX + col ) );
                        } //end for
                } //end for
        } //end for
        printf( "\n" );
} //end gen

int main( int argc, char ** argv )
{ // main
        //printf("%s",argv);
        printf( "If want larger matrix, change MAX. Now it is %d.\n", MAX );
        init();
#       ifdef DEBUG
        mat_mult( c ); // this line multiples a and b and stores the result in c
        print_all();
#       endif
        gen();
        statistics();
        return 0;
} //end main



// 0 * 77 + 10*78 + 20*79 + 30*80 = 4760 
