//=============================================================================
// maped.c:
//=============================================================================

#include <dos.h>
#include <process.h>
#include <stdio.h>

#include "\gfxtools\lib\global.h"
#include "guifunc.h"
#include "\gfxtools\lib\input.h"
#include "\gfxtools\lib\Gui.h"
#include "guiinit.h"
#include "guidefs.h"
#include "\gfxtools\lib\general.h"
#include "mapinpt.h"

extern unsigned _stklen = (MAXSELECTX*MAXSELECTY*2) + 2000;
int num_of_maps=0, num_of_chars=0, num_of_pals=0;

//char charfilename[MAXMAPS][20];
char palfilename[MAXMAPS][20];

uint defaultXSize,defaultYSize;

//==============================================================================

main (int argc,char *argv[])
{
   int a;

	if ( argc == 1 )
		{
		Usage();
		exit( 0 );
		}

	defaultXSize = MAXMAPX;
	defaultYSize = MAXMAPY;

   currentmap=0;

   InitGUI( argc, argv );
   GUIMainLoop ();
}

//=============================================================================

extern FLAG statusBar;
short mapBotEdge = 136;
short MouseX,MouseY;
short oldMouseX,oldMouseY,mouseCoarseX,mouseCoarseY;

struct _gad *firstgadget;
struct _keygad *firstkeygadget;
struct _gad *lastpermanentgadget;

//=============================================================================

GUIMainLoop ()
{
	struct _gad *gadget;
	struct _keygad *keygadget;
	int inchar;
	struct _input in;
	in.inputMask = INPF_CASESENSE;
	DrawGUI ();
	while(1)
     {
    	while ((VGetMouse(&MouseX,&MouseY)==0)&&!kbhit())			// wait for event
		 {
			mouseCoarseX = MouseX/8;
			mouseCoarseY = MouseY/8;
			if(refreshOn && (mouseCoarseX != oldMouseX || mouseCoarseY != oldMouseY) && MouseY < mapBotEdge)
			 {
				HideMouse();
				DrawMapBox(firstgadget);
				ShowBrush(0,0,(long)0);
				ShowMouse();
			 }
			else if(refreshOn && oldMouseY < mapBotEdge/8 && MouseY > (mapBotEdge-1))
			 {
				HideMouse();
				DrawMapBox(&MapBoxGadget);
				ShowMouse();
			 }

			if(statusBar && (zoomFactor>1 || (mouseCoarseX != oldMouseX || mouseCoarseY != oldMouseY)) && MouseY < mapBotEdge)
				PrintStatusBar();

			oldMouseX = mouseCoarseX;
			oldMouseY = mouseCoarseY;
		 }

		if (kbhit ())
		 {
    		inchar = GetDirectKey(&in);
    		//gadget = firstgadget;
    		gadget = &MapBoxGadget;
    		while (gadget!=NULL)
        	 {
        		if (inchar==gadget->hotkey)
            	 {
                	HideMouse ();
                	gadget->hitfunction (0,0,gadget);
                	DrawGUI ();
                	ShowMouse ();
                	break;
            	 }
        		gadget=gadget->next;
        	 }
    		keygadget = firstkeygadget;
    		while (keygadget!=NULL)
        	 {
        		if (inchar==keygadget->hotkey)
            	 {
                	HideMouse ();
                	keygadget->hitfunction (0,0,keygadget);
                	DrawGUI ();
                	ShowMouse ();
                	break;

            	 }
        		keygadget=keygadget->next;
        	 }
    	 }
		else
		 {
    		gadget = firstgadget;
    		while (gadget!=NULL)
        	 {
        		if ((MouseX>=gadget->x&&MouseX<=gadget->x+gadget->width&&
        		MouseY>=gadget->y&&MouseY<=gadget->y+gadget->height))
            	 {
            		HideMouse ();
            		gadget->hitfunction (MouseX-gadget->x,MouseY-gadget->y,gadget);
            		DrawGUI ();
            		ShowMouse ();
            		break;
            	 }
        		gadget=gadget->next;
        	 }
		 }
	 }
}

//=============================================================================

DrawGUI ()
{
    struct _gad *gadget;
    HideMouse ();
    gadget = firstgadget;
    while (gadget!=NULL)
      {
      gadget->drawfunction (gadget);
      gadget=gadget->next;
      }
	if(refreshOn)
		ShowBrush(0,0,(long)0);
	if(statusBar)
		PrintStatusBar();
    ShowMouse ();
}


extern int vga_pal[];
void
	DosShell( void )
	{
	char *szComSpec = getenv("COMSPEC");

	SQ_get_palette( 0, 256, vga_pal );
	SetGfxMode( 3 );
	if ( spawnlp(P_WAIT,szComSpec,NULL) == -1 )
		{
		//Alert("Attempt to run %s failed!", szComSpec);
		//Alert(strerror(errno));
		}
	SetGfxMode( 0x13 );
	SQ_set_palette( 0, 256, vga_pal );
	}

//=============================================================================

