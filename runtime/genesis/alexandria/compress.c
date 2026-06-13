/*============================================================================*/
/* compress.c:
/*============================================================================*/

#include <stdlib.h>
#include <string.h>

#include "genesis.h"
#include "vdp.h"
#include "compress.h"
#include "assert.h"
#include "debug.h"
#include "cdebug.h"

/*============================================================================*/

UBYTE *compOutBufferPtr;

/*============================================================================*/

void DecodeAdaptiveHuffman(UBYTE *data,UBYTE *destBuffer,long size);
void DecodeAdaptiveHuffmanToVDP(UBYTE *data,UWORD vdpAddr,long size);

void DecodeHuffman(UBYTE *data,UBYTE *destBuffer,long size);
void DecodeHuffmanToVDP(UBYTE *data,UWORD vdpAddr,long size);

/*============================================================================*/

void
ComprWriteByte(unsigned char c)
{
	*compOutBufferPtr++ = c;
}

/*============================================================================*/

UWORD otherByte;
UWORD decompPhase;

void
ComprWriteVDPByte(unsigned char c)
{
	UWORD data;
	decompPhase = decompPhase ^ 1;
	if(decompPhase)
		otherByte = c;
	else
	 {
		data = (otherByte << 8) | c;
		*(UWORD *)VDP_DATA = data;
	 }
}

/*============================================================================*/

void
DecompressToRam(TComprData *data,UBYTE *destBuffer)
{
/*	ULONG len = data->dataLen;*/

	assertString(data->dataType != DATATYPE_INVALID,"invalid data type");

	switch(data->compType)
	 {
		case COMPTYPE_NONE:
			Crash("unknown compression type");
			memcpy(destBuffer,&data->data[0],data->dataLen);
			break;
		case COMPTYPE_ADAPTIVEHUFFMAN:
			DecodeAdaptiveHuffman(&data->data[0],destBuffer,data->dataLen);
			break;
		case COMPTYPE_HUFFMAN:
			compOutBufferPtr = destBuffer;
			DecodeHuffman(&data->data[0],ComprWriteByte,data->dataLen);
			break;
		default:
			Crash("unknown compression type");
			break;
	 }
}

/*============================================================================*/

void
DecompressToVDP(TComprData *data,UWORD vdpAddr)
{
#ifdef SEGA_CD
	Crash( "DecompressToVDP() not supported" );
#endif

/*	ULONG len = data->dataLen;*/
	assertString(data->dataType != DATATYPE_INVALID,"invalid data type");

	VDPAddress(vdpAddr);

	switch(data->compType)
	 {
		case COMPTYPE_NONE:
			Crash("unknown compression type");
			break;
		case COMPTYPE_ADAPTIVEHUFFMAN:
			DecodeAdaptiveHuffmanToVDP(&data->data[0],vdpAddr,data->dataLen);
			break;
		case COMPTYPE_HUFFMAN:
			decompPhase = 0;
			VDPAddress(vdpAddr);
			DecodeHuffman(&data->data[0],ComprWriteVDPByte,data->dataLen);
			break;
		default:
			Crash("unknown compression type");
			break;
	 }
}

/*============================================================================*/
