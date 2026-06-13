/* --------------------------------------------------------------------------
			   EQUILIBRIUM Library Code
			 IFF Reader/Writer Functions

				Author: Jack Thornton
-------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h>
#include "iff.h"
#include "dtypes.h"

/* IFF reader/writer scope variables */
static ID formtype;		/* ID of form for WriteFile */
static IntFuncPtr formwriter;	/* pointer to writer function for WriteFile */

/* **************************************************************************
				     CODE
************************************************************************** */

int
WriteBytesIFF(void *ptr, size_t len, FILE *fp)
{
return( fwrite(ptr, (size_t)1, len, fp)==len ? OK : IFF_ERROR );
}


int
WriteWordIFF(uword value, FILE *fp)
{
	if (fputc(*(((ubyte *)&value) + 1), fp) == IFF_ERROR)
		return IFF_ERROR;
	return (fputc(*(((ubyte *)&value)), fp) == IFF_ERROR);
}

int
WriteLongIFF(ulong value, FILE *fp)
{
    if (fputc(*(((ubyte *)&value) + 3), fp) == IFF_ERROR)
	return IFF_ERROR;
    if (fputc(*(((ubyte *)&value) + 2), fp) == IFF_ERROR)
	return IFF_ERROR;
    if (fputc(*(((ubyte *)&value) + 1), fp) == IFF_ERROR)
	return IFF_ERROR;
    return (fputc(*(((ubyte *)&value)), fp) == IFF_ERROR);
}

uword
ReadWordIFF(FILE *file)
{
static uword assemble;

	*(((ubyte *)&assemble) + 1) = fgetc(file);
	*(((ubyte *)&assemble)) = fgetc(file);
    return assemble;
}

ulong
ReadLongIFF(FILE *file)
{
static ulong assemble;

	*(((ubyte *)&assemble) + 3) = fgetc(file);
	*(((ubyte *)&assemble) + 2) = fgetc(file);
	*(((ubyte *)&assemble) + 1) = fgetc(file);
	*(((ubyte *)&assemble)) = fgetc(file);
	return assemble;
}

int
CreateChunk(fp, chunkName, writer, cbSize, aPointer, anID)
FILE *fp;		/* handle of file to create chunk in */
ID chunkName;		/* name of chunk to create */
IntFuncPtr writer;	/* pointer to routine you will be writing chunk with */
long cbSize;
void far *aPointer;	/* a generic pointer parameter to pass to the writer */
int anID;		/* a generic int parameter to pass to the writer */
{
long beginChunkPosn;	/* pointer to current file position */
long endOfChunkPosn;	/* pointer to byte after end of newly written chunk */
ulong chunkSize;	/* size of chunk as written */
long e;

	/* write out chunk ID */
	if (WriteLongIFF(chunkName, fp) == IFF_ERROR)
		return IFF_ERROR;

	/* skip past chunk size entry (write 0 length for chunk size here) */
	if (WriteLongIFF(cbSize, fp) == IFF_ERROR)
		return IFF_ERROR;

	/* remember current file position */
	beginChunkPosn = ftell(fp);

	/* write the chunk */
	if ((*writer)(fp, aPointer, anID) == IFF_ERROR) {
		fseek(fp, beginChunkPosn, SEEK_SET);
		return IFF_ERROR;
	}

	/* calculate chunk size and pad if necessary */
	chunkSize = (endOfChunkPosn = ftell(fp)) - beginChunkPosn;
	if (chunkSize & 1) {
//		chunkSize++;
		endOfChunkPosn++;
		fputc(0, fp);
	}

	if (cbSize == -1)
		{ /* rewind to write chunk size and then restore file pointer */
		/* e = */
		fseek( fp, beginChunkPosn-4, SEEK_SET );
		WriteLongIFF(chunkSize, fp);
		/* e =*/ 
		fseek( fp, endOfChunkPosn, SEEK_SET );
		}

	/* return no error */
	return OK;
}

/* ReadFileIFF
	A very simple IFF file reader, this routine ONLY processes files which
contain one FORM.  Maybe later I'll be less lazy and code a general IFF file
reader/FORM scanner to handle files with LISTS and CATS (for, say, a library
file of art or to pick out individual LBM's from an animation file).

    int ReadFileIFF(char *filename, ID expectedType, ReaderEntryPtr ReaderList)

ENTRY:
------
    "filename" is the name of the file to read
    "expectedType" contains the ID to check for (which type of form you
	are expecting the file to contain. (If the file is not of that FORM,
	this routine blows out with an error).
    "ReaderList" is a pointer to an array of "Reader Entries". A reader entry
	is a structure containing a chunk ID and a pointer to the routine to
	use to process that chunk.

    A "reader list" should look something like:

ReaderEntry mylist[] = {
    { ID_BMHD , funcToReadBMHD },
    { ID_TRAK , funcToReadTRAK },
    { ID_AUTH , funcToReadAUTH },
    { 0 , 0 }
}

    The above list would let this reader recognize three chunk types, "BMHD",
"TRAK" and "AUTH" (assuming that ID_BMHD, ID_TRAK and ID_AUTH were #defined to
equal IDs by using MakeID or something like that). If the reader encounters
a chunk matching one of the above types, the corresponding routine is called
with two parameters.

    Your chunk reading routines should be of the following format:

	    int myReader(FILE *fp, ulong chunkSize);

The file handle is passed in so you know where to read from (!!!), and the
chunk size which was reported in the file is passed in case you want to check
the chunk for valid size, or place a limit on your reading.  If reading the
chunk was successful, you MUST RETURN "OK", OR 0. Any negative returns are
considered to be error returns, and must follow the format described below.
Positive return values are currently undefined but should not be used. You
need not worry about reading the entire chunk in, or reading past the end of
the chunk, since this routine will automatically seek the file to the next
chunk position.

The list MUST END WITH A NULL ENTRY, or the results will be unpredictable (and
perhaps nasty!).


RETURNS:
--------
    OK (0) if file processed. Otherwise the high bit is set (creating a
    negative number) and the errors are as follows:
	(-1):		complex or illegal IFF file
	(-2):		not a FORM of type "expectedType"
	(-3):		chunk size error
	all others:	DOS file i/o error code with high bit set
-------------------------------------------------------------------------- */
int
ReadFileIFF(fp, expectedType, ReaderList)
FILE *fp;		    /* buffered file handle for file to read */
ID expectedType;	    /* FORM type that the file should be */
ReaderEntryPtr ReaderList;  /* list of chunk IDs and readers */
{
ID ckType;		    /* chunk ID validation buffer */
long formSize;		    /* size of form */
long ckSize;		    /* size of chunk */
ulong filePos;
int rval;		    /* return value */
int i;			    /* loop counter */

	/* read in form header and verify that it is of expected type */
	ckType = ReadLongIFF(fp);
	if (ckType != ID_FORM) {
		fclose(fp);
		return -1;			/* code for illegal or complex IFF */
	}

	/* get theoretical size of FORM and validate it */
	formSize = ReadLongIFF(fp)+4;
 
//	if ((filelength(fileno(fp)) + sizeof(ID) + sizeof(long)) != formSize) {
//		fclose(fp);
//		return -3;
//	}

	/* read and validate FORM type */
	ckType = ReadLongIFF(fp);
	if (ckType != expectedType) {
		fclose(fp);
		return -2;
	}

	/* process the chunks as we encounter them */
	while ( (formSize > 0) && !feof(fp) ) {	
		/* get the chunk type and size */
		ckType = ReadLongIFF(fp);
		ckSize = ReadLongIFF(fp);
		if (ckSize > formSize)
			break;

		/* remember where we currently are */
		filePos = ftell(fp);

		/* if we recognize this chunk, process it */
		for (i = 0; ReaderList[i].chunkID; i++) {
		if (ckType == ReaderList[i].chunkID) {
			rval = (*ReaderList[i].chunkReader)(fp, ckSize);
			if (rval < 0) {	/* if reader returns ERROR, close */
			fclose(fp);	/* file and report error */
			return rval;
			}
			break;		/* no need to keep matching */
		}
		}

		/* skip past last chunk gracefully and adjust counter */
		filePos += ckSize;
		if (ckSize & 1) {
			fgetc(fp);
			filePos++;
			formSize--;
			}
		fseek(fp, filePos, SEEK_SET);
		formSize -= (ckSize+8);
	}

	/* if sum of parts not equal whole, exit with error, otherwise OK */
	return (formSize) ? -3 : OK;
}

int
WriteFileIFF(fp, type, writer)
FILE *fp;	    /* output file handle */
ID type;
IntFuncPtr writer;
{
int rval;	    /* return value */

	/* open a form chunk and pass control to support routine */
	formtype = type;
	formwriter = writer;
	rval =	CreateChunk(fp, ID_FORM, wfType, -1L, NULL,0);

	/* exit with error/success condition */
	return rval;
}

int
wfType(fp)
FILE *fp;	    /* file to write to */
{
	/* first, write the form type */
	if (WriteLongIFF(formtype, fp) == IFF_ERROR)
		return errno | 0x8000;

	/* now continue on with writing the file */
	return (*formwriter)(fp);
}

