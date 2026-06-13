;==============================================================================
; msprite.asm: kts port of Scott's msprite.c
;==============================================================================

	opt	llbl
	include 'genesis.equ'
	include 'sprite.equ'

	TSECTION        .text           ;e changed from code segment
	XDEF	_MakeMapSpriteChars

;==============================================================================
; orignal c code:
;
;#define CharWithinMap( m, x, y ) ( (m->wMap)[x+y*m->xSize])
;
;static char *
;MakeMapSpriteChars(TMapData *mapPtr, char * c, char * obuff, TLogicalSpriteData *ls )
;{
;	short	count;					/* # of hardware sprites left to map */
;	short xCount,yCountStore,charNum,yHold;
;	register short  yCount;
;	register short	x,y;
;	THardwareSprite *hsp;
;
;	hsp = ls->oHardSprite;
;
;	count = ls->wCount;
;	while( count--)
;	 {
;		x = hsp->nHPos/8;
;		yStore = hsp->nVPos/8;
;
;		xCount = xSize[hsp->bSize];
;		yCountStore = ySize[hsp->bSize];
;
;		while(xCount--)
;		 {
;			y = yStore;
;			yCount = yCountStore;
;			while(yCount--)
;			 {
;				if ( charNum = CharWithinMap(mapPtr,x,y++) )
;					CopyForward(obuff,c,charNum);
;			  	else
;					ClearSpace(obuff);
;			 	obuff += 32;
;			 }
;			x++;
;		 }
;		hsp++;				/* point to next hardware sprite */
;	 }
;	return obuff;
;}
;------------------------------------------------------------------------------
; register allocation:
;
;	d0 = xCount
;	d1 = yCount
;	d2 = count		; hardware sprite count
;*	d3 = charNum/scratch
;	d4 = x
;	d5 = pre-multiplied y offset
;*	d6 = scratch
;	d7 = yCountStore
;	a0-> hsp
;	a1-> mapPtr
;	a2-> charset
;	a3-> obuff
;*	a4-> ls(then scratch)
;*	a5 = scratch
;*	a6 = scratch
;------------------------------------------------------------------------------
; data on stack
; 	offset	data
;	6	TMapData *mapPtr
;       10	char *charset
;	14	char *obuff
;	18	TLogicalSpriteData *ls
;
;------------------------------------------------------------------------------
; pos d0,d2,a0

MMSCxSize:
	dc.w	1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4
MMSCySize:
	dc.w	1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4

;------------------------------------------------------------------------------

_MakeMapSpriteChars:
.FRAME_MAPPTR 	equ	8
.FRAME_CHARSET	equ	12
.FRAME_OBUFF	equ	16
.FRAME_LS	equ	20
	link	a6,#0
	movem.l	d2-d7/a2-a5,-(sp)

	move.l	.FRAME_MAPPTR(a6),a1			; load variables from stack
	move.l	.FRAME_CHARSET(a6),a2
	move.l	.FRAME_OBUFF(a6),a3
	move.l	.FRAME_LS(a6),a4

	lea	lSprite_Size(a4),a0			; get ptr to 1st hardware sprite struct(hsp = ls->oHardSprite; )
	move.w	lSpriteCount(a4),-(sp)			; count = ls->wCount;

.Loop:							! while( count--)
	tst.w	(sp)
	beq	.Done
	subq.w	#1,(sp)

	clr.w	d6					; xCount = xSize[hsp->bSize];
	move.b	hSpriteSize(a0),d6
	add.w	d6,d6
	move.w	MMSCxSize(pc,d6.w),d0
	move.w	MMSCySize(pc,d6),d7

	move.w	hSpriteHPos(a0),d3			; x = hsp->nHPos/8;
	asr.w	#3,d3
	move.w	hSpriteVPos(a0),d6			; yStore = hsp->nVPos/8;
	asr.w	#3,d6

	move.w	(a1),d4				; pre-calc start position
	move.w	d4,d5
	mulu.w	d6,d5
	add.w	d3,d5					; pre add x offset

.xLoop:							! while(xCount--)
	tst.w	d0
	beq.s	.xDone
	subq.w	#1,d0
	move.w	d5,d2

	move.w	d7,d1					; yCount = yCountStore
.yLoop:							! while(yCount--)
	tst.w	d1
	beq.s	.yDone
	subq.w	#1,d1
;				if ( charNum = (mapPtr->wMap)[x+y*mapPtr->xSize]) )
;					CopyForward(obuff,c,charNum);
;			  	else
;					ClearSpace(obuff);
;				y++;
	move.w	d2,d6
	add.w	d4,d2			; move down one row in map

	add.w	d6,d6			; *2 = word size
	move.w	4(a1,d6.w),d6		; skip sizes, and offset to correct char
;;	tst.w	d6
	bne.s	.Copy

	clr.l	d6			; clear space
	move.l	d6,(a3)+
	move.l	d6,(a3)+
	move.l	d6,(a3)+
	move.l	d6,(a3)+
	move.l	d6,(a3)+
	move.l	d6,(a3)+
	move.l	d6,(a3)+
	move.l	d6,(a3)+
	bra.s	.yLoop			; actually, .Ok

.Copy:
        ext.l   d6                      ; make d6 long to allow >64k of data
	asl.l	#5,d6			; * 32
	lea	(a2,d6.l),a5

;	movem.l	(a5),d3/d6/a4/a6	; 12 + (4*8) = 44 / 2
;	movem.l	d3/d6/a4/a6,(a3)	;  8 + (4*8) = 40 / 2
;	movem.l	16(a5),d3/d6/a4/a6	; 16 + (4*8) = 48 / 3
;	movem.l	d3/d6/a4/a6,16(a3)	; 12 + (4*8) = 44 / 3
;	lea	32(a3),a3		;  		8 / 3
;						      184 / 13

	move.l	(a5)+,(a3)+		; 12/1
	move.l	(a5)+,(a3)+		;
	move.l	(a5)+,(a3)+
	move.l	(a5)+,(a3)+
	move.l	(a5)+,(a3)+
	move.l	(a5)+,(a3)+
	move.l	(a5)+,(a3)+
	move.l	(a5)+,(a3)+
;					 96 / 8 ... Uses 88 few cycles, 10 fewer bytes
.Ok:
	bra.s	.yLoop
.yDone:
	addq.w	#1,d5				; x++
	bra.s	.xLoop
.xDone:
	add.w	#hSprite_Size,a0		; hsp++; /* point to next hardware sprite */
	bra	.Loop
.Done:
	addq.w	#2,sp				; skip d2 on stack
	move.l	a3,a0				; return obuff;
	movem.l	(sp)+,d2-d7/a2-a5/a6		; a6 happens to be at end, so this works
	rts

;==============================================================================

	end

;==============================================================================









