//=============================================================================
//  manager.c: main control logic functions
//	manager object data usage:
//	data1 = x offset of mouse in movement mode
//  data2 = y offset of mouse in movement mode
//  data3 = old input mask
//  data4 = old object input flags
//  data5 = mouse state flag(0 = move, 1 = size)
//=============================================================================

#include	"base.hpp"
#include	"global.hpp"
#include	"mem.hpp"
#include	"general.hpp"
#include	"control.hpp"
#include	"config.hpp"
#include	"screen.hpp"
#include	"list.hpp"
#include	"keys.hpp"
#include	"monkeys.hpp"
#include	"input.hpp"
#include	"gadget.hpp"
#include	"object.hpp"
#include	"listrect.hpp"
#include	"filereq.hpp"
#include	"layer.hpp"
#include	"window.hpp"
#include 	"message.hpp"
#include	"mouse.hpp"
#include 	"menu.hpp"
#include	"macro.hpp"
#include	"error.hpp"
#include	"display.hpp"
#include	"drmon.hpp"
#include	"monwind.hpp"
#include	"textview.hpp"
#include	"reg.hpp"
#include	"slaveio.hpp"
#include	"monmenu.hpp"
#include	"help.hpp"

//=============================================================================

_object *managerObjPtr;

//=============================================================================

FLAG
WindowMoveKeys(_input *in,_object *oPtr)
{
	FLAG newSize = boolean::FALSE;
	unsigned char status;
	unsigned int fullKey;
	_window *pWindow;
	pWindow = (_window*)frontObj->layer;

	switch(in->inputType)
	 {
		case INP_MOUSEMOVE:
			if(managerObjPtr->data5 == 0)
			 {
		    	pWindow->xPos = in->mouseX - managerObjPtr->data1;
		    	pWindow->yPos = in->mouseY;
				refreshAll = boolean::TRUE;
			 }
			else
			 {
				if(pWindow->xPos < in->mouseX)
		     		pWindow->xSize = (in->mouseX - pWindow->xPos)+1;
				else
					pWindow->xSize = pWindow->xMin;

				if(pWindow->yPos < in->mouseY)
		    		pWindow->ySize = (in->mouseY - pWindow->yPos)+1;
				else
					pWindow->ySize = pWindow->yMin;
				newSize = boolean::TRUE;
			 }
			ScrollToMouse(in);
			break;
		case INP_MOUSE_LEFTBUTTON_UP:
	    	frontObj->inputRoutine = oldInputRoutine;
			PrintTitle(pWindow,windowTitleHighlightAttr);
			in->inputMask = managerObjPtr->data3;
			oPtr->inputFlags = managerObjPtr->data4;
	    	break;

		case INP_KEY:
			fullKey = in->fullKey;
			status = fullKey >> 9;
			if(status & 0x1)
				fullKey |= KEY_SHIFTED;
    		switch(fullKey)
     		{
				case CMD_MOVE_LEFT:
					if(pWindow->xPos > 1-(int)pWindow->xSize)
		 	 		 {
		    			pWindow->xPos--;
	    				refreshAll = boolean::TRUE;
		 	 		 }
	    			break;
				case CMD_MOVE_RIGHT:
					if(pWindow->xPos < (int)displayWidth-1)
		 	 		 {
	    				pWindow->xPos++;
	    				refreshAll = boolean::TRUE;
		 	 		 }
	    			break;
				case CMD_MOVE_DOWN:
					if(pWindow->yPos < (int)displayHeight-1)
		 	 		 {
	    				pWindow->yPos++;
	    				refreshAll = boolean::TRUE;
		 	 		 }
	    			break;
				case CMD_MOVE_UP:
					if(pWindow->yPos > 1-(int)pWindow->ySize)
		 	 		 {
		    			pWindow->yPos--;
	    				refreshAll = boolean::TRUE;
		 	 		 }
	    			break;
				case CMD_MOVE_LEFT|KEY_SHIFTED:
					if(pWindow->xSize > pWindow->xMin)
					pWindow->xSize--;
					newSize = boolean::TRUE;
	    			break;
				case CMD_MOVE_RIGHT|KEY_SHIFTED:
					pWindow->xSize++;
					newSize = boolean::TRUE;
	    			break;
				case CMD_MOVE_DOWN|KEY_SHIFTED:
					pWindow->ySize++;
					newSize = boolean::TRUE;
	    			break;
				case CMD_MOVE_UP|KEY_SHIFTED:
					pWindow->ySize--;
					newSize = boolean::TRUE;
	    			break;
				case CMD_MOVE_QUIT:
				case CMD_MOVE_QUIT2:
	    			frontObj->inputRoutine = oldInputRoutine;
					PrintTitle(pWindow,windowTitleHighlightAttr);
	    			break;
				case CMD_QUIT:
	    			frontObj->inputRoutine = oldInputRoutine;
					PrintTitle(pWindow,windowTitleHighlightAttr);
	    			cont = boolean::FALSE;
	    			break;
				default:
	    			break;
     		 }
			break;
	 }
	if(newSize)
	 {
		if(pWindow->xSize > pWindow->xMax)
			pWindow->xSize = pWindow->xMax;
		if(pWindow->xSize < pWindow->xMin)
			pWindow->xSize = pWindow->xMin;
		if(pWindow->ySize > pWindow->yMax)
			pWindow->ySize = pWindow->yMax;
		if(pWindow->ySize < pWindow->yMin)
			pWindow->ySize = pWindow->yMin;

		if(pWindow->xMax != pWindow->xMin || pWindow->yMax != pWindow->yMin)
			ForceWindowResize(pWindow,oPtr,oldInputRoutine);
	 }
								// when in movement mode, cannot do any global commands
    return(boolean::TRUE);
}

//=============================================================================

errorcode
LogWindow(_layer *lPtr)
 {
	int x,y;
	unsigned char ch;
	FILE *outFile = NULL;
    unsigned char far *sBuff;
    sBuff = lPtr->buffer;

	if(logFileName[0] != 0)
		outFile = fopen(logFileName,"a");
	if(!outFile)
		return(ERROR_CANTOPENLOG);
    for(y=0;y<lPtr->ySize;++y)
  	 	{
		for(x=0;x<lPtr->xSize;++x)
	 	 	{
			ch = *sBuff++;
			if(ch < ' ')
				ch = ' ';
	    	fputc(ch,outFile);
	    	sBuff++;
	 	 	}
		fputc('\n',outFile);
     	}
	fclose(outFile);
	return(NOERR);
}

//==============================================================================

FLAG
GlobInput(_input *in,_object *oPtr)
{
	FLAG inputUsed = boolean::TRUE;
	_gadget *gPtr;
	_window* pWindow;
	_layer *l2Ptr;
	errorcode error;

	pWindow = (_window*)oPtr->layer;

	switch(in->inputType)
	 {
		case INP_MOUSEMOVE:
			ScrollToMouse(in);
			inputUsed = boolean::FALSE;			// input not that usefull, so lie
			break;
		case INP_MOUSE_LEFTBUTTON_DOWN:
			if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 {
				switch(gPtr->gNum)
				 {
					case GGAD_CLOSE:
						break;
					case GGAD_RESIZE:
	    				oldInputRoutine = frontObj->inputRoutine;
	    				frontObj->inputRoutine = WindowMoveKeys;
						PrintTitle((_window*)frontObj->layer,windowTitleMoveAttr);
						managerObjPtr->data1 = in->mouseX - frontObj->layer->xPos;
						managerObjPtr->data3 = in->inputMask;
						managerObjPtr->data4 = oPtr->inputFlags;
						managerObjPtr->data5 = 1;
						in->inputMask = INPF_MOUSEMOVE|INPF_MOUSEBUTTONS|INPF_KEY;
						oPtr->inputFlags = INPF_MOUSEMOVE|INPF_MOUSEBUTTONS;
						break;
					case GGAD_ZOOM:
							ZoomWindow(oPtr);
						break;
					case GGAD_DRAG:
	    				oldInputRoutine = frontObj->inputRoutine;
	    				frontObj->inputRoutine = WindowMoveKeys;
						PrintTitle((_window*)frontObj->layer,windowTitleMoveAttr);
						managerObjPtr->data1 = in->mouseX - frontObj->layer->xPos;
						managerObjPtr->data3 = in->inputMask;
						managerObjPtr->data4 = oPtr->inputFlags;
						managerObjPtr->data5 = 0;
						in->inputMask = INPF_MOUSEMOVE|INPF_MOUSEBUTTONS|INPF_KEY;
						oPtr->inputFlags = INPF_MOUSEMOVE|INPF_MOUSEBUTTONS;
						break;
					default:
						inputUsed = boolean::FALSE;
						break;
				 }
			 }
			break;
		case INP_MOUSE_LEFTBUTTON_SHIFTDOWN:
		 {
			int startX,startY;
			unsigned char far *sPtr;
			startX = in->mouseX;
			startY = in->mouseY;
			refreshEnable = boolean::FALSE;
			while(1)
			 {
				if(InputPending(&inBase))
					if(inBase.inputType == INP_MOUSE_LEFTBUTTON_UP)
						break;
				XorDisplayRect(startX-virtualXOffset,startY-virtualYOffset,(inBase.mouseX-1)-virtualXOffset,startY-virtualYOffset);
				UpdateScreen();
				XorDisplayRect(startX-virtualXOffset,startY-virtualYOffset,(inBase.mouseX-1)-virtualXOffset,startY-virtualYOffset);
			 }
			ReadDisplayRect(&copyBuffer[0],startX-virtualXOffset,startY-virtualYOffset,(inBase.mouseX-1)-virtualXOffset,startY-virtualYOffset);
									// kts do actual read here
			refreshEnable = boolean::TRUE;
			break;
		 }
		case INP_KEY:
    		switch(in->fullKey)
     		 {
				case CMD_LOGWINDOW:
					error = LogWindow(frontObj->layer);
					if(error)
						PrintError(error);
					break;
				case CMD_QUIT:
				case CMD_QUIT+KEY_SCROLLLOCK:
	    			cont = boolean::FALSE;
	    			break;
				case CMD_OPENMEMORY:
	    			OpenMemory();
					break;
				case CMD_OPENREG:
	    			OpenReg();
					break;
				case CMD_OPENEXPR:
	    			OpenExpWindow();
					break;
				case CMD_OPENCMD:
					OpenCmdWindow();
					break;
				case CMD_OPENBREAK:
	    			OpenBreak();
					break;
				case CMD_OPENCONSOLE:
	    			OpenConsole();
					break;
				case CMD_OPENSYMBOL:
	    			OpenSymbol();
					break;
				case CMD_OPENWATCH:
					OpenWatch();
					break;
				case CMD_OPENFILEVIEW:
					OpenTextView();
					break;
				case CMD_OPENHELPFILE:
					OpenHelpFile();
					break;
				case CMD_OPENSOURCEFILE:
					OpenSource();
					break;
				case CMD_OPENABOUT:
					OpenAbout();
					break;
				case CMD_OPENPROJINFO:
					OpenProjectInfo();
					break;
				case CMD_OPENASCII:
					OpenASCIIChart();
					break;
//				case CMD_OPENPLATHELP:
//					OpenRefFile(EXT_PLATFORM,PLATFORMNAME,NULL);
//					break;
				case CMD_MENU:
					ActivateMenuBar();
					break;
				case CMD_DOSSHELL:
#ifdef __MSDOS__
					DosShell();
#endif
					break;
				case CMD_NEXTWINDOW:
					 (layBase.Tail())->ToBack();
					ActivateFrontWindow();
					break;
				case CMD_MOVEWIN:
	    			oldInputRoutine = frontObj->inputRoutine;
					managerObjPtr->data3 = in->inputMask;
	    			frontObj->inputRoutine = WindowMoveKeys;
					PrintTitle((_window*)frontObj->layer,windowTitleMoveAttr);
	    			break;
				case CMD_ZOOMWINDOW:
					ZoomWindow(oPtr);
					break;
				case CMD_RUN:
					DoSlaveRunWithUpdates();
	    			break;
				case CMD_STOP:
					SlaveStop();
	    			break;
				case CMD_RUNNOUPDATE:
					SlaveRunNoUpdate();
	    			break;
    			case CMD_STEP:
					if(stepMode == SM_SOURCE)
						SlaveStepSource();
					else
        				SlaveStep();
            		break;
				case CMD_STEPOVER:
					if(stepMode == SM_SOURCE)
						SlaveStepOverSource();
					else
						SlaveStepOver();
					break;
    			case CMD_STEPSOURCE:
        			SlaveStepSource();
            		break;
				case CMD_STEPOVERSOURCE:
					SlaveStepOverSource();
					break;
    			case CMD_STEPASSEMBLY:
        			SlaveStep();
            		break;
				case CMD_STEPOVERASSEMBLY:
					SlaveStepOver();
					break;
				case KEY_ALT0:
				case KEY_ALT1:
				case KEY_ALT2:
				case KEY_ALT3:
				case KEY_ALT4:
				case KEY_ALT5:
				case KEY_ALT6:
				case KEY_ALT7:
				case KEY_ALT8:
				case KEY_ALT9:
					if(l2Ptr = FindNumberedWindow(in->fullKey-((KEY_ALT1)-1)))
					 {
						l2Ptr->ToFront();
						ActivateFrontWindow();
					 }
					break;
				case KEY_LEFT+KEY_SCROLLLOCK:
					ScrollDisplayLeft(1);
					break;
				case KEY_RIGHT+KEY_SCROLLLOCK:
					ScrollDisplayRight(1);
					break;
				case KEY_UP+KEY_SCROLLLOCK:
					ScrollDisplayDown(1);
					break;
				case KEY_DOWN+KEY_SCROLLLOCK:
					ScrollDisplayUp(1);
					break;
				default:
					inputUsed = boolean::FALSE;
	    			break;
     		 }
			break;
		default:
			inputUsed = boolean::FALSE;
	    	break;
	 }
	if(inputUsed)
		MessageBarTick();
	return(inputUsed);
}

//=============================================================================

void
MouseActivateWindow(_input *in)
{
	_layer *pWindow;

	if((pWindow = GetLayerMouseOver(in)) != 0)
	 {
	    if(pWindow->data != 0)  			// insure input window is found
		 {
			pWindow->ToFront();
			ActivateFrontWindow();
		 }
	 }
}

//=============================================================================

void
Manager(_object *oPtr)
{
	_input *in;
	_input inB;

	in = &inBase;

	HandleSlaveInput();

	in->inputMask = frontObj->inputFlags | INPF_MOUSEMOVE;

    while(InputPending(&inBase))
     {
		// kts 04-02-94 04:44am
		RemapEvent(&inBase,&inBase,&frontObj->remap);

		switch(in->inputType)
		 {
			case INP_KEY:
				if(frontObj->inputRoutine != NULL && (frontObj->inputFlags & INPF_KEY))
	 	 	 	 {
	    			if((*frontObj->inputRoutine)(in,frontObj) == boolean::FALSE)
						GlobInput(in,frontObj);
	 	 	 	 }
				else
	    			GlobInput(in,frontObj);
				break;
			case INP_MOUSEMOVE:
				if(frontObj->inputRoutine != NULL && frontObj->inputFlags & INPF_MOUSEMOVE)
//				if(frontObj->inputRoutine != NULL && in->inputMask & INPF_MOUSEMOVE)
				 {
  					if(frontObj->inputRoutine(in,frontObj) == boolean::FALSE)
						GlobInput(in,frontObj);
				 }
				else
	    			GlobInput(in,frontObj);
				break;
			case INP_MOUSE_LEFTBUTTON_DOWN:
				if(MouseInsideWindow(in,frontObj->layer) == boolean::TRUE)
			 	 {
					if((frontObj->inputFlags & INPF_MOUSEBUTTONS) != 0)
					 {
						if(frontObj->inputRoutine(in,frontObj) == boolean::FALSE)
							GlobInput(in,frontObj);
					 }
					else
						GlobInput(in,frontObj);
			 	 }
				else if(menuUp == boolean::FALSE)
					MouseActivateWindow(in);
				break;
			default:
				if((frontObj->inputFlags & INPF_MOUSEBUTTONS) != 0)
				 {
					if(frontObj->inputRoutine(in,frontObj) == boolean::FALSE)
						GlobInput(in,frontObj);
				 }
				else
					GlobInput(in,frontObj);
				break;
		 }
     }
	if(frontObj->layer->cursorOn == boolean::TRUE)
		PositionCursor(frontObj->layer->cursorX+frontObj->layer->xPos,frontObj->layer->cursorY+frontObj->layer->yPos);
}

//=============================================================================

void
InitManager(void)
{
    managerObjPtr = AddObject(Manager);
	InitMenuBar();
	InitMessageBar();
}

//=============================================================================
