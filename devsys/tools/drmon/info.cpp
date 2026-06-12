//============================================================================
//      info.c:  Project information statistics (?)
//============================================================================

#include "moninc.hpp"
#include "help.hpp"

//============================================================================

char *speedItems[] =
{
	"Full Speed",
	"18 FPS    ",
	"9 FPS     ",
	"4 FPS     ",
	NULL
};

//============================================================================

static FLAG projInfoOpen = boolean::FALSE;
static _object* projInfoObjPtr = NULL;

static char frameTbl[] = "/-\\|";

void
ProjectInfoRoutine(_object *oPtr)
{
	unsigned long size = 0;
	char *sPtr;
	_layer *lPtr = oPtr->layer;

	unsigned int nLine = 2;

	sPtr = PrintString(textBuffer,"Memory Free: ");
	*PrintDecimal32Bits(sPtr,size) = '\0';
	PrintLay(lPtr,textBuffer,3,nLine++);

	size = CountListItems( (_list *)&symbolListBase);
	sPtr = PrintString(textBuffer,"Update Rate: ");
	sPtr = PrintString(sPtr,speedItems[monSpeed]);
	PrintLay(lPtr,textBuffer,3,nLine++);

	size = CountListItems( (_list *)&symbolListBase);
	sPtr = PrintString(textBuffer,"Symbols: ");
	*PrintDecimal32Bits(sPtr,size) = '\0';
	PrintLay(lPtr,textBuffer,3,nLine++);

	sPtr = PrintString(textBuffer,"Frame Rate: ");
	*sPtr++ = frameTbl[(oPtr->data1++)&3];
	*sPtr = 0;
	PrintLay(lPtr,textBuffer,3,nLine++);

}


FLAG
ProjectInfoInput(_input *in,_object *oPtr)
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
					projInfoOpen = boolean::FALSE;
					projInfoObjPtr = NULL;
//					CloseWindow(pWindow);
  					delete pWindow;
//					DeleteObject(oPtr);
					delete oPtr;
					inputUsed = boolean::TRUE;
					break;
				case CMD_OPENWINDOWHELPFILE:
					OpenRefFile(EXT_HELP,HELPNAME,"Project Information");
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
						projInfoOpen = boolean::FALSE;
						projInfoObjPtr = NULL;
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


FLAG
OpenProjectInfo( void )
	{
	_object *oPtr;
	_window *pWindow;

	if ( projInfoOpen )
		{
		projInfoObjPtr->layer->ToFront();
		ActivateFrontWindow();
		return(boolean::TRUE);
		}

	if ( oPtr = AddObject( ProjectInfoRoutine ) )
		{
		pWindow = new _window(0,0,29,8,"Project Information", (unsigned char)projinfoAttr, (char)projinfoChar );
		if ( pWindow )
			{
 			projInfoOpen = boolean::TRUE;
			projInfoObjPtr = oPtr;

			pWindow->data = (void *)oPtr;
			oPtr->inputRoutine = ProjectInfoInput;
			oPtr->layer = pWindow;
			oPtr->inputFlags = INPF_MOUSEBUTTONS|INPF_KEY;
			pWindow->activeGadget = NULL;

			AddSysGadgets(pWindow);

			ActivateFrontWindow();
			DrawGadgets(pWindow);

			return(boolean::TRUE);                                                   // in case activated from menu
		 }
		DeleteObject(oPtr);
	 }
	return(boolean::FALSE);
	}

//=============================================================================
