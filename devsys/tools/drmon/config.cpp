/*============================================================================*/
/*       config.c: drmon configuration code */
/*============================================================================*/

//== Turbo C++ (IBM PC C libraries, but not standard C) ========================

#include        <io.h>

//== Application ===============================================================

#include        "moninc.hpp"
#include        "display.hpp"

//== Adept Creations Library ===================================================

#include        <pclib/profile.hpp>

/*============================================================================*/

uint windowTitleMoveAttr;
uint windowTitleAttr,windowTitleHighlightAttr,windowBorderAttr;
uint menuAttr,menuHighlightAttr,menuBarAttr,menuBarHighlightAttr;
uint menuChar;
uint backgroundAttr,backgroundChar;
uint backgroundAttr2,backgroundChar2;
int  xShadow, yShadow;							// offset of shadow layer (0==none)
uint mouseAttr,mouseChar;
uint listAttr,listChar;
uint fileReqAttr,fileReqChar;
uint breakAttr,breakChar;
uint symbolAttr,symbolChar;
uint messageBarAttr;

uint regAttr,regChar;
uint statusAttr,statusChar;
uint dumpAttr,dumpChar;
uint dumpPCAttr,dumpBreakAttr,dumpCursorAttr;
uint sourceAttr,sourceChar;
uint sourcePCAttr,sourceCursorAttr;
uint textViewAttr,textViewChar;
uint strAttr,strChar;
uint exprAttr,exprChar;
uint projinfoAttr,projinfoChar;
uint asciiAttr,asciiChar;                                               // ascii chart
uint procAttr,procChar;                                         // processor chart
uint commandAttr,commandChar;
uint watchAttr,watchChar;

uint gadgPropAttr,gadgPropHighlightAttr;

//============================================================================

char *
configNames[] =
{
	NULL,
	"DISPMODE",
	"TWOSCREEN",
	"WRITEPROTECT",
	"VIRTUALXSIZE",
	"VIRTUALYSIZE",
	"MESSAGEBARATTR",
	"WATCHATTR",
	"WINDOWTITLEATTR",
	"WINDOWTITLEHIGHLIGHTATTR",
	"WINDOWBORDERATTR",
	"WINDOWTITLEMOVEATTR",
	"MEMORYATTR",
	"MEMORYPCATTR",
	"MEMORYBREAKATTR",
	"MEMORYBARATTR",
	"MENUATTR",
	"MENUHIGHLIGHTATTR",
	"MENUBARATTR",
	"MENUBARHIGHLIGHTATTR",
	"MOUSEATTR",
	"MOUSECHAR",
	"BREAKATTR",
	"SYMBOLATTR",
	"REGATTR",
	"STATUSATTR",
	"TEXTVIEWATTR",
	"EXPRATTR",
	"COMMANDATTR",
	"SOURCEATTR",
	"SOURCEPCATTR",
	"SOURCEBARATTR",
	"PROJINFOATTR",
	"ASCIIATTR",
	"PROCATTR",
	"BACKGROUNDATTR",
	"BACKGROUND2ATTR",
	"BACKGROUNDCHAR",
	"BACKGROUND2CHAR",
	"RUNMODE",
	"INSERT",
	"TRACEMODE",
	"SHADOWXOFFSET",
	"SHADOWYOFFSET",
	NULL
};

//============================================================================

unsigned int *
configPtrs[] =
{
	NULL,
	&displayMode,
	&twoScreen,
	&writeProtect,
	&virtualXSize,
	&virtualYSize,
	(uint*)&messageBarAttr,
	(uint*)&watchAttr,
	(uint*)&windowTitleAttr,
	(uint*)&windowTitleHighlightAttr,
	(uint*)&windowBorderAttr,
	(uint*)&windowTitleMoveAttr,
	(uint*)&dumpAttr,
	(uint*)&dumpPCAttr,
	(uint*)&dumpBreakAttr,
	(uint*)&dumpCursorAttr,
	(uint*)&menuAttr,
	(uint*)&menuHighlightAttr,
	(uint*)&menuBarAttr,
	(uint*)&menuBarHighlightAttr,
	(uint*)&mouseAttr,
	(uint*)&mouseChar,
	(uint*)&breakAttr,
	(uint*)&symbolAttr,
	(uint*)&regAttr,
	(uint*)&statusAttr,
	(uint*)&textViewAttr,
	(uint*)&exprAttr,
	(uint*)&commandAttr,
	(uint*)&sourceAttr,
	(uint*)&sourcePCAttr,
	(uint*)&sourceCursorAttr,
	(uint*)&projinfoAttr,
	(uint*)&asciiAttr,
	(uint*)&procAttr,
	(uint*)&backgroundAttr,
	(uint*)&backgroundAttr2,
	(uint*)&backgroundChar,
	(uint*)&backgroundChar2,
	(uint*)&runMode,
	(uint*)&insertDefault,
	&stepMode,
	(uint*)&xShadow,
	(uint*)&yShadow,
	NULL
};

//============================================================================

int
FindConfigName(char *s)
{
	int i = 1;
	UpCaseString(s);
	while(configNames[i])
	 {
		if(!strcmp(configNames[i],s))
			return(i);
		i++;
	 }
	return(NULL);
}

//============================================================================

int
CountConfigParms(void)
{
	int i = 1;
	while(configNames[i+1])
		i++;
	return(i);
}

//============================================================================

ULONG
GetParamAsSymbol(char *string)
{
    int i;
	UpCaseString(string);

	i = FindConfigName(string);
	if(i)
		return(*configPtrs[i]);
	else
	return(OUT_OF_BAND);
}

//==============================================================================
// determine if color or monochrome card is primary

FLAG
IsCGA(void)
{
#ifdef __MSDOS__
	asm {
		mov     ah,0Fh                                          // get video mode
		INT     10H
		}
	return( _AL != 7 );
#else
	return( boolean::TRUE );
#endif

#if 0
	FLAG cga;
	unsigned char far *biosData;

	if(*((char far *)0x00400087) == 0)
	 {
		// no ega, lets check for color card(cga)

		if((*((char far *)0x00400010) &  0x30) == 48)
			cga = boolean::FALSE;
		else
			cga = boolean::TRUE;
	 }
	else
		cga = boolean::TRUE;                                                    // actually ,ega or above
	return(cga);
#endif
}

//============================================================================

void
SetupConfig(void)
{
	gadgPropAttr = ATTR_NORMAL;
	gadgPropHighlightAttr = ATTR_RED;

	mouseAttr = ATTR_BACKRED;
	mouseChar = 15;

	writeProtect = boolean::TRUE;
	twoScreen = boolean::FALSE;
	primaryDispCGA = IsCGA();

// this set for cga/ega/vga primary
	if(primaryDispCGA)
	 {
		backgroundAttr = ATTR_BLUE;
		backgroundChar = '°';
		backgroundAttr2 = ATTR_NORMAL;
		backgroundChar2 = ' ';
		menuBarHighlightAttr = ATTR_INVERSE;
		menuBarAttr = ATTR_BACKBLUE+ATTR_NORMAL;
		messageBarAttr = ATTR_BACKBLUE+ATTR_NORMAL;
		exprAttr = ATTR_BACKBLUE+ATTR_NORMAL;
		exprChar = ' ';
		fileReqAttr = ATTR_BACKBLUE+ATTR_NORMAL;
		fileReqChar = ' ';
		windowTitleAttr = ATTR_NORMAL;
		windowTitleHighlightAttr = ATTR_INVERSE;
		windowTitleMoveAttr = ATTR_BLUE;
		strAttr = ATTR_BACKBLUE+ATTR_NORMAL;
		strChar = ' ';
		dumpPCAttr = ATTR_BACKLTCYAN;
		dumpCursorAttr = ATTR_INVERSE;
		dumpBreakAttr = ATTR_BACKLTRED;
	 }
	else
	 {
// this set for monochrome primary
		backgroundAttr = ATTR_NORMAL;
		backgroundChar = ' ';
		backgroundAttr2 = ATTR_BLUE;
		backgroundChar2 = '°';
		menuBarAttr = ATTR_INVERSE;
		menuBarHighlightAttr = 1;                                               // normal underlined

		messageBarAttr = ATTR_INVERSE;
		exprAttr = ATTR_INVERSE;
		exprChar = '±';
		fileReqAttr = ATTR_INVERSE;
		fileReqChar = '±';
		windowTitleAttr = ATTR_NORMAL;
		windowTitleHighlightAttr = ATTR_HIGHLIGHT;
		windowTitleMoveAttr = 1;
		strAttr = ATTR_INVERSE;
		strChar = '±';
		dumpPCAttr = ATTR_HIGHLIGHT;
		dumpCursorAttr = ATTR_INVERSE;
		dumpBreakAttr = 1;
	 }

	windowBorderAttr = ATTR_NORMAL;
	menuAttr = ATTR_NORMAL;
	menuChar = ' ';
	menuHighlightAttr = ATTR_INVERSE;

	listAttr = ATTR_BACKBLACK+ATTR_NORMAL;
	listChar = ' ';
	commandAttr = ATTR_BACKBLACK+ATTR_NORMAL;
	commandChar = ' ';
	watchAttr = ATTR_BACKBLACK+ATTR_NORMAL;
	watchChar = ' ';

	projinfoAttr = ATTR_BACKBLUE+ATTR_NORMAL;
	projinfoChar = ' ';
	dumpAttr = ATTR_NORMAL;
	dumpChar = ' ';
	textViewAttr = ATTR_BACKBLUE+ATTR_NORMAL;
	textViewChar = ' ';
	asciiAttr = ATTR_NORMAL;
	procAttr = ATTR_BACKBLUE+ATTR_NORMAL;
	sourceAttr = ATTR_BACKBLUE+ATTR_NORMAL;
	sourcePCAttr = ATTR_BACKCYAN;
	sourceCursorAttr = ATTR_INVERSE;
	sourceChar = ' ';
	regAttr = ATTR_NORMAL;
	regChar = ' ';
	breakAttr = ATTR_NORMAL;
	breakChar = ' ';
	breakAttr = ATTR_NORMAL;
	breakChar = ' ';
	statusAttr = ATTR_NORMAL;
	statusChar = ' ';
	insertDefault = 0;

	xShadow = 2;
	yShadow = 1;

	runMode = RM_NOUPDATES;

#ifdef GENESIS
	runMode = RM_WITHUPDATES;
#endif
}

//============================================================================

void
SetConfigParm(uint which, uint value)
{
	switch(which)
	 {
		case PARM_DISPMODE:                             // if it is a set video mode command
			displayMode = value;
			SetupScreen();
			InitDisplay();
			SetupDisplay();
			GetMenuBarLayer();
			InitMessageBar();
			break;
		case PARM_TWOSCREEN:                            // if it changes # of screens, re-evaulate display width
			*configPtrs[which] = value;
			InitDisplay();
			SetupDisplay();
			break;
		case PARM_WRITEPROTECT:
			if(value)
				SlaveSetWriteProtect();
			else
				SlaveClearWriteProtect();
			break;
		case PARM_VIRTUALXSIZE:
			virtualXSize = value;
			InitDisplay();
			SetupDisplay();
			break;
		case PARM_VIRTUALYSIZE:
			virtualYSize = value;
			InitDisplay();
			SetupDisplay();
			break;
		default:
			*configPtrs[which] = value;
			break;
	 }
}

//==============================================================================

errorcode
SetConfigParm(char *name, uint value)
{
	errorcode error = NOERR;
	int which = FindConfigName(name);
	if(!which)
		return(ERROR_PARAMNOTFOUND);

	SetConfigParm(which,value);
	return(error);
}

//============================================================================

void
LoadConfig(void)
{
	char * t;
	int parmCount = CountConfigParms();

	if(!access(PROGNAME ".ini",0))
		strcpy(textBuffer,PROGNAME ".ini");
	else
	 {
		strcpy(textBuffer,progName);
		t = FindNull(textBuffer) - 3;
		strcpy(t,"ini");
	 }

	if(!access(textBuffer,0))
	 {
		for(int i=0;i<parmCount;i++)
			SetConfigParm(i+1,GetPrivateProfileInt(PROGNAME,configNames[i+1],0,textBuffer));

	 }
}

//============================================================================

void
SaveConfig(void)
{
	int parmCount = CountConfigParms();

	strcpy(textBuffer,PROGNAME ".ini");

	for(int i=0;i<parmCount;i++)
		WritePrivateProfileInt(PROGNAME,configNames[i+1],*configPtrs[i+1],textBuffer);
}

//============================================================================
