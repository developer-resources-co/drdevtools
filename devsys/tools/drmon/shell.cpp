//=============================================================================
//      shell.c: dos shell routines(if they ever work)
//=============================================================================

#ifdef __MSDOS__

#include <dos.h>
#include        "base.hpp"
#include        "global.hpp"
#include        "screen.hpp"
#include        "list.hpp"
#include        "layer.hpp"
#include        "input.hpp"
#include "window.hpp"

struct dosParmBlock
{
	UWORD envString;
	char far *cmdLine;
	char far *fcb;
	char far *fcb2;
};

//=============================================================================

UWORD keepSP;
UWORD keepSS;
dosParmBlock myBlock;
char fileName[80] = "";

//=============================================================================

void
DosShell()
{
	union REGS in,out;
	unsigned int size;
	char cmdLine[2];

	myBlock.envString = NULL;
	cmdLine[0] = 0;
	cmdLine[1] = 13;                                // MT command line
	myBlock.cmdLine = (char far *)&cmdLine;
	myBlock.fcb = NULL;
	myBlock.fcb2 = NULL;

	CloseScreen();

	strcpy(fileName,getenv("COMSPEC"));
	asm{

		push    ds
							// point to parameter block
		mov     ax,seg myBlock
		mov     es,ax
		mov     bx,offset myBlock
							// point to file name string
		mov     dx,offset fileName
		mov     ax,seg fileName
		mov ds,ax
							// actually call shell
		mov     ah,4bh
		mov     al,0
		int     21h
							// restore registers
		pop     ds
	 }
	InitScreen();
	ActivateFrontWindow();
}

//=============================================================================

#endif

