;==============================================================================
; bootrom.asm: bootstrap to get Genesis Rom running
;==============================================================================

	LLCHAR	.

COPYRAM	EQU	1		; 0 = normal, 1 = copy self to ram
WORMHOLE	equ	$230000

;==============================================================================

RAM	defs	ram
ram	segment
	org	$ff8000

CODE	DEFS	ROM
CODE	SEGMENT

	org	$0000
	porg	$0

;==============================================================================

CODE	segment

Stack	equ	$00ffef00

	include except.asm
	include header.asm
SegaInit:
	incbin	segainit.bin

	if	COPYRAM
	move.l	#EndOfRom,a0
	move.l	#(last/4),d0
	move.l	#$ff0000,a1
.1:
	move.l	(a0)+,(a1)+
	dbra	d0,.1
	jmp	RamStart
EndOfRom:

	org	$ff0000
	porg	EndOfRom
RamStart:
	endif

	jmp	start

	incdir	\genesis\lib\
	include	default.asm
	include genesis.asm

;==============================================================================

VDPSeg	DEFS	RAM

VDPSeg	SEGMENT
	org	$000
		ds.b	CHARSIZE		; always leave a blank char at zero so it will be easy to clear screen
VDPText		ds.b	TEXT_SIZE
VDPEnd

;==============================================================================

code	segment
Start:
	move.b	#$aa,0				; aa means worm ok
	move.b	WORMHOLE,d0
	cmp.b	#$aa,d0
	beq.s	.Nope
	move.b	#$55,0				; $55 means worm bad
.Nope:
	; tell pc we are running
	lea	testString,a0
	move.l	#1,a1
.CLoop:
	tst.b	(a0)
	move.b	(a0)+,(a1)+
	bne.s	.CLoop

	; set up wormhole
	move.b	#$aa,WORMHOLE

				; initialize the VDP
	lea	VDPTable,a0		; this uses the default.asm vdp init
	jsr	InitVDP

	move.w	#8,d0
	move.w	#$fff,d1
	jsr	SetColor

				; clear front screen
	jsr	ClearFieldA
	jsr	ClearFieldB

	clr.w	d0
	jsr	SetHScrollA
	jsr	SetHScrollB

	move.w	#VDPText,d0
	jsr	InitText

	move.w	#10,cursorX
	move.w	#10,cursorY
	lea	testText,a0
	jsr	PrintText

.FOO
	COLORCYCLE
	BRA	.FOO


NMI:
 	move.b	#$12,WORMHOLE
.OutLoop:
	COLORCYCLE
	move.w	#1000,d0
.Loop
	dbra	d0,.Loop
	bra	.OutLoop


;==============================================================================

testString:
	dt	'Monitor Running'
	dc.b	0

testText:
	dt	'Test Rom'
	dc.b	0

;==============================================================================

	if	COPYRAM
last	equ	pc-$ff0000
	endif

;==============================================================================

	END

;==============================================================================




;==============================================================================
