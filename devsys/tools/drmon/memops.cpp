//=============================================================================
//  memops.c: target memory operations
//=============================================================================

#include	"moninc.hpp"

//=============================================================================

#define MEMGAD_SOURCE GAD_USER+1
#define MEMGAD_LENGTH MEMGAD_SOURCE+1
#define MEMGAD_DEST MEMGAD_LENGTH+1
#define MEMGAD_FILLPAT MEMGAD_DEST+1

ULONG	globTemp,globTemp2;

//=============================================================================

FLAG
MemFillInput(_input *in,_object *oPtr)
{
	FLAG inputUsed = boolean::TRUE;
	FLAG globInputUsed = boolean::TRUE;
	FLAG iUsed = boolean::FALSE;
	FLAG leave = boolean::FALSE;
	void (*fileRoutine)(char *,char *);
	int gadgetNum;

	_gadget  *gPtr;
    _window *pWindow;
    _listRectDesc *lrPtr;

    pWindow = (_window *)oPtr->layer;

	switch(in->inputType)
	 {
		case INP_KEY:
			if(pWindow->activeGadget)
				iUsed = StrGadgInput(pWindow,pWindow->activeGadget,in);
			if(iUsed == boolean::FALSE)
				switch(in->fullKey)
	 		 	 {
					case CMD_LIST_ABORT:
						leave = boolean::TRUE;
						break;
					case KEY_RETURN:
						gadgetNum = pWindow->activeGadget->nGNum;
						if(gadgetNum == 0)
						 {
							leave = boolean::TRUE;
							break;
						 }
						else
						 {
							pWindow->activeGadget = FindGadget(&pWindow->gadgBase,gadgetNum);
							ActivateStrGadget(pWindow,pWindow->activeGadget);
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
						leave = boolean::TRUE;
						break;
					case GGAD_DRAG:
						globInputUsed = boolean::FALSE;
						break;
					default:
						pWindow->activeGadget = gPtr;
						ActivateStrGadget(pWindow,gPtr);
						break;
				 }
			 }
			break;
		default:
			inputUsed = boolean::FALSE;
			break;
	 }

	if(inputUsed)
	 {
		DrawGadgets(pWindow);
	 }

	if(leave)
	 {
		delete pWindow;
		delete oPtr;
	 }
	return(globInputUsed);
}

//=============================================================================

FLAG
GadInput(_input *in,_object *oPtr)
{
	FLAG inputUsed = boolean::TRUE;
	FLAG globInputUsed = boolean::TRUE;
	FLAG iUsed = boolean::FALSE;
	FLAG leave = boolean::FALSE;
	void (*fileRoutine)(char *,char *);
	int gadgetNum;

	_gadget  *gPtr;
    _window *pWindow;
    _listRectDesc *lrPtr;

    pWindow = (_window *)oPtr->layer;

	switch(in->inputType)
	 {
		case INP_KEY:
			if(pWindow->activeGadget)
				iUsed = StrGadgInput(pWindow,pWindow->activeGadget,in);
			if(iUsed == boolean::FALSE)
				switch(in->fullKey)
	 		 	 {
					case CMD_LIST_ABORT:
						leave = boolean::TRUE;
						break;
					case KEY_RETURN:
						gadgetNum = pWindow->activeGadget->nGNum;
						if(gadgetNum == 0)
						 {
							leave = boolean::TRUE;
							break;
						 }
						else
						 {
							pWindow->activeGadget = FindGadget(&pWindow->gadgBase,gadgetNum);
							ActivateStrGadget(pWindow,pWindow->activeGadget);
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
						leave = boolean::TRUE;
						break;
					case GGAD_DRAG:
						globInputUsed = boolean::FALSE;
						break;
					default:
						pWindow->activeGadget = gPtr;
						ActivateStrGadget(pWindow,gPtr);
						break;
				 }
			 }
			break;
		default:
			inputUsed = boolean::FALSE;
			break;
	 }

	if(inputUsed)
	 {
		DrawGadgets(pWindow);
	 }

	if(leave)
	 {
//		CloseWindow(pWindow);
		delete pWindow;
//		DeleteObject(oPtr);
		delete oPtr;
	 }
	return(globInputUsed);
}

//=============================================================================

void
MemFillGUI(ULONG value,void *dataPtr)
{
	SlaveFillMem(globTemp,globTemp2,4,(char *)&value);
}

void
GetMemFillPat(ULONG value,void *dataPtr)
{
	globTemp2 = value;
    GetExpr(MemFillGUI,dataPtr,"Enter bytes to fill with",((_object *)dataPtr)->layer);
}

void
GetMemFillLen(ULONG value,void *dataPtr)
{
	globTemp = value;
    GetExpr(GetMemFillPat,dataPtr,"Enter # of bytes to fill",((_object *)dataPtr)->layer);
}

FLAG
DoMemFill(_menuItem *iPtr,_object *oPtr,int choice)
{
    GetExpr(GetMemFillLen,oPtr,"Enter fill start address",oPtr->layer);
	return(boolean::TRUE);
}

//=============================================================================

void
MemCopyGUI(ULONG value,void *dataPtr)
{
	SlaveCopyMem(globTemp,globTemp2,value);
}

void
GetMemCopyLen(ULONG value,void *dataPtr)
{
	globTemp2 = value;
    GetExpr(MemCopyGUI,dataPtr,"Enter # of bytes to copy",((_object *)dataPtr)->layer);
}

void
GetMemCopyDest(ULONG value,void *dataPtr)
{
	globTemp = value;
    GetExpr(GetMemCopyLen,dataPtr,"Enter address to copy to",((_object *)dataPtr)->layer);
}

FLAG
DoMemCopy(_menuItem *iPtr,_object *oPtr,int choice)
{
    GetExpr(GetMemCopyDest,oPtr,"Enter address to copy from",oPtr->layer);
	return(boolean::TRUE);
}

//=============================================================================

#ifdef DEBUGZARDOZ
ulong GetZardoz3Bytes( FILE* fp )
	{
	ulong v;

	v = (unsigned long)getc( fp );
	v |= ((unsigned long)getc( fp )) << 8;
	v |= ((unsigned long)getc( fp )) << 16;

	return( v );
	}
#endif

errorcode
LoadBinaryFile(ULONG addr,char *fileName)
{
	FILE *inFile;
	ULONG inLen;
	errorcode error = NOERR;

	inFile = fopen(fileName,"rb");
	if ( !inFile )
		return(ERROR_FILENOTFOUND);

	PrintMessageBar("Loading Binary");
	SlaveStop();

#if defined( DEBUGZARDOZ )
	ubyte firstByte = getc( inFile );

	if ( firstByte == 'Z' )
		{
		ulong laddr, lsize;

		while ( ( laddr = GetZardoz3Bytes( inFile ) ) && ( error == NOERR ) )
			{
			if ( ( lsize = GetZardoz3Bytes( inFile ) ) && ( error == NOERR ) )
				{
				PrintToStatWindow( "%06lX size: %d", laddr, lsize );
				error = LoadFileToSlave( inFile, laddr, lsize );
				}
			}
		}
	else
#endif
		{
		fseek(inFile, 0L, SEEK_END);
		inLen = ftell(inFile);
		fseek(inFile, 0L, SEEK_SET );

		error = LoadFileToSlave(inFile,addr,inLen);
		}

	fclose(inFile);
	ClearMessageBar();
	ValidateBreakPts();

	return error;
}

//=============================================================================

void
LoadGUIBinaryFile(ULONG addr, void *dataPtr)
{
	errorcode error;
#pragma warn -pia
	if(error = LoadBinaryFile(addr,(char *)dataPtr))
#pragma warn +pia
		PrintError(error);
}

//=============================================================================

errorcode
SaveBinaryFile(ULONG outStart,ULONG outLen,char *fileName)
{
	FILE *outFile;
	errorcode error;

	outFile = fopen(fileName,"wb");

	if(outFile == 0)
	 {
		return(ERROR_CANTSAVEFILE);
	 }
	else
	 {
		PrintMessageBar("Saving Binary");
		error = SaveFileFromSlave(outFile,outStart,outLen);
		fclose(outFile);
		ClearMessageBar();
	 }
	return error;
}

//=============================================================================

void
SaveGUIBinaryFile(ULONG outLen,void *dataPtr)
{
	errorcode error;
#pragma warn -pia
	if(error = SaveBinaryFile(globTemp,outLen,(char *)dataPtr))
#pragma warn +pia
		PrintError(error);
}
//=============================================================================

void
GetBinFileLoadStart(void *data,char *path,char *fileName)
{
    GetExpr(LoadGUIBinaryFile,fileName,"Enter Address to load at",0);
}

void
GetBinFileSaveLen(ULONG value,void *dataPtr)
{
	globTemp = value;
    GetExpr(SaveGUIBinaryFile,dataPtr,"Enter # of bytes to save",0);
}

void
GetBinFileSaveStart(void *data,char *path,char *fileName)
{
    GetExpr(GetBinFileSaveLen,fileName,"Enter Address to save from",0);
}

//=============================================================================

FLAG
DoMemLoad(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoFileReq("Load Binary File",10,1,GetBinFileLoadStart,0);
	return(boolean::TRUE);
}

//=============================================================================

FLAG
DoMemSave(_menuItem *iPtr,_object *oPtr,int choice)
{
	DoFileReq("Save Binary File",10,1,GetBinFileSaveStart,0);
	return(boolean::TRUE);
}

//=============================================================================

menuItems memopsMenu[] =
{
	{"&Fill...     ",DoMemFill,0},
	{"&Copy...     ",DoMemCopy,0},
	{"&Load Data...",DoMemLoad,0},
	{"&Save Data...",DoMemSave,0},
	{0,0,0}
};

//=============================================================================
