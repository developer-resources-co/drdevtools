//=============================================================================
//  filereq.c: file requester
//=============================================================================

#include <assert.h>

#include <dos.h>

//=============================================================================
// drmon include files

#include "base.hpp"
#include "global.hpp"
#include	"general.hpp"
#include	"config.hpp"
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
#include	"drmon.hpp"
#include	"error.hpp"

//=============================================================================

enum {
	FRGAD_DIR  =	GAD_USER+1,
	FRGAD_FILE,
	FRGAD_OK,
	FRGAD_CANCEL,
	FRGAD_DRIVE,
	FRGAD_PATTERN,
	FRGAD_FLIST,
	FRGAD_DLIST = FRGAD_FLIST + LGAD_COUNT,
	FRGAD_COUNT = FRGAD_DLIST + LGAD_COUNT
	};

enum { FREQ_YSIZE =	23 };

struct _fileList
{
	_stringList list;
};

//=============================================================================

_fileList fileListBase;
_fileList dirListBase;
char patternString[_MAX_PATH] = "*.*         ";
static char fileReqFileName[_MAX_PATH] = "            ";
static char fileReqDirName[_MAX_PATH]  = "            ";	// dir field (was a SetGadgString heap buf, cap 13)

//=============================================================================


void
ScanDir(void)
{
	int i,more;
	FLAG stop;
	_fileList *listNode;
	find_t fileBlock;

	InitListBase((_list *)&fileListBase);
	InitListBase((_list *)&dirListBase);

	stop = boolean::FALSE;

	// first do directories
	more = _dos_findfirst("*.*",FA_DIREC,&fileBlock);
	while(!more && !stop)
	 {
		if((listNode = (_fileList *)	malloc(sizeof(_fileList))) != 0)
		 {
#pragma	warn -pia
			if(listNode->list.Name(DupString(fileBlock.name)))
#pragma	warn +pia
			 {
					if((fileBlock.attrib & FA_DIREC))
	   					InsertListNode(GetLastNode((_list *)&dirListBase),(_list *)listNode);
			 }
			else
		 	 {
				free(listNode);
				PrintError(ERROR_NOMEM);
				stop = boolean::TRUE;
		 	 }
		 }
		else
		 {
			PrintError(ERROR_NOMEM);
			stop = boolean::TRUE;
		 }
		more = _dos_findnext(&fileBlock);
	 }

	// now do files
	more = _dos_findfirst(patternString,0,&fileBlock);
	while(!more && !stop)
	 {
		if((listNode = (_fileList *)	malloc(sizeof(_fileList))) != 0)
		 {
#pragma	warn -pia
			if(listNode->list.Name(DupString(fileBlock.name)))
#pragma	warn +pia
			 {
//					if((fileBlock.attrib & FA_DIREC))
//						;
//					else
	   					InsertListNode(GetLastNode((_list *)&fileListBase),(_list *)listNode);
			 }
			else
		 	 {
				free(listNode);
				PrintError(ERROR_NOMEM);
				stop = boolean::TRUE;
		 	 }
		 }
		else
		 {
			PrintError(ERROR_NOMEM);
			stop = boolean::TRUE;
		 }
		more = _dos_findnext(&fileBlock);
	 }

	SortListRect((_stringList *)&fileListBase);
	SortListRect((_stringList *)&dirListBase);
	return;
}

//===========================================================================
#include <direct.h>

void
AddFileReqGadgets(_window *pWindow)
{
							// dir gadget
	_gadget  *gPtr;
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 12;
	gPtr->ySize = 1;
	gPtr->xPos = 2;
	gPtr->yPos = FREQ_YSIZE-5;
	gPtr->gadgText = fileReqDirName;
	gPtr->gNum = FRGAD_DIR;
	gPtr->gadgAttr = windowBorderAttr;

							// file gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 12;
	gPtr->ySize = 1;
	gPtr->xPos = 22;
	gPtr->yPos = FREQ_YSIZE-5;
	gPtr->gadgText = fileReqFileName;	// writable module buffer (char[13]); edited in place
	gPtr->gNum = FRGAD_FILE;
	gPtr->gadgAttr = windowBorderAttr;

							// drive gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 1;
	gPtr->ySize = 1;
	gPtr->xPos = 2;
	gPtr->yPos = FREQ_YSIZE-3;
	SetGadgString(gPtr," ");
	gPtr->gadgText[0] = getdisk()+'A';
	gPtr->gNum = FRGAD_DRIVE;
	gPtr->gadgAttr = windowBorderAttr;

							// pattern gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 12;
	gPtr->ySize = 1;
	gPtr->xPos = 9;
	gPtr->yPos = FREQ_YSIZE-3;
	gPtr->gadgText = (char *)patternString;
	gPtr->gNum = FRGAD_PATTERN;
	gPtr->gadgAttr = windowBorderAttr;

							// ok gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 4;
	gPtr->ySize = 1;
	gPtr->xPos = 32;
	gPtr->yPos = FREQ_YSIZE-3;
	gPtr->gadgText = "[Ok]";
	gPtr->gNum = FRGAD_OK;
	gPtr->gadgAttr = windowBorderAttr;

							// cancel gadget
	gPtr = NewGadget(&pWindow->gadgBase);
	gPtr->xSize = 7;
	gPtr->ySize = 1;
	gPtr->xPos = 22;
	gPtr->yPos = FREQ_YSIZE-3;
	gPtr->gadgText = "[Cancel]";
	gPtr->gNum = FRGAD_CANCEL;
	gPtr->gadgAttr = windowBorderAttr;
}

//=============================================================================
//// dataPtr->currently active string gadget struct
// dataPtr2->code to run when file is choosen
// dataPtr3->file list rect
// dataPtr4->directory list rect
// dataPtr5-> user data, whatever, goes into fileRoutine

void
DoFileReq(char* title,int xPos,int yPos,void (*fileRoutine)(void* data,char* path,char* fileName),void* data)
{
	_object* oPtr;
	_window* pWindow;
	_gadget* gPtr;
    _listRectDesc* lrPtr;
	_stringList* sPtr;
	ScanDir();
    pWindow = new _window(xPos,yPos,40,FREQ_YSIZE,title,(unsigned char)fileReqAttr,(char)fileReqChar,"\xC9\xCD\xBB\xBA\xBA\xC8\xCD\xBC");
	oPtr = AddObject();
	oPtr->inputFlags = INPF_KEY|INPF_MOUSEBUTTONS;
	oPtr->inputRoutine = FileReqInput;
	oPtr->layer =pWindow;
	pWindow->activeGadget = NULL;
	oPtr->dataPtr2 = (char*)fileRoutine;
	pWindow->data = (void*)oPtr;

	PrintLay(pWindow,"Directory",2,FREQ_YSIZE-6);
	PrintLay(pWindow,"File",22,FREQ_YSIZE-6);
	PrintLay(pWindow,"Drive",2,FREQ_YSIZE-4);
	PrintLay(pWindow,"Pattern",9,FREQ_YSIZE-4);
	AddSysGadgets(pWindow);
	lrPtr = AddListRect(pWindow, 12, FREQ_YSIZE-8, (_stringList*)&fileListBase, FRGAD_FLIST, 22, 2, 1);
	oPtr->dataPtr3 = (char*)lrPtr;
	oPtr->dataPtr4 = (char*)AddListRect(pWindow, 12, FREQ_YSIZE-8, (_stringList*)&dirListBase, FRGAD_DLIST, 2, 2, 1);
	oPtr->dataPtr5 = (char*)data;
	AddFileReqGadgets(pWindow);

	sPtr = (_stringList*)lrPtr->listPtr->Next();
	if(sPtr)
	 {
		gPtr = FindGadget(&pWindow->gadgBase,FRGAD_FILE);
		snprintf(gPtr->gadgText,_MAX_PATH,"%s",sPtr->Name());
		PadString(gPtr->gadgText,gPtr->xSize);
	 }

	lrPtr = (_listRectDesc* )oPtr->dataPtr4;
	sPtr = (_stringList* )lrPtr->listPtr->Next();
	if(sPtr)
	 {
		gPtr = FindGadget(&pWindow->gadgBase,FRGAD_DIR);
		snprintf(gPtr->gadgText,_MAX_PATH,"%s",sPtr->Name());
		PadString(gPtr->gadgText,gPtr->xSize);
	 }

	DrawGadgets(pWindow);

	pWindow->activeGadget = FindGadget(&pWindow->gadgBase,FRGAD_FILE);
	ActivateStrGadget(pWindow,pWindow->activeGadget);
	ActivateFrontWindow();
}

//=============================================================================

void
FreeDirList(_list* lPtr)
{
	while(lPtr->next)
	 {
		free(((_stringList *)lPtr->next)->Name());
		DeleteListNode(lPtr->next);
	 }
}

//=============================================================================

void
FreeDir(void)
{
	FreeDirList((_list *)&fileListBase);
	FreeDirList((_list *)&dirListBase);
}

//=============================================================================

FLAG
FileReqInput(_input *in,_object *oPtr)
{
	FLAG inputUsed = boolean::TRUE;
	FLAG globInputUsed = boolean::TRUE;
	FLAG iUsed = boolean::FALSE;
	FLAG leave = boolean::FALSE;
	FLAG dirIn = boolean::FALSE;
	FLAG fileIn = boolean::FALSE;
	FLAG refresh = boolean::FALSE;
	int newDrive;
	void (*fileRoutine)(void *,char *,char *);

	_gadget  *gPtr;
    _window *pWindow;
    _listRectDesc *lrPtr;
	_stringList *fileListPtr,*dirListPtr;

    pWindow = (_window *)oPtr->layer;
	assert( pWindow );

	if((in->inputType != INP_KEY) || pWindow->activeGadget->gNum == FRGAD_FILE)
		fileIn = ListRectInput(in,oPtr,(_listRectDesc *)oPtr->dataPtr3,FRGAD_FLIST);

	if((in->inputType != INP_KEY) || pWindow->activeGadget->gNum == FRGAD_DIR)
		dirIn = ListRectInput(in,oPtr,(_listRectDesc *)oPtr->dataPtr4,FRGAD_DLIST);

	if((fileIn & LRIF_INPUTUSED)  == boolean::FALSE && (dirIn & LRIF_INPUTUSED) == boolean::FALSE)
	 {
		switch(in->inputType)
	 	 {
			case INP_KEY:
				if(pWindow->activeGadget)
					iUsed = StrGadgInput(pWindow,pWindow->activeGadget,in);
				if(iUsed)
				 {
					strcpy(textBuffer,pWindow->activeGadget->gadgText);
					UnPadString(textBuffer);
					//gPtr = FindGadget(&pWindow->gadgBase,FRGAD_DIR);
					//strcpy(gPtr->gadgText,dirListPtr->name);
					//PadString(gPtr->gadgText,gPtr->xSize);
				 }
				else
				//if(iUsed == boolean::FALSE)
					switch(in->fullKey)
	 		 	 	 {
						case CMD_CLOSEWINDOW:
						case CMD_LIST_ABORT:
							leave = boolean::TRUE;
							break;
						case KEY_RETURN:
							switch(pWindow->activeGadget->gNum)
							 {
								case FRGAD_FILE:
									leave = boolean::TRUE;
									fileRoutine = (void (*)(void *,char *,char *))oPtr->dataPtr2;
									gPtr = FindGadget(&pWindow->gadgBase,FRGAD_FILE);
									(*fileRoutine)(oPtr->dataPtr5,FindGadget(&pWindow->gadgBase,FRGAD_DIR)->gadgText,gPtr->gadgText);
									break;
								case FRGAD_DIR:
									dirIn |= LRIF_NEWSEL;
									break;
								case FRGAD_DRIVE:
									newDrive = pWindow->activeGadget->gadgText[0] - 'A';
									if(newDrive >= 0 && newDrive < setdisk(getdisk()))
									 {
										setdisk(newDrive);
										refresh = boolean::TRUE;
										//fileIn |= LRIF_NEWSEL;
									 }
									break;
								case FRGAD_PATTERN:
									refresh = boolean::TRUE;
									break;
							 }
							break;
						case KEY_TAB:
							switch(pWindow->activeGadget->gNum)
							 {
								case FRGAD_FILE:
									pWindow->activeGadget = FindGadget(&pWindow->gadgBase,FRGAD_DRIVE);
									ActivateStrGadget(pWindow,pWindow->activeGadget);
							   		break;
								case FRGAD_DIR:
									pWindow->activeGadget = FindGadget(&pWindow->gadgBase,FRGAD_FILE);
									ActivateStrGadget(pWindow,pWindow->activeGadget);
									break;
								case FRGAD_DRIVE:
									pWindow->activeGadget = FindGadget(&pWindow->gadgBase,FRGAD_PATTERN);
									ActivateStrGadget(pWindow,pWindow->activeGadget);
									break;
								case FRGAD_PATTERN:
									pWindow->activeGadget = FindGadget(&pWindow->gadgBase,FRGAD_DIR);
									ActivateStrGadget(pWindow,pWindow->activeGadget);
									break;
							 }
							break;
						case CMD_QUIT:
	    					cont = boolean::FALSE;
	    					break;
						default:
							inputUsed = boolean::FALSE;
							break;
	 		 	 	 }
				break;
			case INP_MOUSEMOVE:
				break;
			case INP_MOUSE_LEFTBUTTON_UP:
				break;
			case INP_MOUSE_LEFTBUTTON_DOWN:
				if((gPtr = GadgetHit(&pWindow->gadgBase,in->mouseX - pWindow->xPos,in->mouseY - pWindow->yPos)) != 0)
			 	 {
					switch(gPtr->gNum)
				 	 {
						case GGAD_CLOSE:
						case FRGAD_CANCEL:
							leave = boolean::TRUE;
							break;
						case FRGAD_DIR:
						case FRGAD_FILE:
						case FRGAD_DRIVE:
						case FRGAD_PATTERN:
							pWindow->activeGadget = gPtr;
							ActivateStrGadget(pWindow,gPtr);
							break;
						case FRGAD_OK:
							leave = boolean::TRUE;
							fileRoutine = (void (*)(void *,char *,char *))oPtr->dataPtr2;
							gPtr = FindGadget(&pWindow->gadgBase,FRGAD_FILE);
							(*fileRoutine)(oPtr->dataPtr5,FindGadget(&pWindow->gadgBase,FRGAD_DIR)->gadgText,gPtr->gadgText);
							break;
						case GGAD_DRAG:
							globInputUsed = boolean::FALSE;
							break;
						default:
							inputUsed = boolean::FALSE;
							break;
				 	 }
			 	 }
				break;
			default:
				inputUsed = boolean::FALSE;
				break;
	 	 }
	 }

	lrPtr = (_listRectDesc *)oPtr->dataPtr4;
	dirListPtr = (_stringList *)SkipNodes((_list *)lrPtr->listPtr,lrPtr->selItem+1);
	if((dirIn & LRIF_INPUTUSED) == boolean::TRUE)			// if list req used input, must update string gadgets
	 {
		gPtr = FindGadget(&pWindow->gadgBase,FRGAD_DIR);
		snprintf(gPtr->gadgText,_MAX_PATH,"%s",dirListPtr->Name());
		PadString(gPtr->gadgText,gPtr->xSize);
	 }

	if(dirIn & LRIF_NEWSEL)
	 {
		// change directorys
		gPtr = FindGadget(&pWindow->gadgBase,FRGAD_DIR);
		strcpy(textBuffer,gPtr->gadgText);
		UnPadString(textBuffer);
		chdir(textBuffer);
		refresh = boolean::TRUE;
	 }
	if(refresh)
	 {
		FreeDir();
		ScanDir();
		ChangeListRect(pWindow,(_listRectDesc *)oPtr->dataPtr3,(_stringList *)&fileListBase);
		ChangeListRect(pWindow,(_listRectDesc *)oPtr->dataPtr4,(_stringList *)&dirListBase);
		dirIn |= LRIF_INPUTUSED;
		fileIn |= LRIF_INPUTUSED;
	 }

	lrPtr = (_listRectDesc *)oPtr->dataPtr4;
	if(lrPtr->listPtr->Next())
		dirListPtr = (_stringList *)SkipNodes((_list *)lrPtr->listPtr,lrPtr->selItem+1);
	else
		dirListPtr = NULL;

	if((dirIn & LRIF_INPUTUSED) == boolean::TRUE)			// if list req used input, must update string gadgets
	 {
		gPtr = FindGadget(&pWindow->gadgBase,FRGAD_DIR);
		if(dirListPtr)
			snprintf(gPtr->gadgText,_MAX_PATH,"%s",dirListPtr->Name());
		else
			*gPtr->gadgText = 0;							// null terminate
		PadString(gPtr->gadgText,gPtr->xSize);
	 }

	lrPtr = (_listRectDesc *)oPtr->dataPtr3;
	if(lrPtr->listPtr->Next())
		fileListPtr = (_stringList *)SkipNodes((_list *)lrPtr->listPtr,lrPtr->selItem+1);
	else
		fileListPtr = NULL;

	if((fileIn & LRIF_INPUTUSED) == boolean::TRUE)			// if list req used input, must update string gadgets
	 {
		gPtr = FindGadget(&pWindow->gadgBase,FRGAD_FILE);
		if(fileListPtr)
			snprintf(gPtr->gadgText,_MAX_PATH,"%s",fileListPtr->Name());
		else
			*gPtr->gadgText = 0;							// null terminate
		PadString(gPtr->gadgText,gPtr->xSize);
	 }

	if(inputUsed)
	 {
		UpdateListRect(pWindow,(_listRectDesc *)oPtr->dataPtr3,FRGAD_FLIST);
		UpdateListRect(pWindow,(_listRectDesc *)oPtr->dataPtr4,FRGAD_DLIST);
		DrawGadgets(pWindow);
	 }

	if(leave)
	 {
		delete pWindow;
//		CloseWindow(pWindow);
		delete (_listRectDesc *)oPtr->dataPtr3;
		delete (_listRectDesc *)oPtr->dataPtr4;
		FreeDir();
//		DeleteObject(oPtr);
		delete oPtr;
	 }
	return(globInputUsed);
}

//=============================================================================
