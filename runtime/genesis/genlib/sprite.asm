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

hSprite		DEFS	RAM
hSprite		SEGMENT
		ORG	0

hSpriteVPos		ds.w	1	; vertical position relative to top of logical sprite
hSpriteSize		ds.b	1	; hardware sprite size bits
hSpriteLink		ds.b	1	; linkage information(null in file)
hSpriteFlips
hSpriteChar		ds.w	1	; flip bits and starting char #
hSpriteHPos		ds.w	1	; horizontal position relative to left of logical sprite
hSprite_Size

;------------------------------------------------------------------------------
; logical sprite format

lSprite		DEFS	RAM
lSprite		SEGMENT
		ORG	0

lSpriteXOffset		ds.w	1	; x & y offset in iff file sprite
lSpriteYOffset		ds.w	1	; was cut from(only used by sprite grabber)
lSpriteXSize		ds.w	1	; logical sprite x & y size
lSpriteYSize		ds.w	1
lSpriteHardXOffset	ds.w	1	; offset from logical sprite to upper left edge
lSpriteHardYOffset	ds.w	1	; of hardware sprites(used for collision detection)
lSpriteHardXSize	ds.w	1	; x & y size of hardware sprites
lSpriteHardYSize	ds.w	1	; (used for collision detection)
lSpriteCount		ds.w	1       ; # of hardware sprites in this logical sprite
lSprite_Size

;============================================================================

code	segment

;============================================================================
;	Inputs:
;		d0.w = address in vdp(in chars) that sprite char file is
;			loaded
;	Destroys:
;		none
;============================================================================

InitSprites:
	move.w	d0,spriteVDPBase
;	bsr.s	ClearSpriteTable
;	rts				; just fall through

;============================================================================
; 	Destroys:
;		none
;------------------------------------------------------------------------------

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
;		d0.w-d3,d7.w,a0.l-a2.l,a3.w
;============================================================================

AddLSprite:
	move.w	spriteVDPBase,a3
AddLSpriteBase:
	add.w	#$80,d0					; adjust for screen offset
	add.w	#$80,d1
	move.w	lSpriteCount(a0),d7			; get physical(hardware) sprite count
	move.w	lSpriteXSize(a0),d4			; get logical sprite xSize(kts 2/27/92)
	lea	lSprite_Size(a0),a0			; skip logical sprite structure
	move.l	spriteAttrPtr,a1

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
;============================================================================
					; make sprite attribute table entry
.Loop:
						; do vert pos
	move.w	d1,d3
	add.w	(a0)+,d3				; add in pre-offset for this sprite
	move.w	d3,(a1)+				; write out vert pos
						; do size & link data





	move.w	(a0)+,d3				; read size bits from sprite
	move.b	spriteNum,d3                            ; get current link # into lower byte
	addq.b	#1,d3					; increment link #
	cmp.b	#MAXSPRITES,d3
	beq.s	.Err
	move.w	d3,(a1)+				; write out size & link data
	move.b	d3,spriteNum				; store link # for next time
						; do link # & char #
	move.w	d2,d3
	add.w	(a0)+,d3				; bring in bits from sprite record
	add.w	a3,d3					; add in character offset
	move.w	d3,(a1)+				; write out sprite # & control bits(flips, pal)

	move.w	d0,d3
	add.w	(a0)+,d3				; add in pre-offset for this sprite
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
;============================================================================
					; make sprite attribute table entry
.XLoop:
						; do vert pos
	move.w	d1,d3
	add.w	(a0)+,d3				; add in pre-offset for this sprite
	move.w	d3,(a1)+				; write out vert pos
						; now calculate x offset for flip
	clr.w	d6
	move.b	(a0),d6					; read size bits
	add.w	d6,d6
	move.w	.xSizeTbl(pc,d6.w),d6			; read actual size of sprite
						; do size & link data
	move.w	(a0)+,d3				; read size bits from sprite
	move.b	spriteNum,d3                            ; get current link # into lower byte
	addq.b	#1,d3					; increment link #
	cmp.b	#MAXSPRITES,d3
	beq.s	.Err
	move.w	d3,(a1)+				; write out size & link data
	move.b	d3,spriteNum				; store link # for next time
						; do link # & char #
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

;------------------------------------------------------------------------------

.xSizeTbl:
	dc.w	8,8,8,8,16,16,16,16,24,24,24,24,32,32,32,32

.ySizeTbl:
	dc.w	8,16,24,32,8,16,24,32,8,16,24,32,8,16,24,32

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
						; do vert pos
	move.w	d1,(a1)+				; write out vert pos
						; do size & link data
	move.b	spriteNum,d3                            ; get current link # into lower byte
	addq.b	#1,d3					; increment link #
	cmp.b	#MAXSPRITES,d3
	beq.s	.OverFlow
	move.w	d3,(a1)+				; write out size & link data
	move.b	d3,spriteNum				; store link # for next time
						; do link # & char #
	add.w	spriteVDPBase,d2			; add in character offset
	move.w	d2,(a1)+				; write out sprite # & control bits(flips, pal)

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
	move.l	#0,VDP_DATA
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

;============================================================================

ram	segment
	even
			; buffers
spriteAttrBuffer:			; buffer containing image of vdp sprite attribute table
		ds.b	hSprite_Size*MAXSPRITES		; # of bytes per entry * # of entries

			; non-volatile variables
	even
spriteVDPBase:	ds.w	1	; address in VDP sprite char file is loaded to

			; volatile variables
spriteAttrPtr:                  	; pointer to current position in sprite attribute table
		ds.l	1
spriteNum:       			; current sprite link #
		ds.b	1

;============================================================================

;============================================================================

	END

;============================================================================
