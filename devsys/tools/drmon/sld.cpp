//==============================================================================
// sld.cpp: source level debugging code
//==============================================================================

#include "moninc.hpp"
#include "sld.hpp"
#include "coff.hpp"

//==============================================================================

_sld *stSld = NULL;

//============================================================================

#ifdef DEBUGDR

const char sldHeader[] = "Source level debugging file";   // produced by SPASM/65816";

//==============================================================================
// does a seek to an .SLD file returning an error as per fseek
// also returns an EOF error if seeking before the .SLD data (into the header)

errorcode
	drSld::SldSeek(size_t lOffset, int nSeekMode )
	{
		streamPtr->seekg(lOffset,ios::beg);
		if(streamPtr->rdstate() != ios::goodbit)
			return( -1 );
	return( streamPtr->tellg() < SLDHDRSIZE ? 0 : -1 );
	}

//============================================================================

errorcode
drSld::ParseSLDLine(void)
{
	unsigned char in;
	char *s;
	errorcode error;
	error = NOERR;
	streamPtr->get(in);
	if(streamPtr->rdstate() != ios::goodbit)
		return(ERROR_NOSOURCEINFO);
	switch(in)
	 {
		case 1:
			s = fileName;
			streamPtr->get(in);
			while(in != 0xff && in != 0)
			 {
				*s++ = in;
				streamPtr->get(in);
			 }
			*s = 0;
			streamPtr->get(in);						// skip other 0xff
			break;
		case 2:
			char temp;
			sourceAddr = ((unsigned long)streamPtr->get());
			sourceAddr |= ((unsigned long)streamPtr->get()) << 8;
			sourceAddr |= ((unsigned long)streamPtr->get()) << 16;
			sourceLine = ((unsigned int)streamPtr->get());
			sourceLine |= ((unsigned int)streamPtr->get()) << 8;

			if(sourceLine == 0xffff)
				error = ERROR_BROKENSLDFILE;
			break;
		default:
			return(ERROR_BROKENSLDFILE);
	 }
	return(error);
}

//============================================================================
// returns type of command just backed over (what the file currently points to)
// if -1, then error

int
drSld::BackUpSLDLine(void)
{
	int type;
	unsigned char c,c2;

	if(SldSeek(-2,ios::cur))
		return(-1);

	streamPtr->get(c);
	streamPtr->get(c2);

	if(c == 0xff && c2 == 0xff)
	 {
		type = 1;
		SldSeek(-2,ios::cur);
		while(streamPtr->get() != 1)
			if(SldSeek(-2,ios::cur))					// go back until read begining of filename
				return(-1);
		if(SldSeek(-1,ios::cur))						// back up to command
			return(-1);
	 }
	else
	 {
		type = 2;
		if(SldSeek(-6,ios::cur))
			return(-1);
	 }
	return(type);
}

//============================================================================

errorcode
drSld::ParseBackSLDLine(void)
{
	errorcode error = NOERR;
	int type;
	unsigned long pos,sAddr;
	type = BackUpSLDLine();
	if(type == -1)
		return(ERROR_NOSOURCEINFO);

	if(type == 1)
	 {
		pos = streamPtr->tellg();
		while(BackUpSLDLine() != 1);
		ParseSLDLine();
		error = ParseSLDLine();
		SldSeek(pos,ios::beg);
	 }
	else
	 {
		pos = streamPtr->tellg();
		error = ParseSLDLine();
		SldSeek(pos,ios::beg);
	 }
	return(error);
}

///////////////////////////////////////////////////////////////////////////////
//
// EXTERNAL INTERFACE CALLS
//

errorcode
drSld::SourceLoad(char *fileName)
{
	errorcode error = NOERR;
	_object *oPtr;
	int i;
	char tempBuff[SLDHDRSIZE];

	streamPtr = new ifstream(fileName,ios::in | ios::binary);

	if(!streamPtr)
	        error = ERROR_FILENOTFOUND;
	else
	 {
#if 1
	        for(i=0;i<SLDHDRSIZE;i++)
	                streamPtr->get(tempBuff[i]);
	        tempBuff[strlen(sldHeader)] = 0;
	        if(strcmp(sldHeader,tempBuff))
	                error = ERROR_NOTSLDFILE;
#else
	        strcpy(fileName,sldFileName);
			streamPtr->sgetn( tempBuff, sizeof( sldHeader ) );

			if ( strncmp( tempBuff, sldHeader, sizeof( sldHeader ) ) != 0 )
				{ // Error
				error = ERROR_NOTSLDFILE;
				}
#endif
			else
	         {
	                //PrintMessageBar("Loading SLD File");
	                //ClearMessageBar();
	          }
	 }
	if(error)
	 {
	        if(streamPtr)
	         {
	                delete streamPtr;
	         }
	        streamPtr = NULL;
	 }
	return(error);
}

//============================================================================
// convert current source code line to address, for setting breakpoints, etc.

ULONG
drSld::SourceToAddress(char *fileName, UWORD line)
{
	drSld tempSld;
	errorcode error = NOERR;
	long where;

	where = streamPtr->tellg();
	tempSld.streamPtr = streamPtr;

	tempSld.SldSeek(SLDHDRSIZE,ios::beg);
	while((tempSld.SourceLine() != line || strcmp(fileName,tempSld.fileName)) && !error)
		error = tempSld.ParseSLDLine();
	if(error)
	 {
		SldSeek(where,ios::beg);
		return(0);
	 }
	SldSeek(where,ios::beg);
	return(tempSld.SourceAddress());
}

//==============================================================================

void
drSld::ReSyncSource(ULONG pc)
{
	errorcode error = NOERR;

	if (streamPtr)
	 {
		if(badPC == pc || SourceAddress() == pc)
			return;

		if(SourceAddress() > pc)
		 {
			SldSeek(SLDHDRSIZE,ios::beg);
			while(SourceAddress() > pc && !error)
				error = ParseSLDLine();
		 }
		while((SourceAddress() < pc) && !error)
			error = ParseSLDLine();
		while(SourceAddress() > pc && !error)
			error = ParseBackSLDLine();
		if(error)
			PrintError(error);
		if(error == ERROR_NOSOURCEINFO)
		 {
			SldSeek(SLDHDRSIZE,ios::beg);
		  	badPC = pc;
		 }
	 }
}

//==============================================================================

#endif

//==============================================================================

#ifdef DEBUGCOFF

//==============================================================================
///////////////////////////////////////////////////////////////////////////////
//
// EXTERNAL INTERFACE CALLS
//

errorcode
coffSld::SourceLoad(char *fileName)
{
	// not used by coff, use load from file menu instead
	return(NOERR);
}

//============================================================================
// convert current source code line to address, for setting breakpoints, etc.

ULONG
coffSld::SourceToAddress(char *fileName, UWORD line)
{
	errorcode error = NOERR;

	coffSection *csPtr;
	coffLineNo *lnPtr;

	boolean found = boolean::FALSE;

	if(coffBase)
	 {
			// for now, start at top each time
		csPtr = coffBase->GetSectionPtr();

		while(csPtr && !found)
		 {
			lnPtr = &csPtr->GetLineNo();

			if(lnPtr->FindAddress(fileName,line))
				found = boolean::TRUE;
			else
				csPtr = csPtr->GetNext();
		 }

		if(found)
			return(lnPtr->CurrentPC());
		else
			error = ERROR_NOSOURCEINFO;

		if(error)
			PrintError(error);
	 }
}

//==============================================================================

void
coffSld::ReSyncSource(ULONG pc)
{
	errorcode error = NOERR;

	coffSection *csPtr;
	coffLineNo *lnPtr;

	boolean found = boolean::FALSE;

	if(coffBase)
	 {
		if(badPC == pc || SourceAddress() == pc)
			return;

			// for now, start at top each time
		csPtr = coffBase->GetSectionPtr();

		while(csPtr && !found)
		 {
			lnPtr = &csPtr->GetLineNo();

			if(lnPtr->FindSource(pc))
			 {
				found = boolean::TRUE;
				exactMatch = lnPtr->ExactMatch();
			 }
			else
				csPtr = csPtr->GetNext();
		 }

		if(found)
		 {
			sourceLine = lnPtr->CurrentSourceLine();
			SourceAddress(pc);
		    const char *fnPtr = lnPtr->FileName();
			if(fnPtr)
				strncpy(fileName,fnPtr,_MAX_PATH);
		 }
		else
			error = ERROR_NOSOURCEINFO;


		if(error)
			PrintError(error);
		if(error == ERROR_NOSOURCEINFO)
			badPC = pc;
	 }
}

//==============================================================================

#endif


#if defined( DEBUGZARDOZ )

#include "zardoz.hpp"

errorcode
zardozSld::SourceLoad(char *fileName)
	{
	errorcode error = NOERR;
	_object *oPtr;
	int i;

	streamPtr = new ifstream(fileName,ios::in | ios::binary);

	if(!streamPtr)
	        error = ERROR_FILENOTFOUND;
	else
		{
	   strcpy(fileName,sldFileName);
		// validate
		}

	if ( error )
		{
	   if(streamPtr)
	   	{
	      delete streamPtr;
	      }
		streamPtr = NULL;
		}

	return( error );
	}

//============================================================================
// convert current source code line to address, for setting breakpoints, etc.

ULONG
zardozSld::SourceToAddress(char *fileName, UWORD line)
	{
	return( 0x8000 );
	}

//==============================================================================

void
zardozSld::ReSyncSource( ULONG pc )
	{
	errorcode error = NOERR;

	if ( streamPtr )
		{
		if ( (badPC == pc) || ( SourceAddress() == pc ) )
			return;

	streamPtr->seekg( sizeof( zardozHeader ), ios::beg );		// Already validated

	long addr, size;
	unsigned short snum, len, line, mnum = 0, curName;
	unsigned short nsecs, nmods, nrecs, nfiles, nsyms, type, ps;
//	char buf[ 128 ], *cp;
	char* cp;
	unsigned long val;
	short i;
	LinRec lr;

	int desiredSection = -1;	// section which contains the line number info for pc


	nsecs = get_word( *streamPtr );
	nmods = get_word( *streamPtr );
	PrintToStatWindow( "%d sections - %d modules\n", nsecs, nmods );
	while ( nmods-- )
		{
		// Read name of object module
		len = get_byte( *streamPtr );
		streamPtr->seekg( len, ios::cur );			// Skip past object module name

		nsecs = get_byte( *streamPtr );
		for ( int i=0; i < nsecs; ++i )
			{
			snum = get_byte( *streamPtr );
			addr = get_long( *streamPtr );
			size = get_long( *streamPtr );

			if ( ( addr <= pc ) && ( pc < addr+size ) )
				{ // PC is within *this* section
				desiredSection = snum;
				PrintToStatWindow( "desiredSection = %d", desiredSection );
				}
			}

		nrecs = get_word( *streamPtr );
		PrintToStatWindow( "\t\tnlinrecs - %u", nrecs );
		while ( nrecs-- )
			{
			streamPtr->read( (char*)&lr, sizeof( LinRec ) );
			PrintToStatWindow( "\t\tfile %d line %d @ %lx ps-%02X len-%d\n",
				lr.filNum, lr.linNum, lr.addr, lr,ps, lr.nBytes );

			if ( (desiredSection != -1) && ( lr.addr >= pc ) )
				break;
			}
		// skip past any remaining records
		streamPtr->seekg( (nrecs+1) * sizeof( LinRec ), ios::cur );

		// lr contains what we want (to get to)

		for ( snum=0, nfiles = get_byte( *streamPtr ); nfiles--; ++snum )
			{
			len = get_byte( *streamPtr );
			cp = fileName;
			while ( len-- )
				*cp++ = get_byte( *streamPtr );
			*cp = '\0';
			line = get_word( *streamPtr );

			if ( snum == lr.filNum )
				{
				PrintToStatWindow( "Found filename: %s", fileName );
				}

			PrintToStatWindow( "\t%d: nLines=%d <%s>\n", snum, line, fileName );
			}
		}

		PrintError(error);
		}
	}

//==============================================================================

#endif

//==============================================================================


