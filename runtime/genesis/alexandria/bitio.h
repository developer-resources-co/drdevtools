/* --------------------------------------------------------------------------

                    Alexandria Genesis Library
					--------------------------

Title:			Standard Definitions

File:			BITIO.H

Description:	This file is used with the huffman encoding/decoding
				routines

See Also:		HUFF.C

-------------------------------------------------------------------------- */



#ifndef _BITIO_H
#define _BITIO_H

#include <stdio.h>
#include <alexdef.h>



typedef struct bit_file
{
	UBYTE	*addr;
    UBYTE	mask;
    short	rack;
} BIT_FILE;



BIT_FILE	*OpenInputBitFile( UBYTE *srcBuffer );
short		InputBit( BIT_FILE *bit_file );
ULONG		InputBits( BIT_FILE *bit_file, short bit_count );
UBYTE		InputByte( BIT_FILE *bit_file );
void		CloseInputBitFile( BIT_FILE *bit_file );

#endif



/* EOF -- BITIO.H */

