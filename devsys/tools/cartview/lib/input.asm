
;============================================================================
;
; File:    input.asm
;
; Project: SNES Library
;
; History:
;	20 Jul 92	WBNIV	Created
;
;============================================================================


;==============================================================================

ZPAGE	SEGMENT

Joy1Old		ds	2
Joy1Edge		ds	2
Joy1EdgeDown	ds	2

Joy2Old		ds	2
Joy2Edge		ds	2
Joy2EdgeDown	ds	2

JoyShadow		ds	2
JoyEdge		ds	2
JoyEdgeDown	ds	2


;============================================================================

CODE	SEGMENT

InputInit:
	PUSH_ALL
	mode16

	jsr	|JoystickInit

	stz	Joy1Old
	stz	Joy1Edge
	stz	Joy1EdgeDown

	stz	Joy2Old
	stz	Joy2Edge
	stz	Joy2EdgeDown

	stz	JoyShadow
	stz	JoyEdge
	stz	JoyEdgeDown

	PULL_ALL
	rts


GetJoyInput:
	PUSH_ALL
	mode16

	lda	Joy1Shadow
	sta	Joy1Old
	jsr	|ReadJoy1
	lda	Joy1Shadow
	eor	Joy1Old
	sta	Joy1Edge
	and	Joy1Shadow
	sta	Joy1EdgeDown
;;	lda	Joy1Shadow
;;	eor	#$FF
;;	and	Joy1Edge
;;	sta	Joy1EdgeUp

	lda	Joy2Shadow
	sta	Joy2Old
	jsr	|ReadJoy2
	lda	Joy2Shadow
	eor	Joy2Old
	sta	Joy2Edge
	and	Joy2Shadow
	sta	Joy2EdgeDown
;;	lda	Joy2Shadow
;;	eor	#$FF
;;	and	Joy2Edge
;;	sta	Joy2EdgeUp

	lda	Joy1Shadow
	ora	Joy2Shadow
	sta	JoyShadow

	lda	Joy1Edge
	ora	Joy2Edge
	sta	JoyEdge

	lda	Joy1EdgeDown
	ora	Joy2EdgeDown
	sta	JoyEdgeDown

;;	lda	Joy1EdgeUp
;;	ora	Joy2EdgeUp
;;	sta	JoyEdgeUp

	PULL_ALL
	rts


WaitJoy:
	PUSH_ALL

.10:	jsr	GetJoyInput
	lda	JoyEdgeDown
	beq	.10

	PULL_ALL
	rts


	END









