
#ifndef GFXCOMM_HPP
#define GFXCOMM_HPP

#include <fstream.h>

void PutCommand( int command );
void PutByte( unsigned b );
void PutWord( unsigned short i );
void PutLong( unsigned long l );
void SendFile( ifstream& input );

enum
	{
	CmdNoOp = 0,
	CmdGetVersion,
	CmdColorCycle,
	CmdDownloadCode,
	CmdExecuteCode,
	CmdGetMap,
	CmdGetTiles,
	CmdGetPalette,
	CmdSetHPos,
	CmdSetVPos,
	CmdUpdateDisplay
	};

#endif
