;==============================================================================
; defend.asm: sample scrolling shooting game
;	By Kevin T. Seghetti
;==============================================================================

	LLCHAR	.

;==============================================================================
; equates and defines

SCREENWIDTH	EQU	320
SCREENHEIGHT	EQU	224
;;SCROLLWRAP

;==============================================================================
; init segments, this demo runs from $000

CODE	DEFS	ROM

CODE	SEGMENT
	org	$0

RAM	DEFS	RAM

RAM	SEGMENT
	org	$ff0000
	size	$ffff

;==============================================================================

code	segment

Stack	equ	$00ffef00

	incdir	l:genesis\lib\

	include except.asm

CODE	segment
	incdir
	include	default.asm
	incdir	l:genesis\lib\
	include genesis.asm

;==============================================================================

CODE	segment

	incdir	gfx

	; include backgrouds

	chrinc	stars

fieldMap
	incbin	field.rot			; include map
	even

	; include sprites
SPROFFSET	MACRO
{1}:	EQU	(pc-spriteChr)/CHARSIZE
	ENDM

;------------------------------------------------------------------------------

sprinc	macro
{1}OFFSET	equ	(pc-spriteChr)/CHARSIZE
	incbin	{1}.chr
	endm

spriteChr:
	sprinc	ship
	sprinc	missile1
	sprinc	missile2
	sprinc	enemy
spriteSize	equ	pc-spriteChr

;==============================================================================

sprShip:
	incbin	ship.spr

sprMissile1:
	incbin	missile1.spr

sprMissile2:
	incbin	missile2.spr

sprEnemy:
	incbin	enemy.spr


	palinc	spr
	palinc	field

	incdir

;==============================================================================

VDPSeg	DEFS	RAM

VDPSeg	SEGMENT
	org	$000
	size	VDP_TOPFREE

		ds.b	CHARSIZE		; always leave a blank char at zero so it will be easy to clear screen
VDPField	ds.b	starsSize
VDPSprite	ds.b	VDP_SPRITEBASE-VDPSprite
VDPEnd

;==============================================================================

code	segment

spriteBase:
	dc.l	SHIPOFFSET+(VDPSprite/CHARSIZE)
	dc.l	sprShip

	dc.l	MISSILE1OFFSET+(VDPSprite/CHARSIZE)
	dc.l	sprMissile1

	dc.l	MISSILE2OFFSET+(VDPSprite/CHARSIZE)
	dc.l	sprMissile2

	dc.l	ENEMYOFFSET+(VDPSprite/CHARSIZE)
	dc.l	sprEnemy

;==============================================================================

CODE	segment
Start:
				; initialize the VDP
	lea	VDPTable,a0		; this uses the default.asm vdp init
	jsr	InitVDP


	move.l	#$7392facb,randSeed
	setpal	field,#0
	setpal	spr,#48

	jsr	ClearFieldA

	LoadChr	sprite,VDPSprite

	move.w	#VDPSprite/CHARSIZE,d0
	move.w	#(VDPEnd-VDPSprite)/32,d1
	lea	spriteChr,a0
	jsr	InitSprites			; init sprite engine

	jsr	InitObjects			; set up object handler

;	clr.w	d0
;	jsr	SetHScrollA

; setup background
	LoadChr	stars,VDPField
	lea	scrollB,a0
	clr.w	d0
	clr.w	d1
	move.w	#VDPField/CHARSIZE,d2
	move.w	#FIELD_A,d3
	lea	fieldMap,a1
	jsr	InitHorizScroll

; get ready for main loop

	bsr	InitGameObjects			; game specific objects added to list

;-----------------------------------------------------------------------------
; this is the entire main loop
; all of the real work is done in the objects

.Loop:
	jsr	ClearSpriteTable		; reset sprites
	jsr	UpdateObjects			; do all objects in game
;;	SETBACKCOLOR	$0
	WAITVB					; done with frame
;;	SETBACKCOLOR	$444
	move.w	#VDP_SPRITEBASE,d0
	jsr	CopySprites                     ; update sprite positions in vdp
	bra	.Loop				; loop forever

;==============================================================================

	incdir
	include	gameobj.asm
	include player.asm
	include	weapon.asm
	include manager.asm
	include	enemy.asm

;==============================================================================

RAM	segment
	even
scrollB	ds.b	Scroll_Size

;==============================================================================

CODE	segment

ENDOFGAME:

	END

;==============================================================================
