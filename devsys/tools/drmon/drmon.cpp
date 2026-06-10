/*============================================================================*/
/*	 drmon.c: drmon entry code					     */
/*============================================================================*/

#include <io.h>
#include <time.h>
#include <setjmp.h>
#ifdef DOSX286
#include <phapi.h>
#endif

#include "moninc.hpp"
#include "about.hpp"
#include "display.hpp"
#include "app.hpp"

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
char far *slaveBuffer;

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
			slaveBuffer = (char far *)((long)temp << 16);
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
		pObjBase->Update();
		UpdateScreen();
	 }
	pObjBase->Update();				// give objects a chance to shut down
	BreakClearAll();			// put breakpoints back
	GoodByeSlave();
}

//=============================================================================

#ifdef DOSX286
PEHANDLER oldgp;
#endif

errorcode CleanUp( char* szError = NULL );

errorcode
CleanUp( char* szError )
	{
	UnInitSlaveIO();
	if ( pObjBase )
		delete pObjBase;

	delete drMon;
	if ( xferBuffer ) farfree( xferBuffer );

#ifdef DOSX286
	DosSetExceptionHandler( 0, oldgp, NULL );
#endif

	if ( szError && *szError )
		{
		printf("\n" PROGNAME " %s Error: %s\n", versionString, szError );
		return ERR_NOSLAVE;
		}
	return NOERR;
	}

//============================================================================

jmp_buf toplevel;

#ifdef DOSX286

void interrupt far gpfault( EXCEP_FRAME e )
	{
	PrintToStatWindow( "GP Fault\n" );
	PrintToStatWindow( "AX=%04x BX=%04x CX=%04x DX=%04x  ES=%04x DS=%04x\n", e.ax, e.bx, e.cx, e.dx, e.es, e.ds );
	PrintToStatWindow( "DI=%04x SI=%04x BP=%04x SP=%04x  ERRORCODE=%04x\n", e.di, e.si, e.bp, 0, e.error_code );
	UpdateScreen();

	longjmp( toplevel, 0 );
	}
#endif

//============================================================================

int
main( int argc, char* argv[] )
	{
	char* errorString;

#ifdef DOSX286
	DosSetExceptionHandler( 0x0D, gpfault, &oldgp );
#endif

	errorString = Init( argc, argv );

	if ( !errorString )
		{
		setjmp( toplevel );
		MainLoop();
		}

	return CleanUp( errorString );
	}

//=============================================================================
