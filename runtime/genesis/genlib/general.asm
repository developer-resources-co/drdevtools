;==============================================================================
; general.asm: general purpose code
;==============================================================================

CODE	segment
	even

;==============================================================================
;===	Wait:
;=== 	Inputs:
;===		d0 = # of frames to wait
;==============================================================================

Wait:
	bra	.Entry
.Loop
	ifdef	SOUND
	jsr	SoundTick
	endif
	WAITVB
.Entry:
	dbra	d0,.Loop
	rts

;==============================================================================
;===	WaitJoyRndSeed: Abortable wait
;=== 	Inputs:
;===		d0 = # of frames to wait
;==============================================================================

WaitJoyRndSeed:
	move.w	d0,d2
	bra.s	.Entry

.Loop:
	addq.l	#3,randSeed
	WAITVBSOUND
.Entry:
	jsr	ReadJoy1
	tst.b	d0
	bne.s	.Done
	jsr	ReadJoy2
	tst.b	d0
	bne.s	.Done

	dbra	d2,.Loop
.Done:
	rts

;==============================================================================
;	Inputs:
;		a0-> source memory
;		a1-> destination memory
;		d0.w  = # of bytes to copy
;	Destroys:
;		d0.w
;		a0->just after source memory
;		a1->just after dest memory
;------------------------------------------------------------------------------

MemCopyLoop:
	move.b	(a0)+,(a1)+
MemCopy:
	dbra	d0,MemCopyLoop
	rts

;==============================================================================
;	Inputs:
;		a0-> source memory
;		a1-> destination memory
;		d0.w  = # of bytes to copy
;	Destroys:
;		d0.w
;------------------------------------------------------------------------------

ReverseMemCopy:
	add.w	d0,a0
	add.w	d0,a1
	bra	.Entry
.Loop:
	move.b	-(a0),-(a1)
.Entry
	dbra	d0,.Loop
	rts

;==============================================================================

	END

;==============================================================================
