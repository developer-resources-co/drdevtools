//=============================================================================
// render.h: rendering funcitons declarations
//=============================================================================


//=============================================================================
// variables

extern int charoffset;

//=============================================================================
// routines

void RenderMap(MAP *mapPtr,int plotType, int xScrOffset, int yScrOffset, int xMapOffset, int yMapOffset, int displayMode );
void ShowBrush (int x,int y);
void DrawMapBox(void );
void DrawCharBox (struct _gad *gadget);
void XORChars (int x,int y,int width,int height);
void XORChars2 (int x,int y,int width,int height);
void XORBOX          (int x,int y,int width,int height);

int DrawMapPri      (int x,int y);
int DrawMapColor    (int x,int y);
int DrawMapUsage( int x,int y );

enum
{
	DSP_NORMAL,
	DSP_PRI,
	DSP_COLOR,
	DSP_USAGE,
	DSP_MAX
};

//=============================================================================

