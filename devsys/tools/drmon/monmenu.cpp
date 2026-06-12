//=============================================================================
//  monmenu.c: menu bar for drmon
//=============================================================================

#include        "moninc.hpp"
#include        "source.hpp"
#include        <iostream.h>
#include        <fstream.h>
#include        "help.hpp"
#include        "linux/spawn_window.hpp"

//=============================================================================

#define FILENAME_SIZE 9
_object *menuBarObject;
char logFileName[14] = { 0 };

//============================================================================

#if defined( DEBUGDR ) || defined( DEBUGZARDOZ )

void
LoadFile(void *data,char *path,char *fileName)
{
	char*s;
	strcpy(textBuffer,fileName);
	UnPadString(textBuffer);
	LoadBinaryFile(0L,textBuffer);

#if defined( DEBUGDR )
	s = FindChar(textBuffer,'.');
	strcpy(s,".sym");
	LoadSymbol(textBuffer);                         // try to load symbols
#endif
}

//============================================================================

FLAG
FileMenuLoad(_menuItem *iPtr,_object *oPtr,int choice)
{
	int i;
	strcpy(patternString,"*.bin       ");
	DoFileReq("Load File",10,1,LoadFile,0);
	return(boolean::TRUE);
}

#endif

//============================================================================

//==============================================================================

void
SetLogFile(void *data,char *path,char *fileName)
{
	char*s;
	strcpy(logFileName,fileName);
}

//============================================================================

FLAG
SetLogFile(_menuItem *iPtr,_object *oPtr,int choice)
{
	int i;
	strcpy(patternString,"*.log       ");
	DoFileReq("Set Log File",10,1,SetLogFile,0);
	return(boolean::TRUE);
}

//============================================================================

void
MenuExecuteScript(void *data,char *path,char *fileName)
{
	errorcode error;
	ExecFile(fileName,&error);
	if(error)
		PrintError(error);
}

//============================================================================

FLAG
FileMenuExecute(_menuItem *iPtr,_object *oPtr,int choice)
{
	int i;
	DoFileReq("Execute Script",10,1,MenuExecuteScript,0);
	return(boolean::TRUE);
}

//============================================================================

FLAG
FileMenuQuit(_menuItem *iPtr,_object *oPtr,int choice)
{
	cont = boolean::FALSE;
	return(boolean::TRUE);
}

//============================================================================

FLAG
LoadINI(_menuItem *iPtr,_object *oPtr,int choice)
{
	LoadConfig();
	return(boolean::TRUE);
}

FLAG
SaveINI(_menuItem *iPtr,_object *oPtr,int choice)
{
	SaveConfig();
	return(boolean::TRUE);
}

//============================================================================


menuItems fileMenu[] =
{
#if defined( DEBUGDR ) || defined( DEBUGZARDOZ )
	{"&Load...                 ",FileMenuLoad,       0},
#endif
	{"Execute &Script...       ",FileMenuExecute,    0},
	{"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4" ,NULL,               0},
	{"E&xit               Alt-X",FileMenuQuit,0},
	{0,0,0}
};

//============================================================================

FLAG
MenuNewWindow(_menuItem *iPtr, _object *oPtr, int choice)
{
	if (SpawnNewWindow() < 0)
		PrintMessageBar("New Window: no terminal found - set $DRMON_TERMINAL (e.g. \"xterm -e\")");
	return(boolean::TRUE);
}

//============================================================================

menuItems windowMenu[] =
{
	{"&New Window             ",(unsigned int (*)(_menuItem *,_object *,int))MenuNewWindow,   0},
	{"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4",NULL, 0},
	{"&Memory       Alt-M",(unsigned int (*)(_menuItem *,_object *,int))OpenMemory,      0},
	{"&Register     Alt-R",(unsigned int (*)(_menuItem *,_object *,int))OpenReg,         0},
#ifdef SPC700
	{"SPC Register  Alt-N",(unsigned int (*)(_menuItem *,_object *,int))OpenSpcReg,      0},
#endif
	{"&BreakPoints  Alt-B",(unsigned int (*)(_menuItem *,_object *,int))OpenBreak,       0},
	{"Source       Alt-L",(unsigned int (*)(_menuItem *,_object *,int))OpenSource,      0},
	{"C&onsole      Alt-O",(unsigned int (*)(_menuItem *,_object *,int))OpenConsole,     0},
	{"&Symbol       Alt-S",(unsigned int (*)(_menuItem *,_object *,int))OpenSymbol,      0},
	{"&Expression   Alt-E",(unsigned int (*)(_menuItem *,_object *,int))OpenExpWindow,   0},
	{"&Command      Alt-K",(unsigned int (*)(_menuItem *,_object *,int))OpenCmdWindow,   0},
	{"&Watch        Alt-W",(unsigned int (*)(_menuItem *,_object *,int))OpenWatch,       0},
	{"&TextView     Alt-T",(unsigned int (*)(_menuItem *,_object *,int))OpenTextView,    0},
	{"&Help         Alt-H",(unsigned int (*)(_menuItem *,_object *,int))OpenHelpFile,    0},
	{"Project &Info Alt-I",(unsigned int (*)(_menuItem *,_object *,int))OpenProjectInfo, 0},
	{"ASCII chart  Alt-Y",(unsigned int (*)(_menuItem *,_object *,int))OpenASCIIChart   ,0},
	{0,0,0}
};

//============================================================================

FLAG
SetSpeed(_menuItem *iPtr,_object *oPtr,int choice)
{
	monSpeed = choice;
	return(boolean::TRUE);
}

//============================================================================

menuItems speedMenu[] =
{
	{"&Full Speed",(unsigned int (*)(_menuItem *,_object *,int))SetSpeed,0},
	{"&18 FPS    ",(unsigned int (*)(_menuItem *,_object *,int))SetSpeed,0},
	{"&9 FPS     ",(unsigned int (*)(_menuItem *,_object *,int))SetSpeed,0},
	{"&4 FPS     ",(unsigned int (*)(_menuItem *,_object *,int))SetSpeed,0},
	{0,0,0}
};

//============================================================================

menuItems macroMenu[] =
{
	{"&Create Macro...    ",	(unsigned int (*)(_menuItem *,_object *,int))CreateMacro,0},
	{"&Load Macro File... ",	(unsigned int (*)(_menuItem *,_object *,int))LoadGUIMacros,0},
	{"&Save Macro File... ",	(unsigned int (*)(_menuItem *,_object *,int))SaveGUIMacros,0},
	{"&Delete All Macros  ",	(unsigned int (*)(_menuItem *,_object *,int))NukeMacros,0},
	{0,0,0}
};

//============================================================================

FLAG
RunMenuRun(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSlaveRunWithUpdates();
	return(boolean::TRUE);
}

FLAG
RunMenuStop(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveStop();
	return(boolean::TRUE);
}

FLAG
RunMenuRunNoUpdate(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveRunNoUpdate();
	return(boolean::TRUE);
}

FLAG
RunMenuStep(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveStep();
	return(boolean::TRUE);
}

FLAG
RunMenuStepOver(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveStepOver();
	return(boolean::TRUE);
}

FLAG
RunMenuRestart(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveRestart();
	return(boolean::TRUE);
}

FLAG
RunMenuReset(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveReset();
	return(boolean::TRUE);
}


FLAG
RunMenuProtOn(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveSetWriteProtect();
	return(boolean::TRUE);
}

FLAG
RunMenuProtOff(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveClearWriteProtect();
	return(boolean::TRUE);
}

menuItems runProtMenu[] =
{
	{"On ",RunMenuProtOn,       0},
	{"Off",RunMenuProtOff,      0},
	{0,0,0}
};

FLAG
DoRunProtMenu(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSubMenu(iPtr,oPtr,runProtMenu,"Write Protect");
	return(boolean::FALSE);
}

//============================================================================

FLAG
RunMenuBrkOnWriteOn(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveSetBRKOnWrite();
	return(boolean::TRUE);
}

FLAG
RunMenuBrkOnWriteOff(_menuItem *iPtr,_object *oPtr,int choice)
{
	SlaveClearBRKOnWrite();
	return(boolean::TRUE);
}

FLAG
DoRunBrkMenu(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoSubMenu(iPtr,oPtr,runProtMenu,"Break on ROM Write");
	return(boolean::FALSE);
}

//============================================================================

menuItems runMenu[] =
{
	{"&Run               F2",RunMenuRun,       0},
	{"S&top              F3",RunMenuStop,      0},
	{"Run &no Update     F4",RunMenuRunNoUpdate,0},
	{"&Step              F7",RunMenuStep,      0},
	{"Step &Over         F8",RunMenuStepOver,  0},
	{"Reset Sla&ve         ",RunMenuRestart,   0},
	{"Reset To &Monitor    ",RunMenuReset,     0},
	{"&Write Protect     >>",DoRunProtMenu,    0},
	{"&Break on ROM Write>>",DoRunBrkMenu,    0},
	{0,0,0}
};

//==============================================================================

void
SourceSetPath(char *string,void *dataPtr)
{
	UnPadString(string);
	if(sourcePath)
		free(sourcePath);
	sourcePath = DupString(string);
}

//==============================================================================

FLAG
DoSourcePath(_menuItem *iPtr,_object *oPtr,int choice)
{
	int i;

	if(sourcePath)
		strcpy(expString,sourcePath);
	else
		*expString = 0;
	PadString(expString,EXPRLEN);
	GetString(SourceSetPath,oPtr,"Enter New Source Path",NULL);
	return(boolean::TRUE);
}

//============================================================================

menuItems settingsMenu[] =
{
	{"Set Source &Path...      ",DoSourcePath,0},
	{"&Load INI Settings       ",LoadINI,     0},
	{"&Save INI Settings       ",SaveINI,     0},
	{"Set Lo&g File...         ",SetLogFile,  0},
	{0,0,0}
};

//============================================================================

FLAG
	HelpMenuIndex(_menuItem *iPtr,_object *oPtr,int choice)
	{
//      OpenRefFile( "hlp", "Commands", "Commands:" );
	return( boolean::TRUE );
	}

FLAG
	HelpMenuKeyboard(_menuItem *iPtr,_object *oPtr,int choice)
	{
	OpenRefFile( "hlp", "Keyboard", "Quick Reference Chart:" );
	return( boolean::TRUE );
	}


FLAG
	HelpMenuCommands(_menuItem *iPtr,_object *oPtr,int choice)
	{
	OpenRefFile( "hlp", "Commands", "Commands:" );
	return( boolean::TRUE );
	}


FLAG
	HelpMenuUsingHelp(_menuItem *iPtr,_object *oPtr,int choice)
	{
	OpenRefFile( "hlp", "Using Help", "Using Help:" );
	return( boolean::TRUE );
	}




menuItems helpMenu[] =
{
	{"&Index         ",HelpMenuIndex,   0},
	{"&Keyboard      ",HelpMenuKeyboard,0},
	{"&Commands      ",HelpMenuCommands,0},
	{"&Using Help    ",HelpMenuUsingHelp,0},
	{"\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4",NULL,            0},
	{"&About         ",(unsigned int (*)(_menuItem *,_object *,int))OpenAbout,0},
	{0,0,0}
};

//============================================================================
// bring menu bar to front

void
ActivateMenuBar(void)
{
	menuBarObject->layer->ToFront();
	ActivateFrontWindow();
}

//============================================================================

_menu *
SetupMenuBar(void)
{
	_menu *mPtr,*m2Ptr;
	_menuItem *iPtr;
	iPtr = CreateItems(fileMenu);
	mPtr = new _menu( iPtr, "&File" );
	mPtr->xPos = 2;

	iPtr = CreateItems(runMenu);
	m2Ptr = new _menu( iPtr, "&Control" );
	AddMenu(mPtr,m2Ptr);

	iPtr = CreateItems(windowMenu);
	m2Ptr = new _menu( iPtr, "&Windows");
	AddMenu(mPtr,m2Ptr);

	iPtr = CreateItems (macroMenu);
	m2Ptr = new _menu(iPtr, "&Macros");
	AddMenu(mPtr,m2Ptr);

	iPtr = CreateItems(speedMenu);
	m2Ptr = new _menu( iPtr, "&Rate");
	AddMenu(mPtr,m2Ptr);

	iPtr = CreateItems(settingsMenu);
	m2Ptr = new _menu( iPtr, "&Settings" );
	AddMenu( mPtr, m2Ptr );

	iPtr = CreateItems(helpMenu);
	m2Ptr = new _menu( iPtr, "&Help" );
	AddMenu( mPtr, m2Ptr );

	return(mPtr);
}

//============================================================================

void
GetMenuBarLayer(void)
{
	_layer *lPtr;

	delete menuBarObject->layer;
	lPtr = menuBarObject->layer = new _layer(0,0,screenWidth,1,menuBarAttr,fillChar);

	menuBarObject->layer = lPtr;
	lPtr->data = (void *)menuBarObject;
	lPtr->deactivateRoutine = MenuBarDeactivate;
	lPtr->activateRoutine = MenuBarActivate;
	lPtr->title = NULL;
	RenderMenuTitles(lPtr,(_menu *)menuBarObject->dataPtr,0);
}

//============================================================================

void
InitMenuBar(void)
{
    _object *oPtr;
    _layer *lPtr;
    oPtr = AddObject(MenuRoutine);
	oPtr->data1 = 0;
	oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
    oPtr->inputRoutine = MenuBarKeys;
    frontObj = oPtr;                    // front object must always point to a valid object
	menuBarObject = oPtr;
	oPtr->dataPtr = (char*)SetupMenuBar();
	GetMenuBarLayer();
}

//============================================================================

