//==============================================================================
// sgrPort.cpp:
//==============================================================================

#ifdef __WATCOMC__
#include <graph.h>
#endif

struct hostGfxMode
{
	uint xSize,ySize;
	uint numColors;
};

struct ibmGfxMode
{
	// general stuff
	uint xSize,ySize;
	uint numColors;

	// ibm specific stuff
	uint mode;
	uint baseSeg;
};

//==============================================================================

ibmGfxMode ibmGfxModes[] =
{
	{ 320,200,256,0x13,0xa000 },
	{ 640,200,16,0xe,0xa000 },
	{ 640,200,16,0xf,0xa000 },
	{ 640,200,16,0x10,0xa000 }
};

//------------------------------------------------------------------------------

const uint ibmModeCount = sizeof(ibmGfxModes)/sizeof(ibmGfxModes[0]);

//==============================================================================

uint
FindGfxMode(uint xSize, uint ySize, uint numColors)
{
	for(uint i=0;i<ibmModeCount; i++)
	 {
		if(ibmGfxModes[i].xSize >= xSize &&
		   ibmGfxModes[i].ySize >= ySize &&
		   ibmGfxModes[i].numColors >= numColors)
				return(ibmGfxModes[i].mode);
	 }
	// wish I had throw
	Error(ERROR_GFXMODE_UNAVAILABLE);
	return(0);
}

//==============================================================================

ubyte*
GetIBMScreenAddr(uint gfxMode)
{
	ubyte*addr;

	for ( int i=0; i<ibmModeCount && ibmGfxModes[i].mode != gfxMode; ++i )
		;

	if(ibmGfxModes[i].mode != gfxMode)
		Error(ERROR_UNDEFINED,"Gfx Mode not found in GetVGAScreenAddr");
		// wish I had throw

#ifdef DOSX286
	unsigned short sel;

	DosMapRealSeg( ibmGfxModes[i].baseSeg, 64000, &sel );
	addr = (ubyte*)MAKEP( sel, 0 );
#else
	addr = (ubyte*)(((ulong)ibmGfxModes[i].baseSeg) << 16);
#endif

	return(addr);
}

//==============================================================================

screenGraphPort::screenGraphPort(uint xSize, uint ySize, uint numColors, const color& c)
: rastGraphPort(NULL,xSize,ySize,c)
{
	width = xSize;
	height = ySize;
	uint gfxMode;

	// get desired gfx mode and base address
	gfxMode = FindGfxMode(xSize,ySize,numColors);
	lineArrayPtr[0] = GetIBMScreenAddr(gfxMode);
	for ( uint i=1; i<ySize; ++i )
		lineArrayPtr[ i ] = lineArrayPtr[ i-1 ] + xSize;

	// first, remember old gfx mode
#ifdef 	__BORLANDC__
	gettextinfo(&ti);

	// actually change gfx modes
    asm{
        MOV AX, gfxMode
        SUB AH, AH
        INT 10H							// set graphics mode
    }
#endif

#ifdef __WATCOMC__
	_setvideomode(gfxMode);
#endif

#if 0
	// now fill screen with given color
	for(uint i=0;i<width*height;i++)
		mapPtr[i] = currentIndex;
#endif

	ForcePaletteUpdate();
}

//==============================================================================

screenGraphPort::~screenGraphPort()
{
#ifdef __BORLANDC__
	textmode(ti.currmode);			// restore original gfx mode
#endif
#ifdef __WATCOMC__
	_setvideomode(_DEFAULTMODE);
#endif
}

//==============================================================================

void
screenGraphPort::SetColor(const color& c)
{
	rastGraphPort::SetColor(c);
	PaletteUpdate();
}

//==============================================================================

/*
void
screenGraphPort::SetIBMPalette(colorMapIndex current,const color& c)
{
	ubyte r,g,b;
	uword entry = current;

	r = c.GetRed() >> 2;
	g = c.GetGreen() >> 2;
	b = c.GetBlue() >> 2;

    asm{
         mov dx,3dah
        }
    asm{
        MOV DH, r
        MOV CH, g
        MOV CL, b

        PUSH    ES
        MOV AX, 1010H
        MOV BX, entry
        INT 10H
        POP ES
    }

*/
//=============================================================================

void
screenGraphPort::SetIBMPalette( int start, int count, RGBcolor* rgb )
{
#ifdef __BORLANDC__
    asm{
        PUSH    ES
        MOV AX, 1012H
        MOV BX, start
        MOV CX, count
        LES DX, rgb
        INT 10H
        POP ES
    }
#endif

#ifdef __WATCOMC__

	long pal;
	while(start < count)
	 {
		pal = ((long)rgb[start].r) << 8;
		pal |= ((long)rgb[start].g) << 4;
		pal |= ((long)rgb[start].b);
		_remappalette(start++,pal);
	 }
#endif
}

//==============================================================================

void
screenGraphPort::PaletteUpdate(void)
{
	if(paletteVersion != mapCols->GetVersion())
		ForcePaletteUpdate();
}

//==============================================================================

void
screenGraphPort::ForcePaletteUpdate(void)
{
	RGBcolor pal[COLORMAPINDEX_COUNT];
	color c;
	paletteVersion = mapCols->GetVersion();

	for(int i = 0; i < mapCols->GetColorCount(); i++)
	 {
		c = mapCols->GetColor(i);
		pal[i].r = c.GetRed() >> 2;
		pal[i].g = c.GetGreen() >> 2;
		pal[i].b = c.GetBlue() >> 2;
	 }
	SetIBMPalette(0,mapCols->GetColorCount(),pal);
}

//==============================================================================
