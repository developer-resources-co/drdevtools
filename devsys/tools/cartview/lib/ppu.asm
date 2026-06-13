;==============================================================================
;	ppu.asm: SNES PPU functions
;==============================================================================

;==============================================================================
;
; BgZero:
;	Positions all BG's to (0,0)
;
BgZero:
	php
	a8

	stz     BG1HOFS
	stz     BG1HOFS
	stz     BG1VOFS
	stz     BG1VOFS
	stz     BG2HOFS
	stz     BG2HOFS
	stz     BG2VOFS
	stz     BG2VOFS
	stz     BG3HOFS
	stz     BG3HOFS
	stz     BG3VOFS
	stz     BG3VOFS
	stz     BG4HOFS
	stz     BG4HOFS
	stz     BG4VOFS
	stz     BG4VOFS

	plp
	rts







;==============================================================================
; SetPalette -- set a palette
;	a ->	16 bit ptr to palette
;	x =	# of positions to fill
;	y = 	starting position
;==============================================================================
ZPAGE   segment
lpPalette	ds	3


;==============================================================================

CODE    segment

SetPalette:
	PUSH_ALL		; enter in A16 mode

	sta	lpPalette
	a8
	stz	lpPalette+2

	jsr	NewSetPalette

	PULL_ALL
	rts


;==============================================================================
; NewSetPalette -- set a palette
; ***** REname THIS CODE! *****
; setup:
;	lpPalette ->	24 bit ptr to palette
;	x 	=	# of positions to fill
;	y 	= 	starting position
;==============================================================================

NewSetPalette:
	PUSH_ALL		; enter in A16 mode
	a8
	x16

	tya
	sta     CGADDR          ; store palette entry #
	ldy     #0
.loop:
	lda     [lpPalette],y
	iny
	sta     |CGDATA
	lda     [lpPalette],y
	iny
	sta     |CGDATA
	dex
	bne     .loop

	PULL_ALL
	rts

;==============================================================================
; kts added code

pushz	macro
	lda	{1}
	pha
	endm

popz	macro
	pla
	sta	{1}
	endm

;------------------------------------------------------------------------------

sram	segment
; cross fade variables

fadeVal		ds	2
invFadeVal	ds	2
palOff		ds	2
palTemp		ds	2
palTemp2	ds	2

;==============================================================================

code	segment



	END
