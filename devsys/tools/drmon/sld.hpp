//==============================================================================
// sld.hpp: source level debugging headers
//==============================================================================

#include <iostream.h>
#include <fstream.h>

#include <stdlib.h>

//==============================================================================
// include only once insurance

#ifndef DRMON_sld_H
#define DRMON_sld_H

#include <vector>
#include "symfmt.hpp"   // SymLine + ca65/WLA parsers (modern source formats)

//==============================================================================

class _sld
	{
public:
	_sld() {}
	_sld( istream* newStream ) {}
	_sld( _sld* copy ) {}
	virtual ~_sld() {}
	virtual uword SourceLine( void ) = 0;				// returns line # to display as current
	virtual const char* SourceFileName( void ) = 0;		// returns file name to display as curent
	virtual void MarkInvalid( void ) = 0;				// force internal re-sync to occur
	virtual errorcode SourceLoad(char *fileName) = 0;	// load sld info from given filename
	virtual void ReSyncSource(ULONG pc) = 0;			// set sld to new address

														// converts filename/line # into address(for set break, etc)
	virtual ULONG SourceToAddress( char *fileName,UWORD line) = 0;
	virtual boolean ExactMatch(void) = 0;
	};

//============================================================================
// tableSld — source-line backend for the modern homebrew formats (ca65 .dbg,
// WLA-DX .sym). Unlike drSld/zardozSld (which seek a binary file on demand)
// these text formats are parsed once (via symfmt) into an address-sorted line
// table. Always available, independent of the DEBUGDR/DEBUGZARDOZ build flag.

class tableSld : public _sld
	{
public:
	tableSld() { sourceLine = 0; fileName[0] = 0; MarkInvalid(); }
	virtual ~tableSld() {}

	uword SourceLine(void)           { return( sourceLine ); }
	const char* SourceFileName(void) { return( fileName ); }
	void MarkInvalid(void)           { exactMatch = boolean::FALSE; }

	errorcode SourceLoad(char *fileName);
	void ReSyncSource(ULONG pc);
	ULONG SourceToAddress( char *fileName, UWORD line);
	boolean ExactMatch(void)         { return( exactMatch ); }

private:
	std::vector<SymLine> lines;       // sorted by addr ascending
	char fileName[_MAX_PATH];         // current source file (for ReSyncSource)
	uword sourceLine;                 // current source line
	boolean exactMatch;               // pc landed exactly on a line boundary
	};

//============================================================================

#ifdef DEBUGDR

/*	SLD file format:
	$100 bytes of header, starting with sourceName[]

	----

	control byte:

	01	= new filename, followed by 0xffff terminated filename
	02	= sld entry
		24 bits of address(in 68000 order)
		16 bits of file line offset(in Intel order)
*/

//============================================================================

class drSld : _sld
{
public:
	enum { SLDHDRSIZE = 0x100 };
	drSld()
	 {
		MarkInvalid();
		streamPtr = NULL;
		fileName[0] = NULL;
		sldFileName[0] = NULL;
	 }

	drSld(istream *newStream)
	 {
		MarkInvalid();
		streamPtr = newStream;
		fileName[0] = NULL;
		sldFileName[0] = NULL;
	 }


	drSld(drSld *cop)
	 {
		MarkInvalid();
		strcpy(fileName,cop->fileName);
		SourceLoad(cop->sldFileName);
	 }

	~drSld()
	 {
		if(streamPtr)
			delete streamPtr;
	 }

	uword SourceLine(void) { return(sourceLine); }
	const char *SourceFileName(void) { return(fileName); }
	void MarkInvalid(void) { sourceAddr = 0; badPC = 0xfffffffe; }

	errorcode SourceLoad(char *fileName);
	void ReSyncSource(ULONG pc);
	ULONG SourceToAddress( char *fileName,UWORD line);
	boolean ExactMatch(void) { return(boolean::TRUE); }

private:
	// unused
	int BackUpSLDLine(void);
	errorcode ParseBackSLDLine(void);
	ulong SourceAddress(ulong newSAddr) { sourceAddr = newSAddr; return(sourceAddr); }
	// internal
	errorcode SldSeek(size_t lOffset, int nSeekMode );
	errorcode ParseSLDLine(void);
	ulong SourceAddress(void) { return(sourceAddr); }

	// variables
	unsigned long badPC;
	char fileName[_MAX_PATH];				// desired file

	istream *streamPtr;
	char sldFileName[_MAX_PATH];			// filename of sld file
	uword sourceLine;
	unsigned long sourceAddr;
};

#endif

//==============================================================================

//==============================================================================

#ifdef DEBUGZARDOZ

class zardozSld : _sld
{
public:
	zardozSld()
	 {
		MarkInvalid();
		streamPtr = NULL;
		fileName[0] = NULL;
		sldFileName[0] = NULL;
	 }

	zardozSld(istream *newStream)
	 {
		MarkInvalid();
		streamPtr = newStream;
		fileName[0] = NULL;
		sldFileName[0] = NULL;
	 }

	zardozSld(zardozSld *cop)
	 {
		MarkInvalid();
		strcpy(fileName,cop->fileName);
		SourceLoad(cop->sldFileName);
	 }

	~zardozSld()
	 {
		if(streamPtr)
			delete streamPtr;
	 }

	uword SourceLine(void) { return(sourceLine); }
	const char *SourceFileName(void) { return(fileName); }
	void MarkInvalid(void) { sourceAddr = 0; badPC = 0xfffffffe; }

	errorcode SourceLoad(char *fileName);
	void ReSyncSource(ULONG pc);
	ULONG SourceToAddress( char *fileName,UWORD line);
	boolean ExactMatch(void) { return(exactMatch); }

private:
	struct zardozHeader
		{
		uword magic_id;
		};

	// internal
	ulong SourceAddress(void) { return(sourceAddr); }
	ulong SourceAddress(ulong newSAddr) { sourceAddr = newSAddr; return(sourceAddr); }

	// variables
	unsigned long badPC;
	char fileName[_MAX_PATH];				// desired file
	boolean exactMatch;
	istream *streamPtr;
	char sldFileName[_MAX_PATH];			// filename of sld file
	uword sourceLine;
	unsigned long sourceAddr;
};

#endif

//==============================================================================

extern _sld *stSld;

//==============================================================================

#endif

//==============================================================================
