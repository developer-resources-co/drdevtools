//=============================================================================
// guifunc.h
//=============================================================================

// defines
#define MAXMAPS 10
#define MAXMAPX 410
#define MAXMAPY 26

#define MAXSELECTX MAXMAPX
#define MAXSELECTY MAXMAPY
#define DIMGRAY FindColor(20,20,20)
#define MEDGRAY FindColor(44,44,44)

#if defined(SNES)
#define MAXCHARS 1024
#define CHARMASK   0x03FF
#define HFLIPMASK  0x4000
#define VFLIPMASK  0x8000
#define PRIBITMASK 0x2000

#define PALBIT     10
#define PALMASK    (7 << 10L)
#define PAL0MASK   (1 << 10L)
#define PAL1MASK	 PAL0MASK
#define PAL2MASK   PAL0MASK
#define PAL3MASK   PAL0MASK
#endif

#if defined(GENESIS)
#define MAXCHARS 2048
#define CHARMASK   0x07FF
#define HFLIPMASK  0x0800
#define VFLIPMASK  0x1000
#define PRIBITMASK 0x8000

#define PALBIT     13
#define PALMASK    0x6000   // AND Mask for Pal 0
#define PAL0MASK   0x9fff   // AND Mask for Pal 0
#define PAL1MASK   0x2000   // OR mask
#define PAL2MASK   0x4000   // OR mask
#define PAL3MASK   0x6000   // OR mask
#endif

//=============================================================================

typedef unsigned short TILE;

typedef struct _palette
	{
	uint nOffset;
	uint nColors;
   uint palbuffer[256];				// Max num colors; malloc later... ---
	char *szFilename;
	} PALETTE;

typedef struct _tileset
	{
	uint chars;
	uchar huge *chardata;
	char *szFilename;
	PALETTE palette;
	} TILESET;

typedef struct _map
	{
	uint xSize,ySize;
	uint xOffset,yOffset;			// kts 3/2/92
	uint far *buffer;
	TILESET *tileset;
	uint usage[MAXCHARS];
	char *szFilename;
	FLAG fSpare;
	} MAP;

#if 0
struct _selectedchars
{
    uint xSize;
    uint ySize;
    uint *buffer;
};
#endif

//=============================================================================

// externs
extern int chars;
extern uchar huge *chardata;
extern int charpaloff;
extern int num_of_maps, num_of_chars, num_of_pals;
extern uint offsetbufferx[MAXMAPS];
extern uint offsetbuffery[MAXMAPS];
extern char palfilename[MAXMAPS][20];
extern uint defaultXSize,defaultYSize;
extern MAP selected;


enum
{
	DSP_NORMAL,
	DSP_PRI,
	DSP_COLOR,
	DSP_USAGE,
	DSP_MAX
};


// switches/user parameters
extern int currentmap;
extern MAP maps[MAXMAPS];
extern TILESET tilesets[MAXMAPS];
extern PALETTE palettes[MAXMAPS];
extern FLAG refreshOn;
extern FLAG clipSave;
extern FLAG multiLayer;
extern FLAG xparentOn;
extern int zoomFactor;
extern int displayMode;

extern MAP maps[MAXMAPS];
extern MAP *mapPtr;
extern uint mapwidth,mapheight;
extern int selectwidth,selectheight;

//=============================================================================
// prototypes/routines

void DosShell( void );
int KeyNextDspMode(int x,int y,struct _keygad *gadget);
int KeyZoomOut (int x,int y,struct _keygad *gadget);
int KeyZoomIn (int x,int y,struct _keygad *gadget);
int KeyToggleStatusBar (int x,int y,struct _keygad *gadget);
int KeyToggleFullScreen (int x,int y,struct _keygad *gadget);
int KeyToggleMulti (int x,int y,struct _keygad *gadget);
int KeyMapBrushPal (int x,int y,struct _keygad *gadget);
int KeyMapBrushKill (int x,int y,struct _gad *gadget);
int KeyMapFlipBrushX (int x,int y,struct _gad *gadget);
int KeyMapFlipBrushY (int x,int y,struct _gad *gadget);
int KeyMapLeftEdgeHit (int x,int y,struct _keygad *gadget);
int KeyMapRightEdgeHit (int x,int y,struct _keygad *gadget);
int KeyMapTopEdgeHit (int x,int y,struct _keygad *gadget);
int KeyMapBotEdgeHit (int x,int y,struct _keygad *gadget);
int DrawMapPri      (int x,int y,struct _gad *gadget);
int DrawMapColor    (int x,int y,struct _gad *gadget);
int TextBox         (struct _gad *gadget);
int QuitHit         (int x,int y,struct _gad *gadget);
int RTHit           (int x,int y,struct _gad *gadget);
int XParentOn       (int x,int y,struct _gad *gadget);
int XParentOff      (int x,int y,struct _gad *gadget);
int UndoHit         (int x,int y,struct _gad *gadget);
int SwapHit         (int x,int y,struct _gad *gadget);
int SaveHit         (int x,int y,struct _gad *gadget);
int CharUpDownHit   (int x,int y,struct _gad *gadget);
int MapBox          (int x,int y,struct _gad *gadget);
int SelectKB        (int x,int y,struct _gad *gadget);
int ShowAbout       (int x,int y,struct _gad *gadget);
int ShowHelp       (int x,int y,struct _gad *gadget);
int ShowInfo        (int x,int y,struct _gad *gadget);
int CharBox         (int x,int y,struct _gad *gadget);
void SetGlobals     (void);
void CopyUndoBuffer (void);
void DrawSelectCommands (void);
int MapUpDownHit    (int x,int y,struct _gad *gadget);
int KeyMapUpDownHit    (int x,int y,struct _keygad *gadget);
int MapLeftRightHit (int x,int y,struct _gad *gadget);
int Paste (uint huge *sourcebuffer,uint huge *destbuffer,uint width,uint height,int sourcewidth,int destwidth);
int Clear (uint *destbuffer,uint width,uint height,int destwidth);
int SelectPiece (int x,int y,struct _gad *gadget);
int SelectMapBox (int x,int y,struct _gad *gadget);

//=============================================================================

