;==============================================================================
; GENERAL.ASM -- Misc routines.
;==============================================================================




;==============================================================================

zpage	segment
mcand1	ds	2
mcand2	ds	2


;==============================================================================

SRAM	SEGMENT

nJoyOverride	ds	2
bPauseBreakable	ds	2		; 0 (FALSE) or $FF (TRUE)


;==============================================================================

code	segment

TimeDelay:
	php
	a8

.loop:
	waitvb
	dec	a
	bne	.loop

	plp
	rts


	IFDEF	WALDO
TimeAdvance:
	php
	a16

	stz	nJoyOverride

.loop:
	waitvb

	pha

	jsr	|ReadJoy1
	lda	Joy1Shadow
	ora	nJoyOverride
	sta	nJoyOverride

	lda	bPauseBreakable
	beq	.10

	lda	Joy1Shadow
	beq	.10

	IFDEF	WALDO
	jsr	Tone
	ENDIF
	pla
	bra	.90

.10
	pla

	dec	a
	bne	.loop

.90
	plp
	rts
	ENDIF


WaitSeconds:
	php
	mode8
.loop:
	pha
	lda	#60
	jsr	|TimeDelay
	pla
	dec	a
	bne	.loop
	plp
	rts

	end
