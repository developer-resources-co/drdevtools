;==============================================================================
;===	math.asm: 68000 math and number routines			    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================

	opt	llbl

	TSECTION	.text

	ds.w	1	;even

	xdef	_RandomNum
	xdef	RandomNum
	xdef	randSeed

; 16 bit psuedo random:
;	*13
;	+1

;==============================================================================

_RandomSeed:
	move.w	6(sp),randSeed
	rts

;==============================================================================
;	RandomNum:
;		Written By Bruce Q. Hammond
;	Inputs:
;		randSeed should get randomized at beginning of game
;		(while waiting for joystick input, for example)
;	Outputs:
;		d0.w = random #
;	Destroys:
;		d0.w,d1.w,d2.w
;------------------------------------------------------------------------------

_RandomNum:					; kts 03-05-93 01:33pm
RandomNum:
	move.l	randSeed,d0
	add.w	$C00008,d1
	ror.l	#2,d0
	sub.l	d1,d0
	mulu	#$5641,d0
	move.l	d0,randSeed
	rts

;==============================================================================
; kts this is not used

	move.w	randSeed+2,d0
	move.w	randSeed,d1
;	move.b	randSeed+2,d2
;	lsl.w	#1,d1
	add.w	d1,d1				; new

;	move.b	d2,d1
	move.b	randSeed+2,d1			; new

	move.w	d0,d2
	rol.w	#8,d2
;	and.w	#$ff00,d2
	clr.b	d2				; new
	or.w	d2,d1

	roxl.w	#1,d0
	eor.w	d1,d0
	cmp.w	randSeed,d0
	bne.s	.Ok
	roxl.w	#1,d0
	eor.w	d1,d0
.Ok:
	move.w	d0,randSeed
	rts

;==============================================================================
; return a # between d0 & d1
;	Inputs:
;		d0 = lower end of range
;		d1 = upper end of range
;------------------------------------------------------------------------------
; kts don't use this, it's stupid

RandomRange:
	move.w	d0,d3
	sub.w	d0,d1			; round down to zero
	move.w	d1,d4

	jsr	RandomNum
.Loop:
	cmp.w	d0,d4
	bcc.s	.Ok
	sub.w	d0,d4
	bra.s	.Loop
.Ok:
	add.w	d3,d0			; scale back up to range
	rts

;==============================================================================

	BSECTION	.bss
randSeed	ds.l	1

;==============================================================================

	END

;==============================================================================

