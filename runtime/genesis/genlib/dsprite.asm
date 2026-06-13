;==============================================================================
;===	dsprite.asm: Genesis dynamic sprite library routines  	     	    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================
;	This library was just pulled from a game,
;	it has not been tested stand alone(although the game is using it
;	from this file now), documetation is forthcomming.

;==============================================================================

Code	Segment

;==============================================================================
;	Inputs:
;		d0.w = starting address in vdp sprite library can use
;		d1.w = # of characters in vdp sprite library can use
;
InitDynamicSprites:
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
	rts

;==============================================================================
; make current sprite list permanent
;------------------------------------------------------------------------------

LockDynamicSprites:
	move.w	dsSpriteCurrentVDPAddr,dsSpriteVDPAddr
					; copy sprite ptr tbl to perm table
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
;		d3 = sprite #
;	Outputs:
;		Z = out of sprites, NZ = ok
;	Destroys:
;		d0.w,d1.l-d2,d3.w,d7.w,a0.l-a2,a3.w,a4.l-a6
;	Internal:
;		a4-> spentry
;------------------------------------------------------------------------------

AddDynamicSprite:
	lea	dynamicSpriteBase,a4
	clr.l	d7
	move.w	d3,d7
	add.l	d7,d7
	add.l	d7,d7				; * 4
	add.l	d7,d7
	add.l	d7,d7				; * 16
	add.l	d7,a4

	cmp.w	#VDP_TOPFREE,dsSpriteCurrentVDPAddr
	bcs.s	.Ok
	ifdef	DEBUG					; debugging stuff
.Die:
	COLORCYCLE
	bra	.Die
	endif
	ifndef	DEBUG
	rts						; oh well, won't fit, fuck it
	endif
.Ok:
	move.l	(a4)+,a2					; read ptr to logical sprite struct
	push.w	d0-d2					; d3 still contains sprite #
	bsr	FindDynamicSprite
	pop.w	d0-d2
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
	lea	dsSprTbl,a1
	move.w	d3,d2
	add.w	d2,d2					; * 2
	move.w	(a1,d2.w),d3
 	bne.s	.Found

	move.l	(a4)+,a0				; ROM address
	move.w	(a4),d1					; get size
	beq.s	.Found					; if charsize is zero, do not need to download
	move.w	dsSpriteCurrentVDPAddr,d3
	move.w	d3,(a1,d2.w)				; store address of this sprite in table
	move.w	d3,d2					; remember address
	add.w	d1,d2					; add size to current char ptr
	add.w	d1,d2					; add twice since given in words
	move.w	d2,dsSpriteCurrentVDPAddr

	move.w	d3,d0
	move.l	dsDlTblPtr,a6				; queue to be downloaded
	jsr	FakeDMACopy
	move.l	a6,dsDlTblPtr
	addq.w	#1,dsCount
.Found:
	rts

;==============================================================================

DownloadDynamicSprites:
	lea	dsChrDlTbl,a4
	lea	VDP_CMD,a6
	move.w	dsCount,d4
	dbra	d4,.Loop
	rts
.Loop:
	move.l	(a4)+,(a6)
	move.l	(a4)+,(a6)
	move.w	(a4)+,(a6)
	move.l	(a4)+,(a6)	    	; do the dma
	dbra	d4,.Loop
	rts

;==============================================================================

RAM	segment
	even
dsChrDlTbl:				; table of chr's to dma(download) for dynamic sprites
	ds.l	MAXSPRITES*14		; fake dma size * maxsprites
dsDlTblPtr:			; current dsChrDlTbl ptr
	ds.l	1
dsCount:                       ; # of entries in dsChrDlTbl
	ds.w	1
dsSpriteCurrentVDPAddr:			; next available char address  for sprite use
	ds.w	1
dsSprTbl:
	ds.l	SPRITECOUNT     	; current downloaded sprite table
dsPermSprTbl:
	ds.l	SPRITECOUNT		; record of which sprites are permanently downloaded

dsSpriteVDPAddr:		; ptr to 1st char in VDP dynamic sprites can use
	ds.l	1

;==============================================================================

	END

;==============================================================================
