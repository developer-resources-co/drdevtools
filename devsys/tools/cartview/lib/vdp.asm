;==============================================================================
;	vdp.asm: should be called ppu.asm, SNES PPU functions
;==============================================================================

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


	IFDEF	WALDO
;==============================================================================
;	Inputs:
;		a = starting palette #
;		x = # of colors
;		y = # of frames to delay between steps
;		lpSrcBuffer	->	palette 1
;		lpSrc2Buffer	->	palette 2
;		lpDestBuffer	->	target(storage)
;------------------------------------------------------------------------------

CrossFade
	PUSH_ALL
	mode16

	sta	palOff				; remember starting entry #
	sty	lpTemp				; remember delay value

	lda	lpDestBuffer
	sta	lpPalette			; for SetPalette

	ldy	#0				; current cross-fade value
.Loop:
	pushz	lpSrcBuffer
	pushz	lpSrc2Buffer
	pushz	lpDestBuffer
	tya
	jsr	SetCrossFade
	popz	lpDestBuffer
	popz	lpSrc2Buffer
	popz	lpSrcBuffer

	lda	lpTemp
	jsr	TimeDelay       		; assume returns at beginning
						; of vblank

	phy
	ldy	palOff				; x is preserved
	;blank
	jsr	NewSetPalette
	;unblank
	ply

	iny
	cpy	#$21
	bne	.Loop

	PULL_ALL
	rts


;==============================================================================
;	Inputs:
;		a = current cross fade value(0 = all palette 1, $1f = all palette2)
;		x = # of colors
;		lpSrcBuffer	->	palette 1
;		lpSrc2Buffer	->	palette 2
;		lpDestBuffer	->	target(storage)
;	Restrictions:
;		buffers cannot cross banks
;------------------------------------------------------------------------------

SetCrossFade:
DoPal	macro
					; do red for palette 1
	lda	[{1}],y
	and	#$1f
	ldx	{2}
	jsr	Multiply
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	and	#$1f
	sta	{3}

					; do green for palette 1
	lda	[{1}],y
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	and	#$1f
	ldx	{2}
	jsr	Multiply
	and	#$1f<5
	ora	{3}
	sta	{3}
					; do blue for palette 1
	lda	[{1}],y
	xba
	lsr	a
	lsr	a
	and	#$1f
	ldx	{2}
	jsr	Multiply
	asl	a
	asl	a
	asl	a
	asl	a
	asl	a
	and	#$1f<10
	ora	{3}
	sta	{3}
	endm

	PUSH_ALL
	mode16
	sta	fadeVal
	lda	#$20
	sec
	sbc	fadeVal
	sta	invFadeVal
	ldy	#0
.Loop:
	dex
	bpl	.Entry
	PULL_ALL
	rts
.Entry
; meat
	phx
					;  doesn't assemble in general
	DoPal	lpSrcBuffer,invFadeVal,palTemp
	DoPal	lpSrc2Buffer,fadeVal,palTemp2
	lda	palTemp
	clc
	adc	palTemp2
	sta	[lpDestBuffer],y				; save out new color
	plx
; end meat
	iny
	iny
	brl	.Loop

;==============================================================================
; handy for fading to or from black

blackPalette:
	dw	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dw	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dw	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dw	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dw	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dw	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dw	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	dw	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

	ENDIF


;==============================================================================
; VDPView:
; look at the VDP
;
;==============================================================================


VDPView:
	php
	a8
	x16
	blank
;;	lda	#$00
;;	sta	BG12NBA
;;	lda	#1
;;	sta	TM
	a16
	lda     #$1800
	sta     VMADDL

	ldy     #1023
	lda     #0
.loop:
	sta     VMDATAL

	inc	a
	dey
	bne     .loop

	unblank
	plp
	brk

	mode8

;=============================================================================
;=============================================================================

;=============================================================================
; SetVDPAddr -- set the address for subsequent VDP writes.
;=============================================================================
SetVDPAddr      macro
	lda     #{1}
	sta     vmaddl
	lda     #({1}>8)
	sta     vmaddh
	endm



;==============================================================================
; DMASetup -- Do regs for a DMA.  Macro takes the following params:
;       DMASetup        DMAx,VDP_Address,DataPointer(long),NumBytes(word)
;==============================================================================
DMASetup        macro
	pha
	LONGA   PUSH
	php

	A8
	lda     #$18                    ; VMDATAL
	sta     {1}+DMABBUS
	lda     #$01
	sta     {1}+DMAPARAM

	A16
	lda     #{2}
	sta     VMADDL                  ; store it

	lda     #{3}
	sta     {1}+DMAABUSL            ; get HI and LO
	lda     #>{3}
	sta     {1}+DMAABUSH            ; get BANK and redundant HI

	lda     #{4}
	sta     {1}+DMACOUNTL           ; # of BYTES

	plp
	LONGA   POP
	pla
	endm


;=============================================================================
; VDPCopy -- copy a block of memory from the CPU memory to the VDP.
; setup:
;       use SetVDPAddr to set the VDP address for write.
;       START   pointer to data to copy
;       LENGTH  sixteen bit count # of WORDS (not bytes)
;=============================================================================
zpage   segment
start   ds      3
length  ds      2

code    segment
VDPCopy:
	PUSH_ALL

	mode16
	a8
	lda     #$80
	sta     vmainc
	a16
	lda     length
	tax
	ldy     #0
.loop:
;       lda     |0,x
	lda     [start],y
	sta     vmdatal         ; sixteen bit write, will do both halves
	iny
	iny                     ; bump twice, because it's words
	dex                     ; dec # of words to copy
	bne     .loop           ; go back for more

	PULL_ALL
	rts

VDPCopy2:
	PUSH_ALL

	mode16
	lda     length
	tax
	ldy     #0
.loop:
;       lda     |0,x
	lda     [start],y
	sta     vmdatal         ; sixteen bit write, will do both halves
	iny
	iny                     ; bump twice, because it's words
	dex                     ; dec # of words to copy
	bne     .loop           ; go back for more

	PULL_ALL
	rts

;=============================================================================
; FadeOut -- fade the screen using the FADE register, uses internal constants
; to fade the screen in 15 steps over one second
;
FadeOut:
	PUSH_ALL
	mode8

	IFDEF	FADE
	ldx	#$0F
.loop:
	stx	INIDISP
	lda	#3
	jsr	TimeDelay
	dex
	bpl	.loop
	ENDIF

	lda	#%10000000
	sta	INIDISP

	PULL_ALL
	rts

;==============================================================================
; FadeIn -- fade the screen in using the FADE register.
;
FadeIn:
	PUSH_ALL
	mode8

	IFNDEF	FADE
	lda	#%00001111		; Full intensity
	sta	INIDISP
	ENDIF

	IFDEF	FADE
	ldx	#$00
.loop:
	stx	INIDISP
	lda	#3
	jsr	TimeDelay
	inx
	cpx	#$0F
	blt	.loop
	ENDIF

	PULL_ALL
	rts

;==============================================================================
; PixelOut -- pixelate
;==============================================================================

	IF 0

code    segment
PixelOut:
	php
	mode8
	lda     #$0
	sta     FADELVL
.loop:
	inc     FADELVL
	lda     FADELVL
	cmp     #$10
	beq     .gone
	sta     MOSAIC
	lda     #4
	jsr     |TimeDelay
	bra     .loop
.gone:
	plp
	rts

;=============================================================================
; PixelIn
;==============================================================================


code    segment
PixelIn:
	php
	mode8
	lda     #$f
	sta     FADELVL

.loop:
	dec     FADELVL
	lda     FADELVL
	bmi     .gone
	sta     MOSAIC
	lda     #4
	jsr     |TimeDelay
	bra     .loop
.gone:
	plp
	rts

	ENDIF


	end
