;==============================================================================
; SetPal -- set a palette
; setup:
;	PALPTR  = 16 bit ptr to palette
;	     x  = # of positions to fill
;	     y  = starting position
;==============================================================================
ZPAGE	segment
PALPTR	ds	2

CODE	segment

SetPal:
	php
	mode8
	blank
	tya
	sta	$2121		; set starting address
	ldy	#0
?loop:
	lda	(PALPTR),y
	sta	$2122
	iny
	lda	(PALPTR),y
	sta	$2122
	iny
	dex
	bne  	?loop
	plp
	rts

;=============================================================================
; Plot -- display a map.
; setup:
;	MAPPTR = 16 ptr to map in DB
;	XOFFSET = # of chars from left edge
;	YOFFSET = # of chars from top edge
;
;=============================================================================

ZPAGE	SEGMENT

MAPPTR	ds	2		; pointer to MAP data
XOFFSET	ds	2      		; position on screen
YOFFSET	ds	2

GDATA	ds	2
CHAROFFSET ds	2
MAPADDRESS ds	2

CODE	SEGMENT


Plot:

	php
	mode8
	blank
	mode16
	clc
	lda	#$4
	adc	MAPPTR		; get address of data
	sta	GDATA

	lda	YOFFSET
	and	#$ff		; constrain
	rol	a		; * 2
	rol	a		; * 4
	rol	a 		; * 8
	rol	a		; * 16
	rol	a		; * 32
	sta	YOFFSET		; now contains word offset

	ldy	#$2
	lda	(MAPPTR),y	; get Y size
	tay

?yloop:
	lda	(MAPPTR)		; get X size
	tax
	; comptue screen address
	lda	MAPADDRESS		; pick up screenbase
	clc
	adc	YOFFSET		; add in Y noise
	adc	XOFFSET		; add X noise
	sta	vmaddl		; set vid address
	a8
	lda	#$80
	sta	vmainc
	a16
?xloop:
	lda	(GDATA)		; pick up a word of map
	clc
	adc	CHAROFFSET
	sta	vmdatal		; write to vdp ram
	inc	GDATA
	inc	GDATA		; point to next word of data
	dex
	bne	?xloop		; for all of the X data

	clc
	lda	#$20		; ready to point to next line
	adc	YOFFSET
	sta	YOFFSET
	dey
	bne	?yloop
	plp
	rts



;==============================================================================
; VDPView:
; look at the VDP
;
;==============================================================================


VDPView:
	php
	mode8
	blank
	mode16
	lda	#$4000
	sta	vmaddl
	ldx	#$400
	lda	#$0
?loop:
	sta	vmdatal
	inc	a
	dex
	bne	?loop
	mode8
	unblank
	plp
	rts
	mode8

;=============================================================================
;=============================================================================


;=============================================================================
; SetVDPAddr -- set the address for subsequent VDP writes.
;=============================================================================
SetVDPAddr	macro
	lda	#{1}
	sta	vmaddl
	lda	#({1}>8)
	sta	vmaddh
	endm



;==============================================================================
; DMASetup -- Do regs for a DMA.  Macro takes the following params:
; 	DMASetup	DMAx,VDP_Address,DataPointer(long),NumBytes(word)
;==============================================================================
DMASetup	macro
	pha
	LONGA	PUSH
	php

	A8
	lda	#$18			; VMDATAL
	sta	{1}+DMABBUS
	lda	#$01
	sta	{1}+DMAPARAM

	A16
	lda	#{2}
	sta	VMADDL			; store it

	lda	#{3}
	sta	{1}+DMAABUSL		; get HI and LO
	lda	#>{3}
	sta	{1}+DMAABUSH		; get BANK and redundant HI

	lda	#{4}
	sta	{1}+DMACOUNTL		; # of BYTES

	plp
	LONGA	POP
	pla
	endm




;=============================================================================
; VDPCopy -- copy a block of memory from the CPU memory to the VDP.
; setup:
;	use SetVDPAddr to set the VDP address for write.
;	START	pointer to data to copy
;	LENGTH	sixteen bit count # of WORDS (not bytes)
;=============================================================================
zpage	segment
start	ds	3
length	ds	2

code	segment
VDPCopy:
	php
	mode8
	blank
	mode16
	a8
	lda	#$80
	sta	vmainc
	a16
	lda	length
	tax
	ldy	#0
?loop:
;	lda	|0,x
	lda	[start],y
	sta	vmdatal		; sixteen bit write, will do both halves

	iny
	iny			; bump twice, because it's words
	dex			; dec # of words to copy
	bne	?loop		; go back for more

	plp
	rts

;=============================================================================
;=============================================================================
; FadeOut -- fade the screen using the FADE register, uses internal constants
; to fade the screen in 15 steps over one second
;==============================================================================

zpage	segment
FADELVL	ds	2

code	segment
FadeOut:
	php
	mode8
	lda	#$f
	sta	FADELVL

?loop:
	dec	FADELVL
	lda	FADELVL
	bmi	?gone
	sta	INIDISP
	lda	#4
	jsr	TimeDelay
	bra	?loop
?gone:
	plp
	rts

;==============================================================================
; FadeIn -- fade the screen in using the FADE register.
;==============================================================================

code	segment
FadeIn:
	php
	mode8
	lda	#$0
	sta	FADELVL
?loop:
	inc	FADELVL
	lda	FADELVL
	cmp	#$10
	beq	?gone
	sta	INIDISP
	lda	#4
	jsr	TimeDelay
	bra	?loop
?gone:
	plp
	rts

;==============================================================================
; PixelOut -- pixelate
;==============================================================================


code	segment
PixelOut:
	php
	mode8
	lda	#$0
	sta	FADELVL
?loop:
	inc	FADELVL
	lda	FADELVL
	cmp	#$10
	beq	?gone
	sta	MOSAIC
	lda	#4
	jsr	TimeDelay
	bra	?loop
?gone:
	plp
	rts

;=============================================================================
; PixelIn
;==============================================================================


code	segment
PixelIn:
	php
	mode8
	lda	#$f
	sta	FADELVL

?loop:
	dec	FADELVL
	lda	FADELVL
	bmi	?gone
	sta	MOSAIC
	lda	#4
	jsr	TimeDelay
	bra	?loop
?gone:
	plp
	rts


	end
