/* --------------------------------------------------------------------------

                    Alexandria Genesis Library
					--------------------------

Title:			Standard Definitions

File:			BITIO.C

Description:	This file is used with the huffman encoding/decoding
				routines

See Also:		HUFF.C

-------------------------------------------------------------------------- */



#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "bitio.h"
#include "assert.h"
#include "alexdef.h"



PUBLIC BIT_FILE *
OpenInputBitFile(UBYTE  *addr )
{
    BIT_FILE *bit_file;

    bit_file = (BIT_FILE *) calloc( 1, sizeof( BIT_FILE ) );

    if ( bit_file == NULL )
		return( bit_file );

	bit_file->addr = addr;
    bit_file->rack = 0;
    bit_file->mask = 0x80;

    return( bit_file );
}




PUBLIC void
CloseInputBitFile(BIT_FILE *bit_file )
{
    free( (char *) bit_file );
}




PUBLIC short
InputBit( BIT_FILE *bit_file )
{
    short value;

    if ( bit_file->mask == 0x80 )
	{
        bit_file->rack = *bit_file->addr++;

/*      if ( bit_file->rack == EOF )
            fatal_error( "Fatal error in InputBit!\n" ); */

		assert(bit_file->rack != EOF);
    }

    value = bit_file->rack & bit_file->mask;
    bit_file->mask >>= 1;

    if ( bit_file->mask == 0 )
		bit_file->mask = 0x80;

    return( value ? 1 : 0 );
}




PUBLIC ULONG
InputBits(BIT_FILE * bit_file, short bit_count )
{
    unsigned long mask;
    unsigned long return_value;

    mask = 1L << ( bit_count - 1 );
    return_value = 0;

    while ( mask != 0 )
	{
		if ( bit_file->mask == 0x80 )
		{
	    	bit_file->rack = *bit_file->addr++;

/*	    	if ( bit_file->rack == EOF )
				fatal_error( "Fatal error in InputBit!\n" ); */

			assert(bit_file->rack != EOF);
	 	}

		if ( bit_file->rack & bit_file->mask )
        	return_value |= mask;

        mask >>= 1;
        bit_file->mask >>= 1;

        if ( bit_file->mask == 0 )
        	bit_file->mask = 0x80;
    }

    return( return_value );
}

PUBLIC UBYTE
InputByte(BIT_FILE *bit_file)
{
	return(*bit_file->addr++);
}




/* EOF -- BITIO.C */

