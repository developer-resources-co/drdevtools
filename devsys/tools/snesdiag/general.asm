;==============================================================================
; GENERAL.ASM -- Misc routines.
;==============================================================================




zpage	segment
mcand1	ds	2
mcand2	ds	2

code	segment
Multiply:
	php
	mode16
	lda	#0
?1:
	ldx	mcand1
	beq	?3
	lsr	mcand1
	bcc	?2
	clc
	adc	mcand2

?2:
	asl	mcand2
	bra	?1
?3:
	plp
	rts







TimeDelay:
	php
	mode8
?loop:
	waitvb
	dec	a
	bne	?loop
	plp
	rts


WaitSeconds:
	php
	mode8
?loop:
	pha
	lda	#60
	jsr	TimeDelay
	pla
	dec	a
	bne	?loop
	plp
	rts

	end

