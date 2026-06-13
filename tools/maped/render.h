//=============================================================================
// render.h: rendering funcitons declarations
//=============================================================================


//=============================================================================
// variables

extern int charoffset;
extern signed int mapXOffset;
extern signed int mapYOffset;

//=============================================================================
// routines

void ShowBrush (int x,int y, struct _gad *gadget);
void DrawMapBox(struct _gad *gadget);
void DrawCharBox (struct _gad *gadget);
void XORChars (int x,int y,int width,int height);
void XORChars2 (int x,int y,int width,int height);
void XORBOX          (int x,int y,int width,int height);
void ShowBrush       (int x,int y,struct _gad *gadget);

//=============================================================================
