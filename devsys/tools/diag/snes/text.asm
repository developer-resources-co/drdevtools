;==============================================================================
; Text routines
;==============================================================================

code	segment

;==============================================================================
; Print --- Print an ascii string.  No control chars yet.
;	Y = pointer to string
;	X = position on screen
;==============================================================================

PRINT:
	php
	mode16
	txa
	clc
	adc	#$7800
	sta	vmaddl
	a8
	lda	#$80
	sta	vmainc
	a8
?1:
	lda	|0,y
	beq	?2
	sec
	sbc	#$20
	sta	vmdatal
	lda	#0
	sta	vmdatah
	iny
	bra	?1
?2:
	plp
	rts


;==============================================================================
; Prhex --- Print HEX at a given location.
; 	A = number to print
;	X = position on screen
; X & Y preserved
;==============================================================================

PRHEX:
	php
	mode16
	pha
	txa
	clc
	adc	#$7800	; base of screen 3
	sta	vmaddl	; set up to write
	a8
	lda	#$80
	sta	vmainc
	lda	2,s	;
	and	#$f0
	lsr	a
	lsr	a
	lsr     a
	lsr     a
	clc
	adc	#$10
	cmp	#$1a
	bcc	?1
	adc	#6
?1:
	sta	vmdatal
	lda	#0
	sta	vmdatah
	lda	2,s
	and	#$f
	clc
	adc	#$10
	cmp	#$1a
	bcc	?2
	adc	#6
?2:
	sta	vmdatal
	lda	#0
	sta	vmdatah
	lda	1,s	;
	and	#$f0
	lsr     a
	lsr     a
	lsr     a
	lsr     a
	clc
	adc	#$10
	cmp	#$1a
	bcc	?3
	adc	#6
?3:
	sta	vmdatal
	lda	#0
	sta	vmdatah
	lda	1,s
	and	#$f
	clc
	adc	#$10
	cmp	#$1a
	bcc	?4
	adc	#6
?4:
	sta	vmdatal
	lda	#0
	sta	vmdatah
	a16
	pla
	plp
	rts

	end
