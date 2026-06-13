;============================================================================
;
; File:    256.asm
;
; Project: SNES Graphics Downloader
;
; History:
;	18 Mar 92	WBNIV	Created
;
;============================================================================

TITLESCREEN
;DEBUG


VERSION EQU $01


	INCLUDE	256.mac
	INCDIR	lib
	INCLUDE	snes.equ
	INCLUDE	snes.mac
	INCLUDE	bin.mac
	INCDIR
	INCLUDE	genie.equ

	INCLUDE	segments.asm

ZPAGE	SEGMENT
count	ds	1


;===============================================================================

	INCLUDE	lib/vector.asm


	LIST ON

CODE	SEGMENT
	dt	'Copyright 1994 Developer Resources.  All Rights Reserved.'
bootstrap:
	sei                     	; disable interrupts

	clc
	xce                     	; leave emulation mode

	mode16
	lda	#$01FF
	tcs                     	; initialize stack

	phb
	phd

	ldx	#code_in_rom
	ldy	#code_in_ram
	lda	#code_size-1
	mvn	^code_in_rom,^code_in_ram

	pld
	plb

	LIST ON
	jml	code_in_ram

code_in_rom:
	LORG	$7E2000
code_in_ram:
	jml	main
	LIST OFF

	INCDIR	lib
	INCLUDE	snes.asm
	INCDIR

	INCLUDE	map.asm
	INCLUDE	text.asm
	INCLUDE	display.asm
	INCLUDE	titlscrn.asm
	INCLUDE	status.asm
	INCLUDE	memview.asm

	CHRINC	drlogo,chr
	MAPINC	drlogo,map
	PALINC	drlogo
	CHRINC	blank,chr

main:
	SEI                     ; disable interrupts

	CLC
	XCE                     ; leave emulation mode
	mode16
	lda	#$01FF
	tcs                     ; initialize stack

	JSR	InitSFX


	a8
	lda	#(0<2)|0
	sta	BG1SC
	lda	#(1<2)|0
	sta	BG2SC
	lda	#(2<2)|0
	sta	BG3SC

	lda	#$42        	; bg2 charbase / bg1 charbase
	sta	BG12NBA
	lda	#$00
	sta	BG34NBA

	mode16
MainLoop:
	blank
	jsr	InputInit

	lda	#Display_StatusScreen
	sta	displayPtr
	lda	#>Display_StatusScreen
	sta	displayPtr+1
	jsr	DisplayInit

	jsr	ClearVdpMem

;;;	jsr	TitleScreen_Display

	;----- text init
	LoadChr	font8,#$0C00
	lda	#BG3_0
	jsr	Text_Init
	lda	#(1<13)|384			; priority
	sta	textCHARMASK

;;;	a8
;;;	lda	#%10000000
;;;	sta	NMITIMEN
;;;	a16

	a8
	lda	#%110			; enable cartridge only
	sta	WR_GENIE_CONTROL8

	jmp	MemoryViewer


;   +------------+-------------+---------+--------------+
;   | Source     | Destination | Through | Size (Bytes) |
;   +------------+-------------+---------+--------------+
; 0 | oam_attrib |             | OAMDATA |      544     |	[ NOT USED ]
; 1 | BG1_0      | $0000       | VMDATAL |     2048     |
; 2 | BG2_0      | $0400       | VMDATAL |     2048     |
; 3 | BG3_0      | $0800       | VMDATAL |     2048     |
;   +------------+-------------+---------+--------------+

VBI:
	pha
	php

	a8
	lda	#%10000000
	sta	INIDISP

	lda	$4210

	jsr	DMA_Maps
;T	jsr	MoveBg

	lda	#%00001111
	sta	INIDISP

	jsr	GetJoyInput

	plp
	pla

	rts


DMA_Maps:
	pha
	phx
	php

	;----- Setup update of backgrounds through DMA
	a8
;;	lda	#^oam_attrib
;;	sta	$4304
	lda	#^BG1_0
	sta	$4314           ; A table bank
	lda	#^BG2_0
	sta	$4324
	lda	#^BG3_0
	sta	$4334

;;	lda	# (0<7) | (0<3) | (0)
;;	sta	$4300
	lda	# (0<7) | (0<3) | (1)
	sta	$4310           ; parameter for DMA transfer
	sta	$4320
	sta	$4330

;;	lda	# ( OAMDATA - $2100 )
;;	sta	$4301
	lda	# ( VMDATAL - $2100 )
	sta	$4311           ; B address
	sta	$4321
	sta	$4331

	a16
;;	lda	#oam_attrib
;;	sta	$4302
	lda	#BG1_0
	sta	$4312           ; A table address
	lda	#BG2_0
	sta	$4322
	lda	#BG3_0
	sta	$4332

;;	lda	#(256+16)*2
;;	sta	$4305
	lda	#(32*29)*2
	sta	$4315           ; number of BYTES to be transferred
	sta	$4325
	sta	$4335

	a16
	lda	#$0000
	sta	VMADDL

	;-- and only do this on vblank
	a8
	lda	#$80
	sta	VMAINC

	lda	# (1<1)
	sta	$420B

	a16
	lda	#$0400
	sta	VMADDL
	a8
	lda	#(1<2)
	sta	$420B

	a16
	lda	#$0800
	sta	VMADDL
	a8
	lda	# (1<3)
	sta	$420B

;;	a16
;;	lda	#0
;;	sta	OAMADDL
;;	a8
;;	lda	# (1<0)
;;	sta	$420B

  	plp
	plx
	pla
	rts




	LIST ON
code_size	=	(PC - code_in_ram) + 1
	LIST OFF



	INCLUDE	header.asm


	END

;============================================================================
