;============================================================================
;
; File:    decomp.asm
;
; Project: SNES Library
;
; History:
;       11 May 92       WBNIV   Created
;
;============================================================================


;============================================================================

ZPAGE   SEGMENT
nDecompModulus  ds      2
nDecompLoop	ds	2
lpSrcBuffer     ds      3
lpSrc2Buffer	ds      3
lpDestBuffer    ds      3
lpTemp		ds	3


;============================================================================
; rle format:
; 1xxx xxxx,n  = repeat n x times
; 0xxx xxxx    = there are x unique bytes following
; 00  = run finished
;==============================================================================
;      char file format:
;
; struct hdr
; {
;        UWORD length;
;        UBYTE compType;
;	 UBYTE compTypeData;
; }
; followed by the char data(in whatever compression is specified)
;
; enum
; {
;        compType_none,
;        compType_RLE
; }
;

COMP_HEADER	DEFS	RAM
COMP_HEADER	SEGMENT

comp_header_length	ds	2
comp_header_type	ds	1
comp_header_data	ds	1
COMP_HEADER_SIZEOF


;============================================================================

CODE	SEGMENT

;
; lpSrcBuffer
; lpDestBuffer
;
Decomp_RLE_Delta:
	PUSH_ALL
	mode16

	a8
	phb
	lda	lpDestBuffer+2
	pha
	plb
	a16

	;----- Save original lpDestBuffer
	lda	lpDestBuffer
	sta	lpTemp


	lda     #0
	sta	nDecompLoop

	a8
	ldy     #comp_header_data
	lda     [lpSrcBuffer],y
	a16
	sta     nDecompModulus          ; Do this many passes accross the data!

	clc
	lda	#COMP_HEADER_SIZEOF	; beginning of data
	adc	nDecompLoop
	tay

.10
	jsr	DecompPass_RLE_Delta

	inc	nDecompLoop

	clc
	lda	lpTemp
	adc	nDecompLoop
	sta	lpDestBuffer

	lda	nDecompLoop
	cmp	nDecompModulus
	bne	.10

	plb

	PULL_ALL
	rts


DecompPass_RLE_Delta:
	php
	a8

.decomp_loop:
	lda     [lpSrcBuffer],y
	beq     .decomp_exit             ; Code $00 == all done
	bmi     .decomp_repeat

.decomp_unique:
	iny
	tax                             ; # of bytes to transfer
	dex                             ;  -1 since test on positive (>=0)
.10     lda     [lpSrcBuffer],y
	iny
	cmp	#$FF
	beq	.11
	sta     (lpDestBuffer)
.11
	a16
	clc                             ; Increment destination buffer
	lda     lpDestBuffer            ;  based on modulus
	adc     nDecompModulus
	sta     lpDestBuffer
	lda	#0
	a8
	dex
	bpl     .10
	bra     .decomp_loop

.decomp_repeat:
	iny
	and     #%01111111              ; strip negative high bit
	tax                             ; # of bytes to transfer
	dex                             ;  -1 since test on positive (>=0)
	lda     [lpSrcBuffer],y
	iny
	phy				; Use y as temp copy of a
	tay
.20
	cmp	#$FF
	beq	.21
	sta     (lpDestBuffer)
.21
	a16
	clc
	lda     lpDestBuffer
	adc     nDecompModulus
	sta     lpDestBuffer
	lda	#0
	a8
	tya
	dex
	bpl     .20
	ply
	bra     .decomp_loop

.decomp_exit:
	iny
	plp
	rts


;
; lpSrcBuffer
; lpDestBuffer
;
Decomp_RLE:
	PUSH_ALL
	mode16

	a8
	phb
	lda	lpDestBuffer+2
	pha
	plb
	a16

	;----- Save original lpDestBuffer
	lda	lpDestBuffer
	sta	lpTemp


	lda     #0
	sta	nDecompLoop

	a8
	ldy     #comp_header_data
	lda     [lpSrcBuffer],y
	a16
	sta     nDecompModulus          ; Do this many passes accross the data!

	clc
	lda	#COMP_HEADER_SIZEOF	; beginning of data
	adc	nDecompLoop
	tay

.10
	jsr	DecompPass_RLE

	inc	nDecompLoop

	clc
	lda	lpTemp
	adc	nDecompLoop
	sta	lpDestBuffer

	lda	nDecompLoop
	cmp	nDecompModulus
	bne	.10

	plb
	PULL_ALL
	rts


DecompPass_RLE:
	php
	a8

.decomp_loop:
	lda     [lpSrcBuffer],y
	beq     .decomp_exit             ; Code $00 == all done
	bmi     .decomp_repeat

.decomp_unique:
	iny
	tax                             ; # of bytes to transfer
	dex                             ;  -1 since test on positive (>=0)
.10     lda     [lpSrcBuffer],y
	iny
	sta     (lpDestBuffer)
	a16
	clc                             ; Increment destination buffer
	lda     lpDestBuffer            ;  based on modulus
	adc     nDecompModulus
	sta     lpDestBuffer
	lda	#0
	a8
	dex
	bpl     .10
	bra     .decomp_loop

.decomp_repeat:
	iny
	and     #%01111111              ; strip negative high bit
	tax                             ; # of bytes to transfer
	dex                             ;  -1 since test on positive (>=0)
	lda     [lpSrcBuffer],y
	iny
	phy				; Use y as temp copy of a
	tay
.20     
	sta     (lpDestBuffer)
	a16
	clc
	lda     lpDestBuffer
	adc     nDecompModulus
	sta     lpDestBuffer
	lda	#0
	a8
	tya
	dex
	bpl     .20
	ply
	bra     .decomp_loop

.decomp_exit:
	iny
	plp
	rts


	END

