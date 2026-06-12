//=============================================================================
//  layer.hpp: layer header
//=============================================================================

#ifndef _layer_hpp_
#define _layer_hpp_

//=============================================================================

#include "list.hpp"

//=============================================================================

#define layb_Changed 0
#define layf_Changed 1<<layb_Changed

#define layb_Front 1
#define layf_Front 1<<layb_Front

enum
	{
	BOR_UL,
	BOR_UM,
	BOR_UR,
	BOR_ML,
	BOR_MR,
	BOR_DL,
	BOR_DM,
	BOR_DR
	};

#define	BORDER_X	2
#define	BORDER_Y 	2

enum
	{
	ATTR_INVERSE 		= 0x70U,
	ATTR_HIGHLIGHT 	= 0x0FU,
	ATTR_NORMAL 		= 0x07U,
	};

enum
	{
	ATTR_BACKBLACK		= 0x00,
	ATTR_BACKBLUE 		= 0x10,
	ATTR_BACKGREEN 	= 0x20,
	ATTR_BACKCYAN 		= 0x30,
	ATTR_BACKRED 		= 0x40,
	ATTR_BACKMAGENTA	= 0x50,
	ATTR_BACKBROWN 	= 0x60,
	ATTR_BACKGREY 		= 0x70,

	ATTR_BACKDKGREY	= 0x80,
	ATTR_BACKLTBLUE 	= 0x90,
	ATTR_BACKLTGREEN 	= 0xA0,
	ATTR_BACKLTCYAN 	= 0xB0,
	ATTR_BACKLTRED 	= 0xC0,
	ATTR_BACKPINK		= 0xD0,
	ATTR_BACKYELLOW 	= 0xE0,
	ATTR_BACKWHITE		= 0xF0
	};

enum
	{
	ATTR_BLACK,
	ATTR_BLUE,
	ATTR_GREEN,
	ATTR_CYAN,
	ATTR_RED,
	ATTR_MAGENTA,
	ATTR_BROWN,
	ATTR_GREY,
	ATTR_DKGREY,
	ATTR_LTBLUE,
	ATTR_LTGREEN,
	ATTR_LTCYAN,
	ATTR_LTRED,
	ATTR_PINK,
	ATTR_YELLOW,
	ATTR_WHITE
	};

//-----------------------------------------------------------------------------

struct _object;

class _layer
{
public:
    _layer *next;						// ptr to next layer in list
    _layer *prev;			   			// ptr to previous layer in list
    int xPos;									// signed x coordinate on screen
    int yPos;									// signed y coordinate on screen
    unsigned int xSize;							// x size of layer
    unsigned int ySize;							// y size of layer
	unsigned int xMin,xMax,yMin,yMax;			// minumim & maximum sizes for window
	unsigned int oldXPos,oldYPos,oldXSize,oldYSize;  	// for zoom & un-zoom operations
	FLAG cursorOn;								// if zero, no cursor in this window
	unsigned int cursorX;						// cursor x position inside layer
	unsigned int cursorY;						// cursor y position inside layer
    void (*activateRoutine)(_object *o);	// called whenever this layer becomes active
    void (*deactivateRoutine)(_object *o);	// called whenever this layer becomes unactive
	_list gadgetHdr;						// header node for list of gadgets attached to this layer
    char border[8];					// window border
    char*title;						// window title
	int windowNum;								// window hot-key number
    unsigned char far *buffer;					// ptr to rectangular buffer containing window image
	unsigned char fillChar;			   			// character to fill windows with
    unsigned char charAttr;						// character attributes
    unsigned int flags;							// see layer.h for flag details
	void *data;									// user defined, with GUI, points to object

	_layer( int xPos,int yPos,int xSize,int ySize,unsigned char charAttr,unsigned char fillChar );
	virtual ~_layer();

	class _layer* ToFront();
	class _layer* ToBack();

	virtual void Render(char* buffer, int xOffset, int yOffset, int screenWidth, int screenHeight);

	void Fill( unsigned char fillChar = ' ' );

protected:
	_layer();
};

class _shadowedLayer : public _layer
{
	void RenderShadow(char* buffer, int xOffset, int yOffset, int screenWidth, int screenHeight);
public:
	virtual void Render(char* buffer, int xOffset, int yOffset, int screenWidth, int screenHeight);
	_shadowedLayer( int xPos,int yPos,int xSize,int ySize,unsigned char charAttr,unsigned char fillChar ) :
	_layer(xPos, yPos, xSize, ySize, charAttr, fillChar ) { }
};

class _layerBase : public _layer
	{
public:
	_layerBase();
	~_layerBase();
	void Display( char far *buffer,int xOffset,int yOffset,char backgroundChar, char backgroundAttr,int screenWidth,int ScreenHeight );
	class _layer* Head() { return(next); }
	class _layer* Tail();
	};

//=============================================================================

extern _layerBase layBase;
extern FLAG refreshAll,refreshEnable;

//=============================================================================

void
DisplayLayers(char far* buffer,int xOffset,int yOffset,char backgroundChar, char backgroundAttr,int screenWidth,int ScreenHeight);

//=============================================================================

void
PrintLayAttrClip( class _layer* lPtr, char* text, int xPos, int yPos, unsigned char charAttr,int xClip );

//=============================================================================

void
PrintLayAttr( class _layer* lPtr, char* text,int xPos, int yPos,unsigned char charAttr);

void
PrintLay( class _layer* lPtr, char* text, int xPos, int yPos);

extern void cdecl
FillRectLayer( class _layer* lPtr, int xStart, int yStart,int xSize,int ySize,unsigned char fillChar,unsigned char charAttr);

//=============================================================================

#endif

//=============================================================================
