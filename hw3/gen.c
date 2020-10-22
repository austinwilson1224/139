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
mat_tp a, b, c;

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

int main( void )
{ // main
        printf( "If want larger matrix, change MAX. Now it is %d.\n", MAX );
        init();
#       ifdef DEBUG
        mat_mult( c );
        // print_all();
#       endif
        gen();
        return 0;
} //end main

