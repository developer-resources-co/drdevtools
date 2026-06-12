//============================================================================
// about.cpp: about window handler for drmon
//============================================================================
//
// AboutRoutine()
// AboutInput()
// OpenAbout()
//
//============================================================================

#include "moninc.hpp"
#include "help.hpp"

//============================================================================

FLAG aboutOpen = boolean::FALSE;
_object *aboutObjPtr = NULL;

#define ABOUT_XSIZE 51
#define ABOUT_YSIZE 18

// data1 = frame counter
//============================================================================

char versionString[] =
{
	"V"
#include "version.txt"
};
//	"V@#r"

char dfltMessage[] =
{
	" " PROGNAME " V"
#include "version.txt"
"  Copyright 1991\xC4""1994 Developer Resources"
};

//=============================================================================

void
	AboutRoutine( _object *oPtr )
	{
	}


FLAG
AboutInput(_input *in,_object *oPtr)
{
	_window *pWindow;
	FLAG inputUsed;
	unsigned char *chr;
	_gadget *gPtr;

    pWindow = (_window *)oPtr->layer;
    inputUsed = boolean::FALSE;

	switch(in->inputType)
	 {
		case INP_KEY:
			switch(in->fullKey)
			 {
				case CMD_CLOSEWINDOW:
					aboutOpen = boolean::FALSE;
					aboutObjPtr = NULL;
//					CloseWindow(pWindow);
					delete pWindow;
//					DeleteObject(oPtr);
					delete oPtr;
					inputUsed = boolean::TRUE;
					break;
				case CMD_OPENWINDOWHELPFILE:
					OpenRefFile(EXT_HELP,HELPNAME,"About:");
					inputUsed = boolean::TRUE;
					break;
			 }
			break;
		case INP_MOUSE_LEFTBUTTON_DOWN:
			if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 {
				switch(gPtr->gNum)
				 {
					case GGAD_CLOSE:
						aboutOpen = boolean::FALSE;
						aboutObjPtr = NULL;
//						CloseWindow(pWindow);
						delete pWindow;
//						DeleteObject(oPtr);
						delete oPtr;
						inputUsed = boolean::TRUE;
						break;
					case GGAD_RESIZE:
					case GGAD_DRAG:
						inputUsed = boolean::FALSE;
						break;
				 }
			 }
			break;
	 }
    return(inputUsed);
}

//=============================================================================

FLAG
OpenAbout(void)
{
	static char aboutText[] =
		{
	/*
           	1         2         3         4
 	0123456789012345678901234567890123456789012345
	*/
 	PROGNAME " V"
#include "version.txt"
//@#r
"\n"												// d e v e l o p e r
  	__DATE__ "  " __TIME__ "\n"
 	"\n"											// r e s o u r c e s
 	"                                             \n"
 	"By Kevin T. Seghetti\n"
	"   William B. Norris IV\n"
 	"   Scott L. Statton\n"
#ifdef SNES
 	"\n"
#endif
#ifdef GENESIS
	"   Lars Norpchen\n"
#endif
 	"\n"
 	"\n"
 	"                          Developer Resources\n"
 	"                              P.O. Box 423018\n"
 	"                San Francisco, CA  94142-3018\n"
 	"                                 415.495.2334\n"
 	"                                             \n"
		};

	if ( aboutOpen )
		{
		aboutObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
		}

	_object *oPtr;
	_window *pWindow;

    if ( oPtr = AddObject( AboutRoutine ) )
		{
		pWindow = new _window(-1,-1,ABOUT_XSIZE,ABOUT_YSIZE,"About",ATTR_BACKWHITE|ATTR_BLACK);
		if(pWindow)
			{
			aboutOpen = boolean::TRUE;
			aboutObjPtr = oPtr;

			pWindow->data = (void *)oPtr;
			oPtr->inputRoutine = AboutInput;
			oPtr->layer = pWindow;
			oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;

			AddSysGadgets(pWindow);
			DrawGadgets(pWindow);

			PrintLay(pWindow,aboutText,3,2);

			// Print that new spiffy logo with the line!
			char szDeveloper[] = "d e v e l o p e r";
			PrintLay( pWindow, szDeveloper, ABOUT_XSIZE-strlen(szDeveloper)-3, 2 );
			char szResources[] = "r e s o u r c e s";
			PrintLay( pWindow, szResources, ABOUT_XSIZE-strlen(szResources)-3, 4 );

			for ( int y=0; y<3; ++y )
				PrintLayAttr( pWindow, "\xDB", 26+2, 2+y, ATTR_BACKWHITE|ATTR_BLUE );

			ActivateFrontWindow();
			return(boolean::TRUE);                                                   // in case activated from menu
		 }
		DeleteObject(oPtr);
	 }
	return(boolean::FALSE);
}

//=============================================================================
