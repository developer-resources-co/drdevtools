
;============================================================================
;
; File:    random.asm 
;
; Project: SNES Library Source Code
;
; History: 
;       17 May 92       WBNIV   Created
;	13 Jun 92	WBNIV	Added to library
;
;============================================================================


ZPAGE   SEGMENT
rnd_seed        ds      2
rndPtr		ds	2
rndTemp0	ds	2
rndCount	ds	2

;============================================================================

CODE    SEGMENT

	asm16
randomize:
	php

	IFDEF	USER_RANDOMIZE
	a8
	lda	$2137				; Latch H/V counter

	lda	$213C
	xba
	lda	$213C
	xba
	ENDIF

	IFNDEF	USER_RANDOMIZE
	a16
	lda	#0
	a8			; To simulate flags as per above
	ENDIF

	plp
srand:
	sta	rnd_seed
	rts


	asm16
rand:
	phx

	lda	rnd_seed
	ldx	#13
	jsr	Multiply
	inc	a
	sta	rnd_seed

	IFDEF	USER_RANDOMIZE
	php
	a8
	lda	$2137				; Latch H/V counter

	lda	$213C
	xba
	lda	$213C
	xba

	plp
	ENDIF

	ora	rnd_seed
	sta	rnd_seed

	plx
	rts


	asm16
random:
	phx
	
	tax
	jsr     rand
	jsr     Divide
	txa

	plx
	rts

;
; RandList - Creates
;
; Inputs:
;	A: Pointer to array
;	Y: Number of items to fill
;	X: Starting number (CODE)
;
randlist:
	PUSH_ALL
	mode16

	sta     rndPtr
	tyx
	tya
	sta     rndTemp0
	asl     a
	tay
	dey
	dey
	txa
	dec     a

.10
	sta     (rndPtr),y

	dec     a
	dey
	dey
	bpl     .10


	lda     rndTemp0
	dec     a
	asl     a
	sta     rndCount
.20
	lda     rndTemp0
	jsr     random
	asl     a
	tay
	lda     (rndPtr),y
	phy
	pha

	lda     rndTemp0
	jsr     random
	asl     a
	tay
	lda     (rndPtr),y
	tax

	pla
	sta     (rndPtr),y
	ply
	txa
	sta     (rndPtr),y

	dec     rndCount
	dec     rndCount
	bpl     .20


	PULL_ALL
	rts


	END

