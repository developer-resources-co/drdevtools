//============================================================================
//      info.c:  Project information statistics (?)
//============================================================================

#include "moninc.hpp"
#include "help.hpp"
#include "coff.hpp"

#if defined( __WATCOMC__ )
#include <i86.h>

struct meminfo {
    unsigned LargestBlockAvail;
    unsigned MaxUnlockedPage;
    unsigned LargestLockablePage;
    unsigned LinAddrSpace;
    unsigned NumFreePagesAvail;
    unsigned NumPhysicalPagesFree;
    unsigned TotalPhysicalPages;
    unsigned FreeLinAddrSpace;
    unsigned SizeOfPageFile;
    unsigned Reserved[3];
} MemInfo;
#endif

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

#ifdef __MSDOS__
	size = coreleft();
#endif

#ifdef __WATCOMC__
#define DPMI_INT	0x31

    union REGS regs;
    struct SREGS sregs;

    regs.x.eax = 0x00000500;
    memset( &sregs, 0, sizeof(sregs) );
    sregs.es = FP_SEG( &MemInfo );
    regs.x.edi = FP_OFF( &MemInfo );

    int386x( DPMI_INT, &regs, &regs, &sregs );
	size = MemInfo.LargestBlockAvail;
#if 0
    printf( "Largest available block (in bytes): %lu\n",
	    MemInfo.LargestBlockAvail );
    printf( "Maximum unlocked page allocation: %lu\n",
	    MemInfo.MaxUnlockedPage );
    printf( "Pages that can be allocated and locked: %lu\n",
	    MemInfo.LargestLockablePage );
    printf( "Total linear address space including allocated"
	    " pages: %lu\n", MemInfo.LinAddrSpace );
    printf( "Number of free pages available: %lu\n",
	     MemInfo.NumFreePagesAvail );
    printf( "Number of physical pages not in use: %lu\n",
	     MemInfo.NumPhysicalPagesFree );
    printf( "Total physical pages managed by host: %lu\n",
	     MemInfo.TotalPhysicalPages );
    printf( "Free linear address space (pages): %lu\n",
	     MemInfo.FreeLinAddrSpace );
    printf( "Size of paging/file partition (pages): %lu\n",
	     MemInfo.SizeOfPageFile );
#endif

#endif

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

#ifdef DEBUGCOFF
	if ( coffBase )
		{
		}
#endif
}


FLAG
ProjectInfoInput(_input *in,_object *oPtr)
{
	_window *pWindow;
	FLAG inputUsed;
	unsigned char far *chr;
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
