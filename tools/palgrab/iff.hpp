//==============================================================================
// iff.h: header for iff read/writer
// By Kevin T. Seghetti
// (c) 1992 Adept Creations
//==============================================================================
/*

Documentation:

	Abstract:

	History:
			Created	? Kevin T. Seghetti
			First Documented 10-19-92 02:23pm

	Class Hierarchy:

	Dependancies:
		general
		list

	Restrictions:

	Example:

*/

//==============================================================================
// include only once insurance

#ifndef PCLIB_iff_H
#define PCLIB_iff_H

//==============================================================================
// dependencies

#include <iostream.h>

#include <pclib/general.hpp>
#include <pclib/list.hpp>

//==============================================================================
// globals

#define MakeTag(a,b,c,d) (((ulong)a)<<24) | (((ulong)b)<<16) | (((ulong)c)<<8) | ((ulong)d)

//==============================================================================

typedef ulong iffTag;
typedef ubyte memChunk;

const iffTag tagFORM = MakeTag('F','O','R','M');
const iffTag tagLIST = MakeTag('L','I','S','T');
const iffTag tagCAT  = MakeTag('C','A','T',' ');
const iffTag tagPROP = MakeTag('P','R','O','P');
const iffTag tagFiller = MakeTag(' ',' ',' ',' ');
const iffTag tagNULL = MakeTag(0,0,0,0);
const iffTag tagCopyright = MakeTag('(','c',')',' ');

//==============================================================================
// iff io functions

#if 0
inline void SkipBytes(istream &input,ulong len)
{
		unsigned char temp;
		while(len--)
			input.get(temp);
}
#endif

//==============================================================================
// global inlines

inline ulong ReadIFFLong(istream &input)
{
		ulong data;
		ubyte temp;
		input.get(temp);
		data = ((ulong)temp) << 24;
		input.get(temp);
		data |= ((ulong)temp) << 16;
		input.get(temp);
		data |= ((ulong)temp) << 8;
		input.get(temp);
		data |= ((ulong)temp);
		return(data);
}

//==============================================================================

inline uword ReadIFFWord(istream &input)
{
		uword data;
		ubyte temp;
		input.get(temp);
		data = ((ulong)temp) << 8;
		input.get(temp);
		data |= ((ulong)temp);
		return(data);
}

//==============================================================================

inline boolean WriteIFFLong(ostream &output,ulong data)
{
		output.put((char)(data >> 24));
		output.put((char)(data >> 16));
		output.put((char)(data >> 8));
		output.put((char)data);
		return(boolean::TRUE);
}

//==============================================================================

inline boolean WriteIFFWord(ostream &output,uword data)
{
		output.put((char)(data >> 8));
		output.put((char)data);
		return(boolean::TRUE);
}

//==============================================================================
// debugging functions

void
inline PrintTag(iffTag tag)
{
		cout << (uchar)(tag >> 24);
		cout << (uchar)(tag >> 16);
		cout << (uchar)(tag >> 8);
		cout << (uchar)(tag);
}

//==============================================================================

inline iffTag ReadIFFTag(istream &input)
{
		return((iffTag)ReadIFFLong(input));
}

inline ulong ReadIFFLength(istream &input)
{
		return(ReadIFFLong(input));
}

inline iffTag WriteIFFTag(ostream &output,iffTag tag)
{
		return(WriteIFFLong(output,(iffTag)tag));
}

inline ulong WriteIFFLength(ostream &output, ulong len)
{
		return(WriteIFFLong(output,len));
}

//==============================================================================

class iffRead;

class iffChunk : public nodeInterface<iffChunk>
{
	iffTag chunkName;
	errorcode (*proc)(iffRead *readPtr);
	iffChunk() {}								// MUST init with name & procedure ptr
public:
	iffChunk(iffTag name,errorcode (*pr)(iffRead *readPtr)) : nodeInterface<iffChunk>()
	 {
		proc = pr;
		chunkName = name;
	 }
	iffTag GetName() const {return(chunkName); }
	errorcode CallProc(iffRead *readPtr) {return((*proc)(readPtr)); }
};

//==============================================================================
// class declaration

class iffRead
{
	iffTag currentFormName;
	ulong formLen;
	ulong formBytesLeft;
	iffTag currentChunkName;
	ulong chunkLen;
	ulong chunkBytesLeft;
	list<iffChunk> chunkList;
	istream *streamPtr;
	iffRead() {}											// MUST init with a stream ptr

	iffTag ReadIFFTag(istream &input) { formBytesLeft-=4; return((iffTag)ReadIFFLong(input)); }
	ulong ReadIFFLength(istream &input) { formBytesLeft-=4; return(ReadIFFLong(input)); }

	void SkipBytes(istream &input,ulong len)
	 {
		unsigned char temp;
		formBytesLeft -= len;
		while(len--)
			input.get(temp);
		if(!(input.rdstate() == ios::goodbit))
			formBytesLeft = chunkBytesLeft = 0;			// error
	 }
public:
// initializers
	iffRead(istream &str) : chunkList()
	 {
		currentFormName = currentChunkName = 0;
		formLen = formBytesLeft = chunkLen = chunkBytesLeft = 0;
		streamPtr = &str;
	 }

	iffRead(istream *str) : chunkList()
	 {
		currentFormName = currentChunkName  = 0;
		formLen = formBytesLeft = chunkLen = chunkBytesLeft = 0;
		streamPtr = str;
	 }

// current state readers
	iffTag GetForm() 			const	{return(currentFormName);}		// return name of current form
	iffTag GetFormLen()			const	{return(formLen);}				// return size of current form(0 if none)
	iffTag GetFormBytesLeft()	const	{return(formBytesLeft);}		// return size of current form(0 if none)
	iffTag GetChunk()			const	{return(currentChunkName);}	   	// return name of current chunk
	iffTag GetChunkLen()		const	{return(chunkLen);}	   			// return size of current chunk(0 if none)
	iffTag GetChunkBytesLeft()	const	{return(chunkBytesLeft);}		// return # of bytes left to read in the current chunk

// manual control
	boolean NextForm();									// goes to next form in file, if available
	boolean FindForm(iffTag name);					 		// scan through file looking for a particular form
	boolean NextChunk();							 		// goes to next chunk in file, if available
	boolean FindChunk(iffTag name);						// scan through form looking for chunk
	ubyte GetChunkByte();					 	   		// returns next byte in chunk
	uword GetChunkWord();					 	   		// returns next word in chunk
	ulong GetChunkLong();					 	   		// returns next long word in chunk
	ulong GetChunkData(ubyte *buffer);		 	   		// reads entire chunk into buffer(note: buffer must be large enough to hold current chunk)
	ulong GetChunkData(ubyte *buffer,ulong count); 		// reads portion of chunk into buffer(note: buffer must be large enough to hold current chunk)
	void ChunkWordAlign() { if(chunkBytesLeft & 1) GetChunkByte(); } 						// aligns reader to word compared to beginning of chunk

// automatic chunk parsing control
	void ClearChunks() { chunkList.DeleteAll();	}										// remove all chunks from list
	void RegisterChunk(iffTag chunkName, errorcode procedure(iffRead *readPtr));
	void ParseChunks(void);	   							// parse all chunks in this form,
};

//==============================================================================
// kts IFF writer

class iffWrite
{
	iffTag currentFormName;
	ulong formLen;
	streampos formLenPos;				// pointer to file position of form length
	iffTag currentChunkName;
	ulong chunkLen;
	streampos chunkLenPos;			// pointer to file position of chunk length
	ostream *streamPtr;
	iffWrite() {}											// MUST init with a stream ptr

	boolean WriteIFFTag(ostream &output,iffTag t) { formLen += 4; return(WriteIFFLong(output,t)); }
	boolean WriteIFFLength(ostream &output,ulong len) { formLen += 4;  return(WriteIFFLong(output,len)); }
public:
	iffWrite(ostream *str)
	 {
		currentFormName = 0;
		formLen = chunkLen = 0;
		streamPtr = str;
	 }

	iffWrite(ostream &str)
	 {
		currentFormName = 0;
		formLen = chunkLen = 0;
		streamPtr = &str;
	 }


	~iffWrite()
	 {
		EndForm();
	 }

// current state readers
	iffTag GetForm() 			const	{return(currentFormName);}		// return name of current form
	iffTag GetChunk()			const	{return(currentChunkName);}	   	// return name of current chunk

// manual control
	boolean NewForm(iffTag formName);	 				// creates new form in output file
	boolean EndForm(void); 				 				// closes current form, updating len, etc
	boolean NewChunk(iffTag chunkName);		 		// create new chunk in form, if form is open
	boolean EndChunk(void);		  						// closes current chunk, updating len, etc
	boolean WriteChunkByte(ubyte val);					 	   		// returns next byte in chunk
	boolean WriteChunkWord(uword val);					 	   		// returns next word in chunk
	boolean WriteChunkLong(ulong val);					 	   		// returns next long word in chunk
	boolean WriteChunkData(ubyte *buffer,ulong count); 		// reads portion of chunk into buffer(note: buffer must be large enough to hold current chunk)
	void ChunkWordAlign() { if(chunkLen & 1) WriteChunkByte(0); } 						// aligns reader to word compared to beginning of chunk
};

//==============================================================================
// old Bill IFF writer
// class declaration
// under construction

#include <stdio.h>

typedef errorcode (*IffFuncPtr)(...);

/* "standard" equates */
#define IFF_ERROR	    (-1)
#define OK	    0

#define	WriteByteIFF(b,fp) fputc(b,fp)

/* function prototypes */
errorcode WriteBytesIFF(void *ptr, size_t len, FILE *fp);
errorcode WriteWordIFF(unsigned int value, FILE *fp);
errorcode WriteLongIFF(unsigned long value, FILE *fp);

errorcode WriteFileIFF(FILE *fp, iffTag type, IffFuncPtr writer);
errorcode CreateChunk(FILE *,iffTag,IffFuncPtr,long cbSize=-1L);



















#if 0
class iffWrite
{
public:
//	iffWrite(iffTag fName) {currentFormName = fName; length = 0;};
//	~iffWrite() {};

	errorcode write( iffTag type );
//	errorcode CreateChunk( iffTag tag, fnptr, ulong size=-1UL );

private:
	ostream *streamPtr;
	iffTag currentFormName;
	unsigned long length;



};

inline ulong WriteIFFLong( ostream& output, ulong ul )
	{
	output.put( *(((ubyte*)&ul) + 3) );
	output.put( *(((ubyte*)&ul) + 2) );
	output.put( *(((ubyte*)&ul) + 1) );
	output.put( *(((ubyte*)&ul) + 0) );
	return( ul );
	}

inline uword WriteIFFWord( ostream& output, uword uw )
	{
	output.put( *(((ubyte*)&uw) + 1) );
	output.put( *(((ubyte*)&uw) + 0) );
	return( uw );
	}

inline iffTag WriteIFFTag( ostream& output, iffTag tag )
	{
	return( (iffTag)WriteIFFLong( output, (ulong)tag ) );
	}

inline ulong WriteIFFLength( ostream& output, ulong length )
	{
	return( WriteIFFLong( output, length ) );
	}

inline errorcode
	WriteBytesIFF( ostream& output, void *ptr, size_t len )
	{
	output.write( (uchar*)ptr, (int)len );
	return( 0 );

//	return( fwrite(ptr, (size_t)1, len, fp)==len ? OK : IFF_ERROR );
	}



// Make a WriteIFFPad

typedef errorcode (*IffFuncPtr)();

#define IFF_ERROR	    (-1)
#define OK	    0

int	    wfType(ostream& fp);
int	    WriteFileIFF(ostream&, iffTag type, IffFuncPtr writer);

int CreateChunk(ostream&,iffTag,IffFuncPtr,long=-1L);	//,void far * );		//,int );
#endif



//==============================================================================

#endif

//==============================================================================
