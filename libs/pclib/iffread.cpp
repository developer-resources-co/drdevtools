//==============================================================================
// iff.cpp: iff read/writer
// By Kevin T. Seghetti
// (c) 1992 Adept Creations
//==============================================================================

#include <iostream.h>

#include <pclib/iff.hpp>

//=============================================================================
// add a chunk ot the list of chunks we know how to parse

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

	//		streamPtr->seekg(chunkLen,ios::cur);

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
//=============================================================================


