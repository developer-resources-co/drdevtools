;=============================================================================
;	test.asm: display a static graphic
;=============================================================================


; 8/16 general equates

; FLAGS
LONGI	EQU	$10
LONGA	EQU	$20

howlong	equ	0
needcharset	equ	1

;==============================================================================

;==============================================================================
;	SEGMENTS.ASM --- Definition for all of the segments.
;==============================================================================

SRAM	DEFS	RAM
SRAM	SEGMENT
	SIZE	$2000
	ORG	$0

INIT	DEFS	ROM
INIT	SEGMENT
	size	$4000
	org	$a000
	porg	$2000


CODE	DEFS	ROM
CODE	SEGMENT
	size	$1600
	org	$200
	porg	$200
CODEBASE	equ	pc

;=============================================================================


ZPAGE	DEFS	RAM

ZPAGE	SEGMENT
	SIZE	$FF
	org	0

;=============================================================================

zpage	segment
COUNT	DS	1
COUNT2	DS	1

VBFLAG	ds	2

;=============================================================================

code	segment
	include	sfx.inc			; equates
	include	macro.asm
	include	snes.asm		; includes VDP, JOY, & GENERAL
	include	text.asm

;==============================================================================

init	segment

charset:
	incbin	charset.chr

NMIVEC:
	rti

Top:
	SEI			; disable interrupts

	clc
	xce			; leave emulation mode
	mode16
	LDX  #$1fff
	TXS			; initialize stack
	mode8			; 8 bit X, Y, and A

				; copy into ram loop
	LIST	ON
RamStart:
   	jsr	InitSFX

	mode16
	lda	#LASTBYTE-CODEBASE
	ldx	#$8200
	ldy	#$200

	mvn	$20,0




	; in this test, I've used BG mode 2 ( two 4-bit planes ) to
	; be as similar to the Genesis as possible.
	; changed to BG mode 1 ( two 4-bit planes, plus a 2-bit plane
	; for diagnostic text )

; now, I'll set up the VDP memory map.  I'll put BG1 and BG2 map at 16k

	mode8
	lda	#$38+1

	sta	$2107	; bg1 mapbase + size
	lda	#$78+1
	sta	$2108	; bg2 mapbase + size
	lda	#$78
	sta	$2109	; bg3 mapbase + size

	lda	#1	; mode 1 = 4 + 4 + 2
	sta	$2105	; set BG mode

	lda	#4	; turn on only BG3 [Diagnostic Text Plane]
	sta	$212c	;

	lda	#$40	; bg2 charbase / bg1 charbase
	sta	BG12NBA

	lda	#$0
	sta	BG34NBA ; put it on top of BG1's chars for now

; let's copy the charset into vidram at zero

	blank
	mode16

	jsr clearvdpmem

	mode16

	if	needcharset
	lda	#charset		; get charset pointer
	sta	start

	a8
	lda	#^charset
	sta	start+2
	a16
	lda	#$200		; length
	sta	length

	lda	#$0
	sta	vmaddl

	jsr	VDPCopy		; do it!
	endif
	jml	MainLoop

code	segment

MainLoop:

	a16
	lda	#TestPal
	sta	PALPTR
	ldx	#$40
	ldy	#$0
	jsr	SetPal

	unblank

	waitvb

	mode16
	lda	#$0
	sta	CurMem
	a8
	sta	CurBank
	a16

	list	off
	unblank
?1:
	a8
	lda	CurBank
	pha
	plb
	a16

	lda	CurMem
	tay
	jsr	ViewPage

	jsr	ReadJoy1
	lda	Joy1Shadow

	and	#JOYRBUTTON
	beq	?2
	lda	CurMem
	adc	#$80
	sta	CurMem

?2:
	lda	Joy1Shadow
	and	#JOYLBUTTON
	beq	?3
	lda	CurMem
	sec
	sbc	#$80
	sta	CurMem

?3:
	lda	Joy1Shadow
	and	#JOYRIGHT
	beq	?4
	lda	CurMem
	clc
	adc	#$1000
	sta	CurMem

?4:
	lda	Joy1Shadow
	and	#JOYLEFT
	beq	?5
	lda	CurMem
	sec
	sbc	#$1000
	sta	CurMem

?5:
	lda	Joy1Shadow
	and	#JOYUP
	beq	?6
	a8
	inc	<CurBank
	a16
?6:
	lda	Joy1Shadow
	and	#JOYDOWN
	beq	?7
	a8
	dec	<CurBank
	a16
?7:
	lda	Joy1Shadow
	and	#JOYBBUTTON
	beq	?8
	a8
	lda	<CurBank
	eor	#$80
	sta	<CurBank
	a16
?8:
	jmp	?1

zpage	segment
CurMem	ds	2
CurBank	ds	1


;==============================================================================
; ViewPage -- display 128 bytes of memory.
; 	Y = starting address
;==============================================================================
zpage	segment
line	ds	2
pos	ds	2

code	segment
ViewPage:
	phy
	ldx	#$2a
	ldy	#?2
	jsr	Print
	ply
	tya
	ldx	#$38
	jsr	PrHex
	lda	#0
	a8
	phb
	pla
	a16
	ldx	#$33
	jsr	PrHex
	lda	#16
	sta	line
	lda	#$84
	sta	pos

?1:
	ldx	pos
	jsr	ViewLine
	lda	#$20
	clc
	adc	pos
	sta	pos
	dec	line
	bne	?1

	rts
?2:
	dt	'MEMORY @ 0000*0000'
	db	0





;==============================================================================
; ViewLine -- display 8 bytes of memory.
;	Y = starting address
;	X = first char
;==============================================================================
ViewLine:
	waitvb
	lda	|0,y
	xba
	jsr	PRHEX
	iny
	iny
	txa
	clc
	adc	#$5
	tax

	lda	|0,y
	xba
	jsr	PRHEX
	iny
	iny
	txa
	clc
	adc	#$5
	tax

	lda	|0,y
	xba
	jsr	PRHEX
	iny
	iny
	txa
	clc
	adc	#$5
	tax

	lda	|0,y
	xba
	jsr	PRHEX
	iny
	iny
	rts

LASTBYTE	db	0

init	segment
Welcome:
	dt	'GREETINGS'
	db	0

TestPal	dw	0,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff
	dw	$ffff,$ffff,$ffff,$ffff

ColorCycle:
	LDA  COUNT
	STA  $2122
	LDA  COUNT2
	STA  $2122
	INC  COUNT
	BNE  ?1
	DEC  COUNT2
?1
	rts

init	segment
	include init.asm
BogusSym	equ	0
	END

; end of file
;=============================================================================

