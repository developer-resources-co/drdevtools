;==============================================================================
;===	sprite.asm: Genesis sprite library routines  	     		    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================
;	Purpose: provide an interface to the Genesis sprite hardware
;	Use:
;		First init the library by calling InitSprites
;		Download all of the characters used by the sprites
;			into the VDP
;		Simple hardware sprites can be created
;			by calling AddSprite(we don't recommend this method
;			of creating sprites)
;
;		Logical sprites of any size may be created by calling
;			AddLSprite with a pointer to the lsprite structure
;			created with the sprite tool or the sprite animator.
;
;		If all of your sprite frames do not fit in the VDP at
;			once, use the dynamic sprite library.
;			(see dsprite.asm)
;------------------------------------------------------------------------------
;	Flow:
;		Once:
;			InitSprites
;			Download sprite chars
;		Per Frame:
;			ClearSpriteTable
;			AddSprite or AddLSprite as many times as needed
;		During VBlank:
;			CopySprites
;------------------------------------------------------------------------------
;	(see world.asm or game.asm for an example of how to use this library)

;============================================================================
; hardware sprite format

	opt llbl

	include 'genesis.equ'
	include 'default.equ'
	IFNE SEGA_CD
	include	cd.inc
	include	cd.mac
	ENDC
	include 'genesis.mac'

	XDEF	InitSprites
	XDEF	ClearSpriteTable
	XDEF	CopySprites
	XDEF	NoSprites
	XDEF	AddLSprite
	XDEF	AddLSpriteBase

	include sprite.equ

;============================================================================

	TSECTION	.text

	XREF	VDPAddress

;============================================================================
;	Inputs:
;		d0.w = address in vdp(in chars) that sprite char file is
;			loaded
;	Destroys:
;		none
;============================================================================

	xdef	_InitSprites
_InitSprites:
	move.w	6(sp),d0

InitSprites:
	move.w	d0,spriteVDPBase
;	bsr.s	ClearSpriteTable
;	rts				; just fall through

;============================================================================
; 	Destroys:
;		none
;------------------------------------------------------------------------------

	xdef	_ClearSpriteTable
_ClearSpriteTable:
ClearSpriteTable:
	clr.b	spriteAttrBuffer+hSpriteLink
	move.l	#spriteAttrBuffer,spriteAttrPtr
	sf	spriteNum
	rts

;============================================================================
;	AddLSprite: Add logical sprite to sprite list
;	Inputs:
;		a0-> logical sprite struct
;		d0.w = sprite x pos( 0 = left edge)
;		d1.w = sprite y pos( 0 = top edge)
;		d2 = sprite descriptor(use hardware equates)
;	(base)	a3.w = sprite vdp address of 1st char
;	Outputs:
;		Z = out of sprites, NZ = ok
;	Destroys:
;		d0.w-d7.w,a0.l-a2.l,a3.w-a4.w
;============================================================================

	xdef	_AddLSprite

_AddLSprite:
	movea.l 4(sp),a0	; retrieve pointer to sprite struct
	move.w	10(sp),d0	; get x
	move.w	14(sp),d1	; get y
	move.w	18(sp),d2	; get sprite descriptor constant
	movem.l	d3-d7/a2-a4,-(sp)
	jsr	AddLSprite
	movem.l	(sp)+,d3-d7/a2-a4
	rts			; erk, save and restore registers?

	xdef	_AddLSpriteBase
_AddLSpriteBase:
	link	a6,#-36
	movem.l	d3-d7/a2-a4,-32(a6)
	movea.l 8(a6),a0	; retrieve pointer to sprite struct
	move.w	14(a6),d0	; get x
	move.w	18(a6),d1	; get y
	move.w	22(a6),d2	; get sprite descriptor constant
	move.w	26(a6),a3	; get sprite Base
	jsr	AddLSpriteBase
	movem.l	-32(a6),d3-d7/a2-a4
	unlk	a6
	rts			; erk, save and restore registers?


AddLSprite:
	move.w	spriteVDPBase,a3
AddLSpriteBase:
	add.w	#$80,d0					; adjust for screen offset
	add.w	#$80,d1
	move.w	lSpriteCount(a0),d7			; get physical(hardware) sprite count
	move.w	lSpriteXSize(a0),d4			; get logical sprite xSize(kts 2/27/92)
	move.w	lSpriteYSize(a0),a4			; get logical sprite ySize(kts 3/1/93)
	lea	lSprite_Size(a0),a0			; skip logical sprite structure
	move.l	spriteAttrPtr,a1

	btst	#SPRITEB_YFLIP,d2
	beq.s	.NotY
	btst	#SPRITEB_XFLIP,d2
	bne	.XYFlippedSprite
	bra	.YFlippedSprite
.NotY:
	btst	#SPRITEB_XFLIP,d2
	bne	.XFlippedSprite
	bra.s	.Entry

;============================================================================
;	Inputs:
;		This routine is for AddLSprite, if you want to manualy
;		add hardware sprites, use AddSprite
;		d0.w = sprite x pos( $80 = left edge)
;		d1.w = sprite y pos( $80 = top edge)
;		d2 = sprite descriptor(use hardware equates)
;		a0-> hardware sprite struct
;		a1-> current position in sprite attr table
;		a3.w = VDPSpriteAddr
;	Outputs:
;		a0-> just beyond sprite struct
;	Destroys:
;		d0.w-d3.w,a0.l,a1.l
;===	Internal:
;===		d7 = lSprite count
;============================================================================
;					; make sprite attribute table entry
.Loop:
;						; do vert pos
	move.w	d1,d3
	add.w	(a0)+,d3				; add in pre-offset for this sprite
	move.w	d3,(a1)+				; write out vert pos
;						; do size & link data





	move.w	(a0)+,d3				; read size bits from sprite
	move.b	spriteNum,d3                            ; get current link # into lower byte
	addq.b	#1,d3					; increment link #
	cmp.b	#MAXSPRITES,d3
	beq.s	.Err
	move.w	d3,(a1)+				; write out size & link data
	move.b	d3,spriteNum				; store link # for next time
;						; do link # & char #
	move.w	d2,d3
	add.w	(a0)+,d3				; bring in bits from sprite record
	add.w	a3,d3					; add in character offset
	move.w	d3,(a1)+				; write out sprite # & control bits(flips, pal)

	move.w	d0,d3
	add.w	(a0)+,d3				; add in pre-offset for this sprite
	tst.w	d3
	bne	.no
	addq.w	#1,d3
.no:
	move.w	d3,(a1)+				; write out horiz pos
.Entry:
	dbra	d7,.Loop
	move.l	a1,spriteAttrPtr
	moveq	#1,d0					; clear Z flag
.Err:
	rts

;============================================================================
;===	Inputs:
;===		This routine is for AddLSprite, if you want to manualy
;===		add hardware sprites, use AddSprite
;===		d0.w = sprite x pos( $80 = left edge)
;===		d1.w = sprite y pos( $80 = top edge)
;===		d2 = sprite descriptor(use hardware equates)
;===		d4 = logical x size of sprite
;===		a0-> hardware sprite struct
;===		a1-> current position in sprite attr table
;===		a3.w = VDPSpriteAddr
;===	Outputs:
;===		a0-> just beyond sprite struct
;===	Destroys:
;===		d0.w-d3.w,a0.l,a1.l
;===	Internal:
;===		d6 = x offset
;===		d7 = lSprite count
;============================================================================
;					; make sprite attribute table entry
.XLoop:
;						; do vert pos
	move.w	d1,d3
	add.w	(a0)+,d3				; add in pre-offset for this sprite
	move.w	d3,(a1)+				; write out vert pos
;						; now calculate x offset for flip
	clr.w	d6
	move.b	(a0),d6					; read size bits
	add.w	d6,d6
	move.w	.xSizeTbl1(pc,d6.w),d6			; read actual size of sprite
;						; do size & link data
	move.w	(a0)+,d3				; read size bits from sprite
	move.b	spriteNum,d3                            ; get current link # into lower byte
	addq.b	#1,d3					; increment link #
	cmp.b	#MAXSPRITES,d3
	beq.s	.Err
	move.w	d3,(a1)+				; write out size & link data
	move.b	d3,spriteNum				; store link # for next time
;						; do link # & char #
	move.w	d2,d3
	add.w	(a0)+,d3				; bring in bits from sprite record
	add.w	a3,d3					; add in character offset
	move.w	d3,(a1)+				; write out sprite # & control bits(flips, pal)

	add.w	(a0)+,d6				; add in pre-offset to sprite size
	move.w	d4,d3
	sub.w	d6,d3                                   ; subract from logical sprite size to get new horizonal position inside logical sprite
	add.w	d0,d3					; add in position of logical sprite on screen
	move.w	d3,(a1)+				; write out horiz pos
.XFlippedSprite:
	dbra	d7,.XLoop
	move.l	a1,spriteAttrPtr
	moveq	#1,d0					; clear Z flag
	rts

;----------------------------------------------------------------------------

.xSizeTbl1:
	dc.w	8,8,8,8,16,16,16,16,24,24,24,24,32,32,32,32

;============================================================================
;	Inputs:
;		This routine is for AddLSprite, if you want to manualy
;		add hardware sprites, use AddSprite
;		d0.w = sprite x pos( $80 = left edge)
;		d1.w = sprite y pos( $80 = top edge)
;		d2 = sprite descriptor(use hardware equates)
;		a0-> hardware sprite struct
;		a1-> current position in sprite attr table
;		a3.w = VDPSpriteAddr
;	Outputs:
;		a0-> just beyond sprite struct
;	Destroys:
;		d0.w-d3.w,a0.l,a1.l
;===	Internal:
;===		d6 = y offset
;===		d7 = lSprite count
;============================================================================
;					; make sprite attribute table entry
.YLoop:
	clr.w	d6
	move.b	2(a0),d6					; read size bits
	add.w	d6,d6
	move.w	.ySizeTbl(pc,d6.w),d6			; read actual size of sprite

;						; do vert pos
	add.w	(a0)+,d6				; add in pre-offset for this sprite
	move.w	a4,d3
	sub.w	d6,d3                                   ; subract from logical sprite size to get new vertical position inside logical sprite
	add.w	d1,d3					; add in position of logical sprite on screen
	move.w	d3,(a1)+				; write out vert pos
;						; do size & link data
;
	move.w	(a0)+,d3				; read size bits from sprite
	move.b	spriteNum,d3                            ; get current link # into lower byte
	addq.b	#1,d3					; increment link #
	cmp.b	#MAXSPRITES,d3
	beq	.Err
	move.w	d3,(a1)+				; write out size & link data
	move.b	d3,spriteNum				; store link # for next time
;						; do link # & char #
	move.w	d2,d3
	add.w	(a0)+,d3				; bring in bits from sprite record
	add.w	a3,d3					; add in character offset
	move.w	d3,(a1)+				; write out sprite # & control bits(flips, pal)

	move.w	d0,d3
	add.w	(a0)+,d3				; add in pre-offset for this sprite
	move.w	d3,(a1)+				; write out horiz pos
.YFlippedSprite:
	dbra	d7,.YLoop
	move.l	a1,spriteAttrPtr
	moveq	#1,d0					; clear Z flag
	rts

;------------------------------------------------------------------------------

.ySizeTbl:
	dc.w	8,16,24,32,8,16,24,32,8,16,24,32,8,16,24,32

.xSizeTbl2:
	dc.w	8,8,8,8,16,16,16,16,24,24,24,24,32,32,32,32

;============================================================================
;	Inputs:
;		This routine is for AddLSprite, if you want to manualy
;		add hardware sprites, use AddSprite
;		d0.w = sprite x pos( $80 = left edge)
;		d1.w = sprite y pos( $80 = top edge)
;		d2 = sprite descriptor(use hardware equates)
;		a0-> hardware sprite struct
;		a1-> current position in sprite attr table
;		a3.w = VDPSpriteAddr
;	Outputs:
;		a0-> just beyond sprite struct
;	Destroys:
;		d0.w-d3.w,a0.l,a1.l
;===	Internal:
;===		d5 = y offset
;===		d6 = x offset
;===		d7 = lSprite count
;============================================================================
;					; make sprite attribute table entry
.XYLoop:
	clr.w	d6
	move.b	2(a0),d6					; read size bits
	add.w	d6,d6
	move.w	.ySizeTbl(pc,d6.w),d5			; read actual size of sprite
	move.w	.xSizeTbl2(pc,d6.w),d6			; read actual size of sprite
;						; do vert pos
	add.w	(a0)+,d5				; add in pre-offset for this sprite
	move.w	a4,d3
	sub.w	d5,d3                                   ; subract from logical sprite size to get new vertical position inside logical sprite
	add.w	d1,d3					; add in position of logical sprite on screen
	move.w	d3,(a1)+				; write out vert pos
;						; do size & link data
	move.w	(a0)+,d3				; read size bits from sprite
	move.b	spriteNum,d3                            ; get current link # into lower byte
	addq.b	#1,d3					; increment link #
	cmp.b	#MAXSPRITES,d3
	beq	.Err
	move.w	d3,(a1)+				; write out size & link data
	move.b	d3,spriteNum				; store link # for next time
;						; do link # & char #
	move.w	d2,d3
	add.w	(a0)+,d3				; bring in bits from sprite record
	add.w	a3,d3					; add in character offset
	move.w	d3,(a1)+				; write out sprite # & control bits(flips, pal)

	add.w	(a0)+,d6				; add in pre-offset to sprite size
	move.w	d4,d3
	sub.w	d6,d3                                   ; subract from logical sprite size to get new horizonal position inside logical sprite
	add.w	d0,d3					; add in position of logical sprite on screen
	move.w	d3,(a1)+				; write out horiz pos
.XYFlippedSprite:
	dbra	d7,.XYLoop
	move.l	a1,spriteAttrPtr
	moveq	#1,d0					; clear Z flag
	rts

;==============================================================================
;	Inputs:
;		d0.w = sprite x pos( $80 = left edge)
;		d1.w = sprite y pos( $80 = top edge)
;		d2 = sprite descriptor(use hardware equates)
;		d3 = sprite size
;		d4 = starting character number
;	Outputs:
;		Z if overflow, NZ if ok
;	Destroys:
;		d0.w-d3.w,a1.l
;------------------------------------------------------------------------------

AddSprite:
	move.l	spriteAttrPtr,a1
;						; do vert pos
	move.w	d1,(a1)+				; write out vert pos
;						; do size & link data
	move.b	spriteNum,d3                            ; get current link # into lower byte
	addq.b	#1,d3					; increment link #
	cmp.b	#MAXSPRITES,d3
	beq.s	.OverFlow
	move.w	d3,(a1)+				; write out size & link data
	move.b	d3,spriteNum				; store link # for next time
;						; do link # & char #
	add.w	spriteVDPBase,d2			; add in character offset
	move.w	d2,(a1)+				; write out sprite # & control bits(flips, pal)
;
	move.w	d0,(a1)+				; write out horiz pos
	move.l	a1,spriteAttrPtr

	moveq	#1,d0					; clear Z flag
.OverFlow:
	rts

;============================================================================
; copy sprite attr buffer to vdp
; 	Inputs:
;		d0 = where in vdp your sprite attrib table goes
;============================================================================
	xdef	_CopySprites
_CopySprites:
	move.w	6(sp),d0
CopySprites:
	bsr	VDPAddress			; point to correct spot in vdp
	tst.b	spriteNum
	beq.s	NoSprites
	move.l	spriteAttrPtr,a0
	clr.b	-5(a0)				; nuke link data on last sprite
;	clr.l	(a0)				; clear next sprite entry(only 2nd word needs clearing since it contains the link data)
	lea	spriteAttrBuffer,a0
	lea	VDP_DATA,a1
.Loop:
	move.l	(a0)+,(a1)			; do 1st long
	beq.s	.Done
	move.l	(a0)+,(a1)			; do 2nd long
	bra.s	.Loop
.Done:						; all done, no more
	rts

;==============================================================================

NoSprites:
	WRITE_VDP_DATA #0
	rts

;============================================================================
; copy sprite attr buffer to vdp in reverse order
; 	Inputs:
;		d0 = where in vdp your sprite attrib table goes
;	Internal:
;		d0 = new sprite link data
;		d1 = sprite counter
;		d2.l = temp
;		a0-> sprite buffer
;		a1->VDP_DATA
;============================================================================

CopySpritesReversed:
	bsr	VDPAddress			; point to correct spot in vdp
	tst.b	spriteNum
	beq.s	NoSprites
	move.l	spriteAttrPtr,a0
	clr.w	d1
	move.b	spriteNum,d1			; # of sprites to download
	subq.w	#1,d1
	clr.b	d0

	lea	VDP_DATA,a1
	sub.l	#SPRITEATTR_SIZE,a0		; back up just one to start
	bra.s	.Entry
.Loop:
	move.l	(a0)+,d2			; read 1st long
	move.b	d0,d2
	move.l	d2,(a1)				; write 1st long
	move.l	(a0)+,(a1)			; do 2nd long
	sub.l	#SPRITEATTR_SIZE*2,a0
.Entry:
	addq.b	#1,d0
	dbra	d1,.Loop

	move.l	(a0)+,d2			; read 1st long
	clr.b	d2
	move.l	d2,(a1)				; write 1st long
	move.l	(a0)+,(a1)			; do 2nd long
	rts


	xdef	_CopyForward
_CopyForward:
	movea.l	4(sp),a0		; get dest
	movea.l	8(sp),a1		; get source
	move.w	14(sp),d0		; get #
	move.l	a2,-(sp)
	asl.w	#5,d0			; * 32
	lea	(a1,d0.w),a1

	movem.l	(a1),d0-d2/a2
	movem.l	d0-d2/a2,(a0)

	movem.l	16(a1),d0-d2/a2
	movem.l	d0-d2/a2,16(a0)
	move.l	(sp)+,a2
	rts

	xdef	_ClearSpace
_ClearSpace:
	movea.l	4(sp),a0
;	move.w	#7,d0
;.1:
	clr.l	d1
	move.l	d1,(a0)+
	move.l	d1,(a0)+
	move.l	d1,(a0)+
	move.l	d1,(a0)+
	move.l	d1,(a0)+
	move.l	d1,(a0)+
	move.l	d1,(a0)+
	move.l	d1,(a0)+
;	clr.l	(a0)+
;	dbra	d0,.1
	rts

;============================================================================

	BSECTION	.bss
	ds.w	1	;even

; buffer containing image of vdp sprite attribute table
spriteAttrBuffer:
		ds.b	hSprite_Size*MAXSPRITES		; # of bytes per entry * # of entries

;			; non-volatile variables
	ds.w	1	; even

; address in vdp sprite char file is loaded to
spriteVDPBase:
		ds.w	1

;			; volatile variables
; pointer to current position in sprite attribute table
spriteAttrPtr:
		ds.l	1
; current sprite link #
spriteNum:
		ds.b	1

;============================================================================

;============================================================================

	END

;============================================================================

