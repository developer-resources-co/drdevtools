;===============================================================================
;
; File:    text.asm
;
; Project: Developer Resources Development System ROM
;	(c) 1992 Developer Resources.  All Rights Reserved.
;
; History:
;   18 Nov 92   WBNIV   Created (Pulled from Waldo project)
;
;
;===============================================================================


ZPAGE	SEGMENT
textPtr		ds	3	; Pointer to text to print
textCursorPtr	ds	2
textScreenPtr	ds	2	; Screen base to print on
fontPtrL		ds	3	; Pointer to font map
textTemp		ds	2
nLeftMargin	ds	2	; not used so far
textCHARMASK	ds	2


;===============================================================================

CODE	SEGMENT

	CHRINC	font8,chr
	MAPINC	font8,map
	PALINC	font8

hexAsciiTranslate:	dt	'0123456789ABCDEF'

;===============================================================================
;
; Text_Init
;	This routine initialize the variables needed for the text-printing
;	routines.  Required information include base address of screen
;	to render text on, ...
;
; Inputs:
;
Text_Init:
	php
	mode16

	sta	textScreenPtr
	sta	textCursorPtr

	lda	#font8Map
	sta	fontPtrL
	lda	#>font8Map
	sta	fontPtrL+1

	stz	nLeftMargin
	stz	textCHARMASK

	plp
	rts


	asm16
; MoveCursor (16-bit)
;	X,Y:	coordinates to print text at
MoveCursor:
	php

	txa
	sta	textCursorPtr

	lda	#32
	tyx
	jsr	Multiply
	clc
	adc	textCursorPtr
;UNTESTED	adc	nLeftMargin

	asl	a

	adc	textScreenPtr
	sta	textCursorPtr

	plp
	rts


	asm8
PrintHex4:
	tax
	lda	hexAsciiTranslate,x
;	jsr	PrintChar
;	rts

;          ^
PrintChar:
	php
	a16
	and	#$007F

	clc
	adc	textCHARMASK

	sta	(textCursorPtr)
	inc	textCursorPtr
	inc	textCursorPtr

	plp
	rts


PrintHex8:
	pha
	phx
	php
	mode8

	tay

	lsr
	lsr
	lsr
	lsr
	jsr	PrintHex4

	tya
	and	#$0F
	jsr	PrintHex4

	plp
	plx
	pla
	rts


PrintBinary8:
	phx
	php
	mode8

	ldy	#8-1
.10:
	pha
	and	#$80
	lsr
	lsr
	lsr
	lsr
	lsr
	lsr
	lsr
	tax
	lda	hexAsciiTranslate,x
	jsr	PrintChar
	pla
	rol

	dey
	bpl	.10

	plp
	plx
	rts


;
; !!! NOT DONE !!!
; This routine only prints the first three characters
;
Print16:
	phx
	php
	mode16

	ldx	#100
	jsr	Divide
	clc
	adc	#'0'
	cmp	#'0'
	beq	.no_100
	jsr	PrintChar
.no_100:

	txa
	ldx	#10
	jsr	Divide
	clc
	adc	#'0'
	cmp	#'0'
	beq	.no_10
	jsr	PrintChar
.no_10:

	txa
	clc
	adc	#'0'
	jsr	PrintChar

	plp
	plx
	rts

;===============================================================================
;
; PrintString
;
; Inputs:
;	textPtr:	long pointer to NUL-terminated text string
;
PrintString:
	PUSH_ALL
	mode16

.10	lda	[textPtr]
	and	#$007F
	beq	.exit
	jsr	PrintChar
	inc	textPtr
	bra	.10

.exit:	PULL_ALL
	rts

	END
