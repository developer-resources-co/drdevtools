//=============================================================================
//	message.c: message bar handling
//=============================================================================

#include <stdarg.h>

#include	"moninc.hpp"
#include	"about.hpp"
#include	"display.hpp"

//==============================================================================

#define MSG_STATUS_XPOS	67
#define MSG_STATUS_XSIZE 13

//=============================================================================

char *status[] =
{
	"Stopped      ",
	"Run No Update",
	"Running      ",
	"Slave Dead   ",
#ifdef GENESIS
 	"Address Error",
 	"Illegal Instr",
 	"Div 0 Error  ",
 	"Trace!!      ",
 	"ALine        ",
 	"FLine        ",
 	"NMI!!!       ",
 	"DEBUG!!!     "
#endif

#ifdef SNES
	"BREAK        "
#endif
};

//=============================================================================

_layer *messageLayer = NULL;
char *defaultMessage;
int errorDelay;

//=============================================================================

void
ClearMessageBarSection(int x,int size)
{
	while(size--)
		PrintLay(messageLayer," ",x++,0);
}

//=============================================================================

void
PrintMessageBarSection(int x,int size,char *text)
{
	ClearMessageBarSection(x,size);
	PrintLay(messageLayer,text,x,0);
}

//=============================================================================

void
PrintMessageStatus(int stat)
{
	PrintMessageBarSection(MSG_STATUS_XPOS,MSG_STATUS_XSIZE,status[stat]);
}

//=============================================================================

void
ClearMessageBar(void)
{
	messageLayer->Fill();						//	FillLayer(messageLayer,' ');
	if(defaultMessage)
		PrintMessageBar(defaultMessage);
	UpdateScreen();
}

//=============================================================================

void
MessageBarTick(void)
{
	if(errorDelay)
	 {
		errorDelay--;
		if(!errorDelay)
			ClearMessageBar();
	 }
}

//=============================================================================

void
PrintMessageBar( char *fmt, ... )
{
	va_list pArg;
	char buffer[ 256 ];
	char* text = buffer;

	va_start( pArg, fmt );
	vsprintf( buffer, fmt, pArg );
	va_end( pArg );

	errorDelay = 2;
	messageLayer->Fill();						// FillLayer(messageLayer,' ');
	PrintLay(messageLayer,text,0,0);
	UpdateScreen();
}

//=============================================================================

void
PrintWarning(char *text)				// for now, maybe in red later
{
	PrintMessageBar(text);
	UpdateScreen();
}

//=============================================================================

void
PrintError( errorcode error )
	{
	if ( error != NOERR )
		PrintWarning( SYSERRORS[ error ] );
	}

//=============================================================================

void
InitMessageBar(void)
{
	delete messageLayer;

#if 0
	if(messageLayer)
		DeleteLayer(messageLayer);
    messageLayer = OpenLayer(0,displayHeight-1,screenWidth,1,messageBarAttr,fillChar);
#endif
    messageLayer = new _layer(0,displayHeight-1,screenWidth,1,messageBarAttr,fillChar);
	defaultMessage = dfltMessage;
	ClearMessageBar();
}

//=============================================================================

