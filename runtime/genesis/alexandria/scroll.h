
#include "gentypes.h"

#ifndef _SCROLL_H_
#define _SCROLL_H_

#ifndef SCROLL_FieldA
#define SCROLL_FieldA 0
#define SCROLL_FieldB 2
#define SCROLL_HScroll 0
#define SCROLL_VScroll 1
#endif

void SetVScroll(short scroll,short field);
void SetHScroll(short scroll,short filed);

void InitMetaScroll(short xCoordinate,
short yCoordinate,
short nFirstChar,
short nField,
TScrollField *pScrollStruct,
const TMetaCharMap *pMap,
const TMetaMap *pMetaMap,
const TXlateTable *Xlation);

void ScrollMetaUp(TScrollField *pScrollStruct,unsigned short );
void ScrollMetaDown(TScrollField *pScrollStruct, unsigned short );
void ScrollMetaLeft(TScrollField *pScrollStruct, unsigned short );
void ScrollMetaRight(TScrollField *pScrollStruct, unsigned short );
void SetScrollRegs(void);
short GetScrollRegs(short regNum);

struct SCROLL
	{
	TScrollField scrollStruct;
	short nLeftEdgeOfMapVisibleOnScreen, nTopEdgeOfMapVisibleOnScreen;
	TMapData* pMap;
	unsigned short wCharBase;
	short chMapWidth;
	short pxMapWidth;
	short chMapHeight;
	short pxMapHeight;
	short nRightLimit;
	short nBottomLimit;
	void (*fnScreenAddress)();
	short nPreviousDirection;			/* 0==left, -1==right */
	/* Metamap stuff below here */
	long l[2];
	short w[6];
	};

/* Two-way scrolling */
void InitHorizScroll
	(short xOffset, short yOffset, short nCharBase, short nField,
	 struct SCROLL*, TMapData* );
void RedrawHorizScroll( struct SCROLL* );
void ScrollHorizLeft( struct SCROLL* );
void ScrollHorizRight( struct SCROLL* );

#endif
