;==============================================================================
;===	dsprite.asm: Genesis dynamic sprite library routines  	     	    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================
;   How It Works ....
;   Or, what you always wanted to know about Kevin's programming style,
;   but were afraid to ask.
;
;   The Dynamic sprite library uses a pair of arrays that correspond to each
;   other to keep track of which LSprites have been loaded in a given frame.
;
;   All DLSprites that ever will exist in the game are kept in an array in ROM.
;   The structure of this array is as follows
;	struct DynamicSprite {
;		_lSpriteData	*spr;
;		_CharSetData	*chr;
;		} dynamicSpriteArray[MAXSPRITES];
;   On entry to AddDynamicSprite, you indicate which sprite you are adding
;   by it's position in this table.  An array in RAM keeps track of which
;   of these DynamicSprites are actually loaded, by storing either a zero
;   for unloaded sprites, or the 1stChar number for loaded sprites.
;   AddDynamicSprite calls FindDynamicSprite which given the sprite serial
;   number, indexes into this RAM array.  If the characters are already
;   downloaded, it returns.  Otherwise, it queues a DeferedDMA to load the
;   appropriate characters.  In either case, it returns the 1stChar number
;   of that LSprite, which will be used by AddLSpriteBase.

;   I will be changing this routine, so that the CharData pointer is not kept
;   in the ROM array, because the Mapped Sprite routine will be creating those
;   on the fly.

;==============================================================================
	opt llbl
	include	'genesis.equ'
	include 'default.equ'

	XREF	InitSprites
	XREF	spriteVDPBase
	XREF	ClearSpriteTable
	XREF	AddLSpriteBase
	XREF	DeferedDMACopy

	XDEF	AddDynamicSprite
	XDEF	DownloadDynamicSprites
	XDEF	ClearDynamicSpriteTable
	XDEF	InitDynamicSprites

	TSECTION	.text

;==============================================================================
;	Inputs:
;		d0.w = starting address in vdp sprite library can use
;
	xdef	_InitDynamicSprites
_InitDynamicSprites:
	move.w	6(sp),d0
InitDynamicSprites:
	move.w	d7,-(sp)
	jsr	InitSprites
	move.w	spriteVDPBase,d0
	lsl.w	#5,d0			; *32(CHARSIZE)
	move.w	d0,dsSpriteVDPAddr
	lea	dsPermSprTbl,a0
	clr.l	d0
	move.w	#SPRITECOUNT-1,d7
.Loop:
	move.l	d0,(a0)+
	dbra	d7,.Loop
	bsr.s	ClearDynamicSpriteTable
	move.w	(sp)+,d7
	rts

;==============================================================================
; make current sprite list permanent
;------------------------------------------------------------------------------
	xdef	_LockDynamicSprites
_LockDynamicSprites:
	link	a6,#-36
	movem.l	d0-d6/a2,-32(a6)
	bsr	LockDynamicSprites
	movem.l	-32(a6),d0-d6/a2
	rts

LockDynamicSprites:
	move.w	dsSpriteCurrentVDPAddr,dsSpriteVDPAddr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; copy sprite ptr tbl to perm table
	lea	dsPermSprTbl,a1
	lea	dsSprTbl,a0
	move.w	#(SPRITECOUNT/8)-1,d7
.Loop:
	movem.l	(a0)+,d0-d6/a2			; copy 8 longs at once
	movem.l	d0-d6/a2,(a1)
	lea	4*8(a1),a1
	dbra	d7,.Loop
	rts

;==============================================================================
	xdef	_ClearDynamicSpriteTable
_ClearDynamicSpriteTable:
	link	a6,#-40
	movem.l	d0-d7/a2,-36(a6)
	bsr	ClearDynamicSpriteTable
	movem.l	-36(a6),d0-d7/a2
	rts

ClearDynamicSpriteTable:
	move.l	#dsChrDlTbl,dsDlTblPtr
	clr.w	dsCount
	move.w	dsSpriteVDPAddr,dsSpriteCurrentVDPAddr

; copy perm tbl to temp tbl
	lea	dsPermSprTbl,a0
	lea	dsSprTbl,a1
	move.w	#(SPRITECOUNT/8)-1,d7
.Loop:
	movem.l	(a0)+,d0-d6/a2			; copy 8 longs at once
	movem.l	d0-d6/a2,(a1)
	lea	4*8(a1),a1
	dbra	d7,.Loop
	jsr	ClearSpriteTable
	rts

;============================================================================
;	AddDynamicSprite: Add logical sprite to sprite list
;	Inputs:
;		d0.w = sprite x pos( 0 = left edge)
;		d1.w = sprite y pos( 0 = top edge)
;		d2 = sprite descriptor(use hardware equates)
;		d3 = spite # (index into dynamicSpriteArray)
;	Outputs:
;		Z = out of sprites, NZ = ok
;	Destroys:
;		d0.w,d1.l-d2,d3.w,d7.w,a0.l-a2,a3.w,a4.l-a6
;	Internal:
;		a4-> spentry
;------------------------------------------------------------------------------


	xdef	_AddDynamicSprite
_AddDynamicSprite:
	link	a6,#-40
	movem.l	d3-d7/a2-a5,-36(a6)
	move.w	10(a6),d0
	move.w	14(a6),d1
	move.w	18(a6),d2
	move.w	22(a6),d3
	jsr	AddDynamicSprite
	bne.s	.1
	clr.l	d0
	bra.s	.2
.1
	moveq	#1,d0
.2
	movem.l	-36(a6),d3-d7/a2-a5
	unlk	a6
	rts

	xref	_dynamicSpriteArray
AddDynamicSprite:
	lea	_dynamicSpriteArray,a4
	clr.l	d7
	move.w	d3,d7
	add.l	d7,d7
	add.l	d7,d7				; * 4
	add.l	d7,d7				; * 8
	add.l	d7,a4

	cmp.w	#VDP_TOPFREE,dsSpriteCurrentVDPAddr
	bcs.s	.Ok
	rts					; oh well, won't fit, fuck it
.Ok:
	move.l	(a4)+,a2			; read ptr to lSprite struct
; save d0-d2 for call to AddLSprite
	movem.w	d0-d2,-(sp)				; d3 still contains sprite #
	bsr	FindDynamicSprite
	movem.w	(sp)+,d0-d2
	move.l	a2,a0

	lsr.w	#5,d3						; /32(CHARSIZE)
	move.w	d3,a3
	jsr	AddLSpriteBase		; put it into sprite table
	rts

;==============================================================================
; Inputs:
;	d3 = sprite to check
;	a4-> rom address of sprite to add(if needed)
; Outputs:
;	d3 = base of sprite(will add to list if needed)
; Destroys:
;==============================================================================
FindDynamicSprite:
	lea	dsSprTbl,a1	; table of downloaded sprites
	move.w	d3,d2
	move.w	d3,d0		; save this for CreateCharset
	add.w	d2,d2		; each entry is a word
	move.w	(a1,d2.w),d3	; first char # of sprite #d3
 	bne.s	.Found

;	movea.l	(a4)+,a0		; pointer to charSetData
;	move.l	(a0)+,d1			; get size

	bsr	CreateCharset	; for sprite #d0, return charset in A0
;				; and size in d1

	move.w	dsSpriteCurrentVDPAddr,d3 ; next spot in VRAM
	move.w	d3,(a1,d2.w)		; store address of this sprite in table
	move.w	d3,d2
	add.w	d1,d2			; add size to current char ptr
	add.w	d1,d2			; add twice since given in words
	move.w	d2,dsSpriteCurrentVDPAddr

	move.w	d3,d0			; Vaddr of 1st char (again)
	move.l	dsDlTblPtr,a1		; queue to be downloaded
	jsr	DeferedDMACopy
	move.l	a0,dsDlTblPtr
	addq.w	#1,dsCount
.Found:
	rts
; this version is for the way we do it now...

CreateCharset:
	movea.l	(a4)+,a0
	move.l	(a0)+,d1
	rts

;==============================================================================

	xdef	_DownloadDynamicSprites
_DownloadDynamicSprites:
DownloadDynamicSprites:
	lea	dsChrDlTbl,a0
	lea	VDP_CMD,a1
	move.w	dsCount,d0
	dbra	d0,.Loop
	rts
.Loop:
	move.l	(a0)+,(a1)
	move.l	(a0)+,(a1)
	move.w	(a0)+,(a1)
	move.l	(a0)+,(a1)	    	; do the dma
	dbra	d0,.Loop
	rts

;==============================================================================

	BSECTION	.bss

dsChrDlTbl: ds.l	MAXSPRITES*14	; table of chr's to dma for dynamic sprites
dsDlTblPtr: ds.l	1		; current dsChrDlTbl ptr
dsCount:    ds.w	1            	; # of entries in dsChrDlTbl
dsSpriteCurrentVDPAddr:	ds.w	1	; next available char address  for sprite use
dsSprTbl:   ds.l	SPRITECOUNT     ; current downloaded sprite table
dsPermSprTbl: ds.l	SPRITECOUNT	; record of which sprites are permanently downloaded
dsSpriteVDPAddr: ds.l 1		; ptr to 1st char in VDP dynamic sprites can use

;==============================================================================

	END

;==============================================================================
