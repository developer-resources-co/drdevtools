/************************** Start of HUFF.C *************************
 *
 * This is the Huffman coding module used in Chapter 3.
 * Compile with BITIO.C, ERRHAND.C, and either MAIN-C.C or MAIN-E.C
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "debug.h"
#include "bitio.h"
#include "alexdef.h"

/*
 * The NODE structure is a node in the Huffman decoding tree.  It has a
 * count, which is its weight in the tree, and the node numbers of its
 * two children.  The saved_count member of the structure is only
 * there for debugging purposes, and can be safely taken out at any
 * time.  It just holds the intial count for each of the symbols, since
 * the count member is continually being modified as the tree grows.
 */
typedef struct tree_node {
    unsigned short count;
    unsigned short saved_count;
    short child_0;
    short child_1;
} NODE;

/*
 * A Huffman tree is set up for decoding, not encoding.  When encoding,
 * I first walk through the tree and build up a table of codes for
 * each symbol.  The codes are stored in this CODE structure.
 */
typedef struct code {
    unsigned short code;
    short code_bits;
} CODE;

/*
 * The special EOS symbol is 256, the first available symbol after all
 * of the possible bytes.  When decoding, reading this symbols
 * indicates that all of the data has been read in.
 */
#define END_OF_STREAM 256

/*
 * Local function prototypes, defined with or without ANSI prototypes.
 */

short build_tree( NODE *nodes );
void input_counts( BIT_FILE *input, NODE *nodes );
void expand_data( BIT_FILE *input, void (*writeRoutine)(unsigned char),
					NODE *nodes, short root_node );

/*============================================================================*/
/*
 * ExpandFile is the routine called by MAIN-E.C to expand a file that
 * has been compressed with order 0 Huffman coding.  This routine has
 * a simpler job than that of the Compression routine.  All it has to
 * do is read in the counts that have been stored in the compressed
 * file, then build the Huffman tree.  The data can then be expanded
 * by reading in a bit at a time from the compressed file.  Finally,
 * the node array is freed and the routine returns.
 *
 */

void ExpandFile( BIT_FILE *input, void (*writeRoutine)(unsigned char))
{
    NODE *nodes;
    short root_node;

    if ( ( nodes = (NODE *) calloc( 514, sizeof( NODE ) ) ) == NULL )
        Crash( "Error allocating nodes array\n" );
    input_counts( input, nodes );
    root_node = build_tree( nodes );
    expand_data( input, writeRoutine, nodes, root_node );
    free( (char *) nodes );
}


void DecodeHuffman(UBYTE *srcBuffer,void (*writeRoutine)(unsigned char))
{
	BIT_FILE *input = OpenInputBitFile(srcBuffer);
	ExpandFile(input,writeRoutine);
	CloseInputBitFile(input);
}


/*
 * When expanding, I have to read in the same set of counts.  This is
 * quite a bit easier that the process of writing them out, since no
 * decision making needs to be done.  All I do is read in first, check
 * to see if I am all done, and if not, read in last and a string of
 * counts.
 */

void input_counts( input, nodes )
BIT_FILE *input;
NODE *nodes;
{
    short first;
    short last;
    short i;
    short c;

    for ( i = 0 ; i < 256 ; i++ )
	nodes[ i ].count = 0;
    if ( ( first = InputByte( input ) ) == EOF )
	Crash( "Error reading byte counts\n" );
    if ( ( last = InputByte( input ) ) == EOF )
	Crash( "Error reading byte counts\n" );
    for ( ; ; ) {
	for ( i = first ; i <= last ; i++ )
	    if ( ( c = InputByte(input ) ) == EOF )
		Crash( "Error reading byte counts\n" );
	    else
		nodes[ i ].count = (unsigned short) c;
	if ( ( first = InputByte( input ) ) == EOF )
	    Crash( "Error reading byte counts\n" );
	if ( first == 0 )
	    break;
	if ( ( last = InputByte( input ) ) == EOF )
	    Crash( "Error reading byte counts\n" );
    }
    nodes[ END_OF_STREAM ].count = 1;
}

/*
 * This routine counts the frequency of occurence of every byte in
 * the input file.  It marks the place in the input stream where it
 * started, counts up all the bytes, then returns to the place where
 * it started.  In most C implementations, the length of a file
 * cannot exceed an unsigned long, so this routine should always
 * work.
 */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif


/*
 * Building the Huffman tree is fairly simple.  All of the active nodes
 * are scanned in order to locate the two nodes with the minimum
 * weights.  These two weights are added together and assigned to a new
 * node.  The new node makes the two minimum nodes into its 0 child
 * and 1 child.  The two minimum nodes are then marked as inactive.
 * This process repeats until their is only one node left, which is the
 * root node.  The tree is done, and the root node is passed back
 * to the calling routine.
 *
 * Node 513 is used here to arbitratily provide a node with a guaranteed
 * maximum value.  It starts off being min_1 and min_2.  After all active
 * nodes have been scanned, I can tell if there is only one active node
 * left by checking to see if min_1 is still 513.
 */
short build_tree( nodes )
NODE *nodes;
{
    short next_free;
    short i;
    short min_1;
    short min_2;

    nodes[ 513 ].count = 0xffff;
    for ( next_free = END_OF_STREAM + 1 ; ; next_free++ ) {
	min_1 = 513;
	min_2 = 513;
	for ( i = 0 ; i < next_free ; i++ )
            if ( nodes[ i ].count != 0 ) {
                if ( nodes[ i ].count < nodes[ min_1 ].count ) {
                    min_2 = min_1;
                    min_1 = i;
                } else if ( nodes[ i ].count < nodes[ min_2 ].count )
                    min_2 = i;
            }
	if ( min_2 == 513 )
	    break;
	nodes[ next_free ].count = nodes[ min_1 ].count
	                           + nodes[ min_2 ].count;
        nodes[ min_1 ].saved_count = nodes[ min_1 ].count;
        nodes[ min_1 ].count = 0;
        nodes[ min_2 ].saved_count =  nodes[ min_2 ].count;
        nodes[ min_2 ].count = 0;
	nodes[ next_free ].child_0 = min_1;
	nodes[ next_free ].child_1 = min_2;
    }
    next_free--;
    nodes[ next_free ].saved_count = nodes[ next_free ].count;
    return( next_free );
}


/*
 * Expanding compressed data is a little harder than the compression
 * phase.  As each new symbol is decoded, the tree is traversed,
 * starting at the root node, reading a bit in, and taking either the
 * child_0 or child_1 path.  Eventually, the tree winds down to a
 * leaf node, and the corresponding symbol is output.  If the symbol
 * is the END_OF_STREAM symbol, it doesn't get written out, and
 * instead the whole process terminates.
 */
void expand_data( BIT_FILE *input, void (*writeRoutine)(unsigned char), NODE *nodes, short root_node )
{
    short node;

    for ( ; ; ) {
        node = root_node;
        do {
            if ( InputBit( input ) )
                node = nodes[ node ].child_1;
            else
                node = nodes[ node ].child_0;
        } while ( node > END_OF_STREAM );
	if ( node == END_OF_STREAM )
            break;
		(*writeRoutine)((unsigned char)node);
    }
}
/*************************** End of HUFF.C **************************/
