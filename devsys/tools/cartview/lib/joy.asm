
;============================================================================
;
; File:    joy.asm
;
; Project: SNES Library
;
; History:
;	   ???		SLS	Created
;	20 Jul 92	WBNIV	Added code for init, two joysticks, cleaned up
;
;============================================================================

;==============================================================================
; JOY.ASM -- Joystick routines for SNES.
;==============================================================================

JOYRIGHT	EQU	$1
JOYLEFT		EQU	$2
JOYDOWN		EQU	$4
JOYUP		EQU	$8
JOYSTART	EQU	$10
JOYSELECT	EQU	$20
JOYYBUTTON	EQU	$40
JOYBBUTTON	EQU	$80

JOYRBUTTON	EQU	$1000
JOYLBUTTON	EQU	$2000
JOYXBUTTON	EQU	$4000
JOYABUTTON	EQU	$8000


zpage	segment

Joy1Shadow	ds	2
Joy2Shadow	ds	2


code	segment

	asm16

; mode16
JoystickInit:
	stz	Joy1Shadow
	stz	Joy2Shadow
	rts

;
; Joystick routines must be called during Vertical Blank!
;
ReadJoy1:
	PUSH_ALL
	a16
	stz	Joy1Shadow

	a8
	lda	#1
	sta	$4016
	lda	#0
	sta	$4016

	ldx	#8
.1:
	asl	Joy1Shadow
	lda	$4016
	and	#1
	ora	Joy1Shadow
	sta	Joy1Shadow
	dex
	bne	.1

	ldx	#8
.2:
	asl	Joy1Shadow+1
	lda	$4016
	and	#1
	ora	Joy1Shadow+1
	sta	Joy1Shadow+1
	dex
	bne	.2

	;----- Total reset?
	a16
	lda	Joy1Shadow
	cmp	# (JOYSTART | JOYSELECT | JOYLBUTTON | JOYRBUTTON)
	bne	.3
	a8
	lda	#0
	sta	NMITIMEN
	jml	bootstrap
.3
;;;	jsr	randomize	; Trashes A -- doesn't matter at this point

	PULL_ALL
	rts


ReadJoy2:
	PUSH_ALL
	a16
	stz	Joy2Shadow

	a8
	lda	#1
	sta	$4017
	lda	#0
	sta	$4017

	ldx	#8
.1:
	asl	Joy2Shadow
	lda	$4017
	and	#1
	ora	Joy2Shadow
	sta	Joy2Shadow
	dex
	bne	.1

	ldx	#8
.2:
	asl	Joy2Shadow+1
	lda	$4017
	and	#1
	ora	Joy2Shadow+1
	sta	Joy2Shadow+1
	dex
	bne	.2

	;----- Total reset?
	a16
	lda	Joy2Shadow
	cmp	# (JOYSTART | JOYSELECT | JOYLBUTTON | JOYRBUTTON)
	bne	.3
	a8
	lda	#0
	sta	NMITIMEN
	jml	bootstrap
.3

	PULL_ALL
	rts


	END

