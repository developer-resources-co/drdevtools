/*============================================================================*/
/*	 drmon.c: drmon entry code					     */
/*============================================================================*/

#include <io.h>
#include <time.h>
#include <setjmp.h>

#include "moninc.hpp"
#include "about.hpp"
#include "display.hpp"
#include "app.hpp"
#if defined(__GNUC__)
#include <ncurses_io.h>     // drmon_nc_resized() for the main-loop resize check
#endif

//=============================================================================
// globals

FLAG cont,haveMouse;
_object *frontObj;
FLAG (*oldInputRoutine)(_input *,_object *);
char textBuffer[160+_MAX_PATH];
char *xferBuffer;

int monSpeed = 0;
FLAG modeUpdate = boolean::TRUE;
char *progName;

App* drMon;


//==============================================================================
// locals

FLAG timeToChange = 0;

//=============================================================================

#define ERR_NOSLAVE 5

//=============================================================================

unsigned int slaveBase = PORTBASE;
char *slaveBuffer;

//------------------------------------------------------------------------------

void
LoadEnvVariables(void)
{
	char *strPtr;
	int temp;
	strPtr = getenv("DR"PLATFORMNAME"PORT");
	if ( strPtr )
	 {
		if(sscanf(strPtr,"%x",&temp))
			slaveBase = temp;
	 }

	strPtr = getenv("DR"PLATFORMNAME"MEMBUFFER");
	if ( strPtr )
	 {
		if(sscanf(strPtr,"%x",&temp))
			slaveBuffer = (char *)((long)temp << 16);
	 }
}

//=============================================================================

void
DoStartupExec( char* name )
{
	errorcode error;
	sprintf(xferBuffer,PLATFORMNAME" Monitor Initializing: %s\n",name);
 	PrintToCmdWindow(xferBuffer);
	ExecFile(name,&error);
	if (error)
		PrintError(error);
}

//=============================================================================

char*
Init( int argc, char* argv[] )
{
	// Allocate engough memory for a full-size ASCII memory dump
	xferBuffer = (char*)farmalloc( 12000 );

	int ver;
	SetupConfig();
	displayMode = 3;
	LoadEnvVariables();

	if ( !InitSlaveIO( slaveBase, 0xD000 ) )	// returns false if I/O trouble
	  	return("Slave not found");

    ver = SlaveGetVer();
    if(ver != SLAVE_VER_NUMBER)
	 {
#if defined(__GNUC__)
		// Linux port: no dev hardware/target yet. Run the UI disconnected instead
		// of aborting (a real target arrives via the Phase 2 MAME bridge).
		fprintf(stderr,"Warning: no/incompatible slave ROM (version %d) - running disconnected\n",ver);
#else
	  	sprintf(textBuffer,"Incorrect Slave ROM Version %d",ver);
	  	return(textBuffer);
#endif
	 }

	drMon = new App( argc, argv );

	g_curDesktop = MakeDesktop(0);		// Option A: primary terminal = desktop 0
	pObjBase = new _object;

	InitScreen();
	InitDisplay();
	SetupDisplay();
//    InitLayers();
	InitInput();
	InitMacros();
	InitCmdMacros();	/* SLS 30 Jul 1991 */
//    InitObjects();
	InitListBase((_list *)&statTextBase);
	InitListBase((_list *)&cmdTextBase);
	commandWindow = NULL;
	cmdObjPtr = NULL;
    InitManager();
	InitBreakPts();
	InitSymbols();
	HandleSlaveInput();

	// parse input
	char* t;
	progName = argv[0];

	if (argv[1])
	 {
		if(!access(argv[1],0))
			DoStartupExec(argv[1]);
		else
		 {
			strcpy(textBuffer,argv[1]);
			strcat(textBuffer,".SCR");
			DoStartupExec(textBuffer);
		 }
	 }
	else								// if no command file, check for default
	 {
		if(!access(PROGNAME ".scr",0))
			DoStartupExec(PROGNAME ".scr");
		else
		 {
//			filename startupScript( drMon->fn() );
//			startupScript.ext( "scr" );
//			if( !access( startupScript(), 0 ) )
//				DoStartupExec( startupScript() );

			strcpy(textBuffer,progName);
			t = FindNull(textBuffer) - 3;
			strcpy(t,"SCR");

			if( !access( textBuffer, 0 ) )
				DoStartupExec( textBuffer );
		 }
	 }
	return(NULL);
}

//=============================================================================

void
MainLoop()
{
	clock_t oldTime;
	cont = boolean::TRUE;

	oldTime = clock();
	while(cont)									// main loop
	 {
		timeToChange = boolean::FALSE;
		switch(monSpeed)
		 {
			case 0:
				timeToChange = boolean::TRUE;
				break;
			case 1:
				if(oldTime != clock())
				 {
					oldTime = clock();
					timeToChange = boolean::TRUE;
				 }
				break;
			case 2:
				if(oldTime+1 < clock())
				 {
					oldTime = clock();
					timeToChange = boolean::TRUE;
				 }

				break;
			case 3:
				if(oldTime+3 < clock())
				 {
					oldTime = clock();
					timeToChange = boolean::TRUE;
				 }

				break;
		 }

		slaveUpdate = (modeUpdate && timeToChange);
		for(int di = 0; di < g_numDesktops; di++)	// Option A: update+render every terminal
		 {
			switchDesktop(g_desktops[di]);
			pObjBase->Update();
#if defined(__GNUC__)
			if(drmon_nc_resized())		// terminal resized -> re-fill before drawing
				ReSizeViewport();
#endif
			UpdateScreen();
		 }
	 }
	pObjBase->Update();				// give objects a chance to shut down
	BreakClearAll();			// put breakpoints back
	GoodByeSlave();
}

//=============================================================================

errorcode CleanUp( char* szError = NULL );

errorcode
CleanUp( char* szError )
	{
	UnInitSlaveIO();
	if ( pObjBase )
		delete pObjBase;

	delete drMon;
	if ( xferBuffer ) farfree( xferBuffer );

	if ( szError && *szError )
		{
		printf("\n" PROGNAME " %s Error: %s\n", versionString, szError );
		return ERR_NOSLAVE;
		}
	return NOERR;
	}

//============================================================================

jmp_buf toplevel;

//============================================================================

int
main( int argc, char* argv[] )
	{
	char* errorString;

	errorString = Init( argc, argv );

	if ( !errorString )
		{
		setjmp( toplevel );
		MainLoop();
		}

	return CleanUp( errorString );
	}

//=============================================================================
