#ifndef IFF_H
#define IFF_H

#include <stdio.h>

/* "standard" typedefs */
typedef unsigned char ubyte;
typedef int boolean;
//typedef unsigned int uint;
typedef unsigned int uword;
typedef unsigned long ulong;
typedef unsigned void (*FuncPtr)();
typedef char (*CharFuncPtr)();
typedef int (*IntFuncPtr)();
typedef char *(*StringFuncPtr)();

#define MakeID(a,b,c,d) (ulong) ( ((ulong)(a) << 24) | ((ulong)(b) << 16) | \
			((uint)(c) << 8) | (d) )					  

typedef long ID;

typedef struct Chunk {
	ID	ckID;
	long	ckSize;
	ubyte	ckData[1];
} CHUNK;

/* reader list structure */
typedef struct {
    ID chunkID; 		/* chunk ID to match */
    IntFuncPtr chunkReader;	/* routine to call to process that chunk */
} ReaderEntry, *ReaderEntryPtr;

/* "standard" equates */
#define TRUE	    1
#define FALSE	    0
#define IFF_ERROR	    (-1)
#define OK	    0

/* globally reserved IDs */
#define ID_NULL 0L
#define ID_FORM MakeID('F','O','R','M')
#define ID_LIST MakeID('L','I','S','T')
#define ID_PROP MakeID('P','R','O','P')
#define ID_CAT	MakeID('C','A','T',' ')
#define ID_FILLER	makeID(' ',' ',' ',' ')

#define	WriteByteIFF(b,fp) fputc(b,fp)

/* function prototypes */
uword	    ReadWordIFF(FILE *fp);
ulong	    ReadLongIFF(FILE *fp);
int     WriteBytesIFF(void *ptr, size_t len, FILE *fp);
int	    WriteWordIFF(unsigned int value, FILE *fp);
int	    WriteLongIFF(unsigned long value, FILE *fp);
FILE *	    OpenIFFContext(char *file, char *flags);
int	    CloseIFFContext(FILE *fp);
// int	    CreateChunk(/*FILE *fp, ID chunkName, int cbSize, IntFuncPtr writer, ...*/);
int	    wfType(FILE *fp);
int	    WriteFileIFF(FILE *fp, ID type, IntFuncPtr writer);
int	    ReadFileIFF(FILE *fp, ID expectedType, ReaderEntryPtr ReaderList);
int CreateChunk(FILE *,ID,IntFuncPtr,long,void far *,int );

#endif

