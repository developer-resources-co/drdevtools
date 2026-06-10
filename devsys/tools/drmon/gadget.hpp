//=============================================================================
//  gadget.hpp: hit box handling
//=============================================================================

#ifndef DRMON_gadget_H
#define DRMON_gadget_H

//=============================================================================

#include "list.hpp"
#include "input.hpp"
#include "layer.hpp"

//=============================================================================

	enum
{
	GGAD_NONE,
	GGAD_CLOSE,
	GGAD_RESIZE,
	GGAD_DRAG,
	GGAD_ZOOM,
	GAD_USER  = 100
};

#define GAD_USER 100

#define	STRGADB_MODE_INSERT 0
#define	STRGADF_MODE_INSERT 1<<STRGADB_MODE_INSERT


enum
{
	GADGTYPE_NONE,
	GADTYPE_STRING,
	GADTYPE_BUTTON,
	GADTYPE_TOGGLEBUTTON
};

#define GFLAGB_TERMINATE 0
#define GFLAGF_TERMINATE 1<<GFLAGB_TERMINATE

//=============================================================================

struct _gadget
{
	_list link;
	unsigned int xPos;
	unsigned int yPos;
	unsigned int xSize;
	unsigned int ySize;
	unsigned int cursorX;						// cursor x position inside text box
	unsigned int mode;				// contains all the mode bits for the string gadget(like insert)
    void (*routine)(void);	    	// called whenever gadget hit
	unsigned char gadgAttr;			// text attribute for this gadget
	char* gadgText;					// text repersentation of gadget
	int gFlags;						// gadget flags
	int gType;						// gadget type
	int gNum;						// externally assigned gadget #
	int pGNum;						// parent gadget #, used by scroll bar box, & by string gadgets for tab looping
	int nGNum;						// next gadget, if set, gadget # of next gadget to activate
	int data;						// user data, whatever you want
};

//=============================================================================

class _window;

extern _gadget *pGadgDown;
extern uint insertDefault;

//==============================================================================

int
AutoCenterXOnDisplay(_layer* lPtr,int size);

int
AutoCenterYOnDisplay(_layer* lPtr,int size);

void
InitGadgetList(_gadget* gBase);

_gadget* 
NewGadget(_gadget* gList);

_gadget* 
FindGadget(_gadget* gPtr,int gNum);

_gadget* 
NewGadgetInit(_gadget* gList,int xPos,int yPos,int xSize, int ySize);

_gadget* 
MakeGadget(_window* pWindow,int xPos,int yPos,int xSize,int ySize,int gNum);

void
DeleteGadget(_gadget* gPtr);

void
DeleteAllGadgets(_gadget* gPtr);

_gadget* 
GadgetHit(_gadget* gList,int xPos,int yPos);

void
DrawGadgets(_window* pWindow);

void
AddSysGadgets(_window* pWindow);

void
AddVScrollGadgetsCoord(_window* pWindow, int upNum,int downNum,int scrollNum,int scBoxNum,int xPos,int yTop, int yBot);

void
AddVScrollGadgets(_window* pWindow,int upNum,int downNum,int scrollNum,int scBoxNum);

void
UpdateVScrollGadg(_window* pWindow,unsigned int gNum,uword prop, uword propShown=0);

unsigned int
GetPropGadgPos(_window* pWindow,unsigned int gNum,int yOffset);

unsigned int
GetNormalizedPropGadgPos(_window* pWindow,unsigned int gNum, int yOffset,unsigned int maxVal);

void
PropGadgDown(_object* pObject, _input* in,_gadget* gPtr);

FLAG
PropGadgUp(_object* pObject, _input* in);

void
ActivateStrGadget(_window* pWindow,_gadget* gPtr);

void
SetGadgString(_gadget* gPtr,const char* text);

FLAG
StrGadgInput(_window* pWindow,_gadget* gPtr,_input* in);

void
DeactivateStrGadget(_window* pWindow);

void
GetString(void (*strRoutine)(char* string,void* dataPtr),void* dataPtr,char* title,_layer* backLPtr);

//=============================================================================

#endif

//=============================================================================
