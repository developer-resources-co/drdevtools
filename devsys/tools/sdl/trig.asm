;============================================================================
; trig.asm was spheremath.asm
;============================================================================

	XDEF	cosSin
	EVEN

	include	sincos.dat

;============================================================================

	asm16
cosSin
;;;sinCos( theta )
;;;; ENTRY d0.w contains theta
;;;; EXIT HiWord( d0.L ) is sin( theta )      LoWord( DO.L ) is cos( theta )
	lda	d0+2		; check if positive
	bpl	.positive
;	eor	#$ffff
;	inc
	neg68w	d0
.negative
.again
	cmp68w	#359,d0
	bcs	.getVals
	sub68w	#360,d0
	bra	.again
.getVals
	neg68w	d0
	add68w	#360,d0
	bra	.getValues

.positive
.tryAgain
	cmp68w	#359,d0
	bcs	.getValues
	sub68w	#360,d0
	bra	.tryAgain

.getValues
	lda	d0
	asl
	asl
	tax
	lda	sinCos,x
	sta	d0
	lda	sinCos+2,x
	sta	d0+2
	rts

;============================================================================
	end
;============================================================================
