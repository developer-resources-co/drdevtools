;============================================================================
;
; File:    math.asm
;
; Project: SNES Library
;
; History:
;       10 May 92       WBNIV   Created
;	11 Jul 92	WBNIV	Added 8x8 signed multiply
;
;============================================================================


;============================================================================

SRAM	SEGMENT
bSigned		ds	2

;============================================================================

CODE	SEGMENT

	LONGA	ON
	LONGI	ON

MultiplySigned:
	pha
	phx

	stx	bSigned
	eor	bSigned
	sta	bSigned

	pla			; x value
	bpl	.10
	eor	#$FFFF
	inc	a
.10
	tax

	pla			; a value
	bpl	.20
	eor	#$FFFF
	inc	a
.20

	jsr	Multiply
	tay

	lda	bSigned
	and	# (1<8)
	beq	.clear_msb

	tya
	eor	#$FFFF
	inc	a
	tay
	sec
	bra	.exit

.clear_msb
	clc

.exit
	tya
	rts

Multiply:
	php

	mode8
	stx     $4202
	sta     $4203
	mode16                  ; 3 cycles
	mode16			; 3 cycles
	nop			; 2 
				; 8 cycles total
	lda     $4216
	plp
	rts

;==============================================================================
;
; MODE16: A=DIVIDEND, X=DIVISOR
;
;
;------------------------------------------------------------------------------

Divide:
	php

	sta     $4204
	x8
	stx     $4206
	x16				; 3
	nop				; 2
	nop				; 2
	nop				; 2
	nop				; 2
	nop				; 2
	ldx     #0                      ; 3, Ensure x returns a valid 8-bit
					; 16 cycles total
	lda     $4214
					;  and 16-bit number by zeroing
					;  all of x in 16-bit mode
	x8
	ldx     $4216
	x16

	plp
	rts

;==============================================================================

	END

;==============================================================================

