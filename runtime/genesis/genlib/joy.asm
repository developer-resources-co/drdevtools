;============================================================================
;===	joy.asm: Genesis joystick routines				  ===
;===	(c)1991,92 Developer Resources					  ===
;============================================================================
;	Purpose:
;		provide an interface to the user joysticks.
;------------------------------------------------------------------------------

code	segment

;==============================================================================
;	Inputs:
;		none
;	Outputs:
;		d0.b = joystick bits
;	Destroys:
;		d0.b,d1.b,a0.l
;------------------------------------------------------------------------------

;============================================================================
;	See Genesis.inc for joystick equates
;=== Bit 0 = up / Bit 1 = Down / Bit 2 = Left / Bit 3 = Right
;=== Bit 4 = B  / Bit 5 = C    / Bit 6 = A    / Bit 7 = START
;------------------------------------------------------------------------------

GetJoy1
ReadJoy1:
		MOVE.B	#$40,$A10009		; DDR Port 1 Joystick
		lea	$A10003,A0
		bra.s	GetJoy

GetJoy2
ReadJoy2:
		lea	$A10005,A0
		MOVE.B	#$40,$A1000B		; DDR Port 2 Joystick

GetJoy		bclr	#6,(A0)			; Set Bank 0
		nop
		nop
		move.b	(A0),D1
		and.b	#%00110000,D1
		asl.b	#2,D1
		bset	#6,(A0)			; Set Bank 1
		nop
		nop
		move.b	(A0),D0
		and.b	#%00111111,D0
		or.b	D1,D0
		not.b	D0
		rts

;==============================================================================

ButtonReleaseBoth
	ifdef	SOUNDDRIVER
	WAITVBSOUND
	endif
	clr.l	d0
	bsr.s	GetJoyBoth
	tst.l	d0
	bne.s	ButtonReleaseBoth
	rts

;==============================================================================

ButtonRelease:
	ifdef	SOUNDDRIVER
	WAITVBSOUND
	endif

	clr.l	d0
	bsr	GetJoy1
	tst.l	d0
	bne.s	ButtonRelease
	rts

;==============================================================================
;	Destroys:
;		d0.b,d1.b,d2.b,a0.l
;------------------------------------------------------------------------------

GetJoyBoth:
	bsr	GetJoy1
	move.b	d0,d2
	bsr	GetJoy2
	or.b	d2,d0
	rts

;==============================================================================

WaitForButton1
	clr.l	d0
	bsr	GetJoy1
	tst.l	d0
	beq.s	WaitForButton1
	rts

;==============================================================================
;===	WaitJoy: Abortable wait						    ===
;===	Wait for # of frames, unless user presses button		    ===
;=== 	Inputs:                                                             ===
;===		d0 = # of frames to wait                                    ===
;==============================================================================

WaitJoy:
	move.w	d0,d3
	bra.s	.Entry

.Loop:
	WAITVBSOUND
.Entry:
	jsr	GetJoyBoth
	tst.b	d0
	dbne	d3,.Loop
.Done:
	rts

;============================================================================
;	ButtonDispatch: Automated button handling
;	(see gameobj.asm for example of use)
;	Inputs:
;		a0-> list of ptrs to routines
;		d0 = joystick input
; Bit 0 = up / Bit 1 = Down / Bit 2 = Left / Bit 3 = Right
; Bit 4 = B  / Bit 5 = C    / Bit 6 = A    / Bit 7 = START
; destroys:
;	d6-d7.w,a0-a1.l
;------------------------------------------------------------------------------

ButtonDispatch:			; speed is not that important here
	move.w	#8-1,d7
	move.w	d0,d6
.Loop:
	move.l	(a0)+,a1
	roxr.b	#1,d6
	bcc.s	.Nope
	movem.l	d6/d7/a0,-(sp)
	jsr	(a1)
	movem.l	(sp)+,d6/d7/a0
.Nope:
	dbra	d7,.Loop
	rts

;============================================================================

	END

;============================================================================
