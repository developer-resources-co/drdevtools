
#include "gentypes.h"

#ifndef _VDP_H_
#define _VDP_H_

/*--------------------------------------------------------------------------*/
#ifndef FIELD_A
#define FIELD_A 0
#define FIELD_B 2
#endif
/*--------------------------------------------------------------------------*/

void InitVDP(char *);

extern char VDPTable[19];
extern char VDP256Table[19];

extern const TCharSetData oBlankCharset;

void  VDPAddress(short vdpAddr);

void SetPalette(short base, short count, void *palette);

void GetPalette(short base, short count, void *palette);

/*--------------------------------------------------------------------------
 *
 * PlotSubMap will plot a rectangular region bounded by x1,y1,x2,y2 of
 * a map at position x,y, correctly wrapping if the screen happens to be scrolled
 * to the Field boundary.  Takes a Field pointer to determine which playfield
 * to plot to.
 *
 --------------------------------------------------------------------------*/

void PlotSubMap(short x, short y,short x1,short y1,short x2,short y2,short attrib,TPlayField *,TMapData *);

/*--------------------------------------------------------------------------*/

void PlotMapA(short x, short y, long base, void *map);
void PlotWrapA(short x, short y, short base, void *map);
void PlotWrapB(short x, short y, short base, void *map);
short DMACopy(short base, short count, void *data);
void WaitBlank(void);
void SetBackColor(short c);
extern void *DeferedDMACopy(unsigned short wVDPAddr, unsigned short nWords,
	void *pDMAData, void* pBuffer);
extern void ExecuteDeferedDMA(void *pDeferDMAArray,unsigned short nDMACount);
extern void ClearFieldA (void);

short LoadCharsAtAddress( const TCharSetData* const p, unsigned short VDPCharPointer );
short LoadChars( const TCharSetData* const charset );

void InitFieldStruct(TPlayField *,unsigned short);

void PlotMapField (short nXPosition, short nYPosition, long iCharOffset,
	void *pFieldStructure, void *pMap);
void PlotMapWindow (short nXPosition, short nYPosition, long iCharOffset,
	void *pMap);
void PlotMapB (short nXPosition, short nYPosition, long iCharOffset,
	void *pMap);
void PlotMapXFlipA (short nXPosition, short nYPosition, long iCharOffset,
	void *pMap);
void PlotMapYFlipA (short nXPosition, short nYPosition, long iCharOffset,
	void *pMap);
void PlotMapXYFlipA (short nXPosition, short nYPosition, long iCharOffset,
	void *pMap);
void PlotMapXFlipB (short nXPosition, short nYPosition, long iCharOffset,
	void *pMap);
void PlotMapYFlipB (short nXPosition, short nYPosition, long iCharOffset,
	void *pMap);
void PlotMapXYFlipB (short nXPosition, short nYPosition, long iCharOffset,
	void *pMap);
void ClearFieldB (void);

#endif

