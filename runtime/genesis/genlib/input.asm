;==============================================================================
;===	Input.asm: high level input handling				    ===
;===	This library is under construction				    ===
;===									    ===
;==============================================================================

Input_Struct	defs	ram
Input_Struct	segment
	org	0
Input_JoyPtr	ds.l	1			; pointer to joystick routine to call
Input_OldJoy	ds.b	1       		; last joystick input
Input_Edge	ds.b	1			; bits that changed since last call
Input_DownEdge	ds.b	1			; bits that have gone on since last call
Input_UpEdge	ds.b	1			; bits that have gone on since last call
Input_Size

;==============================================================================

code	segment

;==============================================================================
;=== InitInput: 							    ===
;=== 	Inputs:								    ===
;===		a0-> input structure					    ===
;===		a1-> joystick routine to use				    ===
;==============================================================================

InitInput:
	move.l	a1,Input_JoyPtr(a0)
	rts

;==============================================================================
;=== ReadDigitalInput: return joystick compatible input bits immediately    ===
;=== 	Inputs:								    ===
;===		a0-> input structure					    ===
;===									    ===
;===									    ===
;==============================================================================

ReadDigitalInput:
	move.l	a1,-(sp)
	move.l	Input_JoyPtr(a0),a1
	move.l	a0,-(sp)
	jsr	(a1)
	move.l	(sp)+,a0
	move.b	Input_OldJoy(a0),d1
	eor.b	d0,d1				; generate edge
	move.b	d1,Input_Edge(a0)
	move.b	d1,Input_UpEdge(a0)

	and.b	d0,d1				; generate down edge
	move.b	d1,Input_DownEdge(a0)
	move.b	d0,d2
	eor.b	#$ff,d2
	and.b	d2,Input_UpEdge(a0)

	move.b	d0,Input_OldJoy(a0)
	; this is all for now, other input handling goes here
	move.l	(sp)+,a1
	rts

;==============================================================================
;=== GetDigitalInput:                         				    ===
;===	Waits for joystick input, then returns joystick compatible input    ===
;===									    ===
;=== 	Inputs:								    ===
;===		a0-> input structure					    ===
;===	Outputs:		    					    ===
;===		d0.b = joystick bits					    ===
;==============================================================================

GetDigitalInput:
	move.l	a0,a2
.Loop:
	WAITVBSOUND
	bsr	ReadDigitalInput
	tst.b	d0
	beq.s	.Loop

	; this is all for now, other input handling goes here
	rts

;==============================================================================
;=== InputRelease:                         				    ===
;===	Waits for all inputs to be released				    ===
;===									    ===
;=== 	Inputs:								    ===
;===		a0-> input structure					    ===
;==============================================================================

InputRelease:
	move.l	a0,a2
	bra	.Entry
.Loop:
	WAITVBSOUND
.Entry
	move.l	Input_JoyPtr(a2),a1
	jsr	(a1)
	tst.b	d0
	bne.s	.Loop
	; this is all for now, other input handling goes here
	rts

;==============================================================================

	end

;==============================================================================
