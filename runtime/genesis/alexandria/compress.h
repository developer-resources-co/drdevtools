/*============================================================================*/
/* compress.h: data decompression routines */
/*============================================================================*/

#ifndef _COMPRESS_H_
#define _COMPRESS_H_

#include <alexdef.h>

/*============================================================================*/

enum
{
	DATATYPE_INVALID = 0,
	DATATYPE_UNKNOWN
};

enum
{
	COMPTYPE_INVALID= 0,
	COMPTYPE_NONE,
	COMPTYPE_ADAPTIVEHUFFMAN,
	COMPTYPE_RLE,
	COMPTYPE_HUFFMAN
};

/*============================================================================*/

typedef struct comprData
{
	UBYTE compType;
	UBYTE dataType;
	ULONG dataLen;
	UBYTE data[1];		/* this is really UBYTE data[?] */
} TComprData;


/*============================================================================*/

void
DecompressToRam(TComprData *data, UBYTE *buffer);

void
DecompressToVDP(TComprData *data, UWORD vdpAddr);

/*============================================================================*/

#endif

/*============================================================================*/
