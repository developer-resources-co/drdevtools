
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
Joy2Shadow	ds	3


code	segment

	asm16

ReadJoy1:
	php
	a8
	ldx	#$0
	bra	ReadJoy

ReadJoy2:
	php
	a8
	ldx	#$2

ReadJoy:
	waitvb
	x16
	phx
	x8


	a8
	lda	#1
	sta	$4016
	lda	#0
	sta	$4016
	sta	Joy1Shadow
	sta	Joy1Shadow+1
	ldx	#8
?1:
	asl	Joy1Shadow
	lda	$4016
	and	#1
	ora	Joy1Shadow
	sta	Joy1Shadow
	dex
	bne	?1

	ldx	#8
?2:
	asl	Joy1Shadow+1
	lda	$4016
	and	#1
	ora	Joy1Shadow+1
	sta	Joy1Shadow+1
	dex
	bne	?2

	x16
	plx

	plp
	rts


ButtonRelease:			; enter with X = joystick offfset
	jsr	ReadJoy
	ora	#0
	bne	ButtonRelease
	rts

	end
