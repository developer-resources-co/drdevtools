//==============================================================================
// iff.cpp: iff read/writer
// By Kevin T. Seghetti
// (c) 1992 Adept Creations
//==============================================================================

#include <iostream.h>
//#include <fstream.h>

#include <pclib/iff.hpp>

//=============================================================================
// add a chunk to the list of chunks we know how to parse

void
iffRead::RegisterChunk(iffTag chunkName, errorcode procedure(iffRead *readPtr))
{
	iffChunk *chunkPtr;
	chunkPtr = new iffChunk(chunkName, procedure);
	chunkList.Insert(chunkPtr);
}

//=============================================================================

boolean
iffRead::NextForm()
{
	iffTag tagName;

	if(formBytesLeft)
	 {
		SkipBytes((istream &)*streamPtr,formLen);		// skip previous form, if any
		if(formLen & 1)
			SkipBytes((istream &)*streamPtr,1);				// if odd, skip one more byte
	 }

	chunkLen = chunkBytesLeft = 0;								// kts 6/6/92
	tagName = ::ReadIFFTag((istream &)*streamPtr);

	currentChunkName = chunkLen = chunkBytesLeft = 0;

	if(tagName == tagFORM)
	 {
		formLen = formBytesLeft = ::ReadIFFLength((istream &)*streamPtr);
		currentFormName = ReadIFFTag((istream &)*streamPtr);
		return(boolean::TRUE);
		// input stream is now positioned to read 1st chunk tag
	 }
	currentFormName = formLen = formBytesLeft = 0;
	return(boolean::FALSE);
}

//=============================================================================

boolean
iffRead::NextChunk()
{
	if(formBytesLeft)									// only works if we are in a form
	 {
		if(chunkBytesLeft)						// if we are already in a chunk, skip the rest of it
			SkipBytes((istream &)*streamPtr,chunkBytesLeft);

		if(chunkLen & 1)						// if chunk length odd, skip once more
			SkipBytes((istream &)*streamPtr,1);

		if(formBytesLeft)
		 {
			currentChunkName = ReadIFFTag((istream &)*streamPtr);
		   	chunkLen = chunkBytesLeft = ReadIFFLength((istream &)*streamPtr);
			return(boolean::TRUE);
			// input stream is now positioned to read chunk data
		 }
	 }
	return(boolean::FALSE);
}

//==============================================================================

boolean
iffRead::FindForm(iffTag name)
{
	while(streamPtr->rdstate() == ios::goodbit)
	 {
		if(GetForm() == name)
			return(boolean::TRUE);
		NextForm();
	 }
	return(boolean::FALSE);
}

//==============================================================================

boolean
iffRead::FindChunk(iffTag name)
{
	while(GetFormBytesLeft())
	 {
	 	if(GetChunk() == name)
			return(boolean::TRUE);
		NextChunk();
	 }
	return(boolean::FALSE);
}

//==============================================================================

ubyte
iffRead::GetChunkByte(void)
{
	ubyte temp;
	if(formBytesLeft && chunkBytesLeft)
	 {
		chunkBytesLeft--;
		formBytesLeft--;
		streamPtr->get(temp);
		return(temp);
	 }
	return(0);
}

//=============================================================================

uword
iffRead::GetChunkWord(void)
{
	uword temp;
	if((formBytesLeft > 1) && (chunkBytesLeft > 1))
	 {
		chunkBytesLeft-=2;
		formBytesLeft-=2;

		temp = ReadIFFWord(*streamPtr);
		return(temp);
	 }
	return(0);
}

//=============================================================================

ulong
iffRead::GetChunkLong(void)
{
	ulong temp;
	if((formBytesLeft > 3) && (chunkBytesLeft > 3))
	 {
		chunkBytesLeft-=4;
		formBytesLeft-=4;

		temp = ReadIFFLong(*streamPtr);
		return(temp);
	 }
	return(0);
}

//=============================================================================

ulong
iffRead::GetChunkData(ubyte *buffer)
{
	ulong temp = chunkBytesLeft;
	streamPtr->read(buffer,chunkBytesLeft);
	formBytesLeft -= chunkBytesLeft;
	chunkLen = chunkBytesLeft = 0;
	return(temp);
}

//=============================================================================

ulong
iffRead::GetChunkData(ubyte *buffer, ulong count)
{
	if(count > chunkBytesLeft)
		count = chunkBytesLeft;
	streamPtr->read(buffer,count);
	formBytesLeft -= count;
	chunkBytesLeft -= count;
	return(count);
}

//=============================================================================
// actually go through the iff file, reading chunks, and dispatching them
// as we go

void
iffRead::ParseChunks(void)
{
	boolean cont = boolean::TRUE;
	errorcode err;
	iffChunk *chunk;
	while(formBytesLeft > 0)
	 {
		NextChunk();
		if(formBytesLeft)
		 {
			chunk = chunkList.GetNext();
			while(chunk != 0 && (chunk->GetName() != currentChunkName))
				chunk = chunk->GetNext();
			if(chunk)
				err = chunk->CallProc(this);
		 }
	 }
}

//=============================================================================


/* --------------------------------------------------------------------------

			 IFF Writer Functions

-------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h>
#include <pclib/iff.hpp>
#include <pclib/general.hpp>

/* IFF reader/writer scope variables */
static iffTag formtype;		/* ID of form for WriteFile */
static IffFuncPtr formwriter;	/* pointer to writer function for WriteFile */


//==============================================================================
// kts iff writer

//=============================================================================

boolean
iffWrite::NewForm(iffTag formName)
{
	EndForm();

	::WriteIFFTag((ostream &)*streamPtr,tagFORM);
	formLenPos = streamPtr->tellp();
	::WriteIFFLength((ostream &)*streamPtr,0);				// not known yet
	WriteIFFTag((ostream &)*streamPtr,formName);

	// output stream is now positioned to Write 1st chunk tag
	currentFormName = formName;
	return(boolean::FALSE);
}

//==============================================================================

boolean
iffWrite::NewChunk(iffTag chunkName)
{
	EndChunk();

	WriteIFFTag((ostream &)*streamPtr,chunkName);
	chunkLenPos = streamPtr->tellp();
	WriteIFFLength((ostream &)*streamPtr,0);				// not known yet

	// output stream is now positioned to Write 1st chunk tag
	currentChunkName = chunkName;
	chunkLen = 0;
	return(boolean::FALSE);
}

//=============================================================================

boolean
iffWrite::EndForm()
{
	streampos temp;

	EndChunk();

	if(formLen)
	 {
		// code to seek to length and write it, then return to current position
		temp = streamPtr->tellp();
		streamPtr->seekp(formLenPos,ios::beg);
		::WriteIFFLength((ostream &)*streamPtr,formLen);
		streamPtr->seekp(temp,ios::beg);

		currentFormName = formLen = formLenPos = 0;
	 }
	return(boolean::TRUE);
}

//==============================================================================

boolean
iffWrite::EndChunk()
{
	streampos temp;

	if(chunkLen)
	 {
		// code to seek to length and write it, then return to current position
		temp = streamPtr->tellp();
		streamPtr->seekp(chunkLenPos,ios::beg);
		::WriteIFFLength((ostream &)*streamPtr,chunkLen);
		streamPtr->seekp(temp,ios::beg);
		ChunkWordAlign();

		currentChunkName = chunkLen = chunkLenPos = 0;
	 }
	return(boolean::TRUE);
}

//=============================================================================

boolean
iffWrite::WriteChunkByte(ubyte val)
{
	chunkLen++;
	formLen++;
	streamPtr->put(val);
	return(boolean::TRUE);
}

//=============================================================================

boolean
iffWrite::WriteChunkWord(uword val)
{
	chunkLen+=2;
	formLen+=2;
	return(WriteIFFWord(*streamPtr,val));
}

//=============================================================================

boolean
iffWrite::WriteChunkLong(ulong val)
{
	chunkLen+=4;
	formLen+=4;
	return(WriteIFFLong(*streamPtr,val));
}

//=============================================================================

boolean
iffWrite::WriteChunkData(ubyte *buffer, ulong count)
{
	streamPtr->write(buffer,count);
	formLen += count;
	chunkLen += count;
	return(boolean::TRUE);
}

//==============================================================================

/* **************************************************************************
				     CODE
************************************************************************** */

errorcode
	WriteBytesIFF(void *ptr, size_t len, FILE *fp)
	{
	return( fwrite(ptr, (size_t)1, len, fp)==len ? OK : IFF_ERROR );
	}


errorcode
WriteWordIFF(uword value, FILE *fp)
	{
	if (fputc(*(((ubyte *)&value) + 1), fp) == IFF_ERROR)
		return IFF_ERROR;
	return (fputc(*(((ubyte *)&value)), fp) == IFF_ERROR);
	}

errorcode
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

errorcode
CreateChunk(
	FILE *fp,		/* handle of file to create chunk in */
	iffTag chunkName,		/* name of chunk to create */
	IffFuncPtr writer,	/* pointer to routine you will be writing chunk with */
	long cbSize
	)
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
	if ((*writer)(fp /*, aPointer, anID*/ ) == IFF_ERROR) {
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


errorcode
wfType(
	FILE *fp	    /* file to write to */
	)
{
	/* first, write the form type */
	if (WriteLongIFF(formtype, fp) == IFF_ERROR)
		return errno | 0x8000;

	/* now continue on with writing the file */
	return (*formwriter)(fp);
}

errorcode
WriteFileIFF(
	FILE *fp,	    /* output file handle */
	iffTag type,
	IffFuncPtr writer
	)
	{
	/* open a form chunk and pass control to support routine */
	formtype = type;
	formwriter = writer;
	return( CreateChunk(fp, tagFORM, (IffFuncPtr)wfType) );
	}




#if 0
/* IFF reader/writer scope variables */
static iffTag formtype;		/* ID of form for WriteFile */
static IffFuncPtr formwriter;	/* pointer to writer function for WriteFile */

////////////////////////////////////////////////////////////////////////////////

int
CreateChunk(
	ofstream& output,		/* handle of file to create chunk in */
	iffTag chunkName,		/* name of chunk to create */
	IffFuncPtr writer,	/* pointer to routine you will be writing chunk with */
	long cbSize
	)
{
long beginChunkPosn;	/* pointer to current file position */
long endOfChunkPosn;	/* pointer to byte after end of newly written chunk */
ulong chunkSize;	/* size of chunk as written */
long e;

	/* write out chunk ID */
	WriteIFFTag(output, chunkName);

	/* skip past chunk size entry (write 0 length for chunk size here) */
	WriteIFFLength(output, cbSize);

	/* remember current file position */
//$	beginChunkPosn = output.tellg();

	/* write the chunk */
	(*writer)(output);

	/* calculate chunk size and pad if necessary */
	chunkSize = (endOfChunkPosn = output.tellg()) - beginChunkPosn;
	if (chunkSize & 1) {
		endOfChunkPosn++;
		output.put( (char)0 );
	}

	if (cbSize == -1L)
		{ /* rewind to write chunk size and then restore file pointer */
		/* e = */
//		fseek( output, beginChunkPosn-4, SEEK_SET );
		WriteIFFLong(output, chunkSize);
		/* e =*/
//		fseek( output, endOfChunkPosn, SEEK_SET );
		}

	/* return no error */
	return OK;
}

errorcode
	wfType( ofstream& output )
	{
	/* first, write the form type */
	WriteIFFTag(output, formtype);

	/* now continue on with writing the file */
	return (*formwriter)(output);
	}

int
WriteFileIFF(
	FILE *output,	    /* output file handle */
	iffTag type,
	IffFuncPtr writer
	)
{
int rval;	    /* return value */

	/* open a form chunk and pass control to support routine */
	formtype = type;
	formwriter = writer;
	rval = CreateChunk(output, tagFORM, (IffFuncPtr)wfType, -1L, NULL, 0);

	/* exit with error/success condition */
	return rval;
}
#endif



//=============================================================================


