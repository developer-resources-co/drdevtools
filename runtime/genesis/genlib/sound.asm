;==============================================================================
;===	sound.asm: 68000 sound routines					    ===
;===	By Kevin T. Seghetti						    ===
;===	(c)1991 Developer Resources					    ===
;==============================================================================

code	segment
	even

;==============================================================================
;===	Inputs:	none							    ===
;===	Turns all psg sound off						    ===
;==============================================================================

InitPSG:
	move.w	#4-1,d3
.Loop:
	clr.b	d1
	move.b	d3,d0
	bsr.s	SetPSGVolume
	dbra	d3,.Loop
	rts

;==============================================================================
;===	Inputs:								    ===
;===		d0.b = channel #(0-2)					    ===
;===		d1.w = new channel pitch(0-$3ff)			    ===
;===	Destroys:							    ===
;==============================================================================

SetPSGPitch:
	ror.b	#3,d0			; place channel # in correct place
	move.w	d1,d2
	lsr.w	#6,d2
	or.b	d2,d0
	or.b	#$80,d0
	move.b	d0,PSG
	and.b	#$3f,d1
	move.b	d1,PSG			; write out second pitch byte
	rts

;==============================================================================
;===	Inputs:								    ===
;===		d0.b = channel #(0-3)					    ===
;===		d1.b = new channel volume(0-f)				    ===
;==============================================================================

SetPSGVolume:
	ror.b	#3,d0			; place channel # in correct place
	or.b	#$90,d0			; set control bits
	move.w	#$f,d2
	sub.w	d1,d2			; invert volume(so 0 = silence, $F = full)
	or.b	d2,d0
	move.b	d0,PSG
	rts

;==============================================================================
;===	Inputs:								    ===
;===		d0.b = psg noise bits(see equates)			    ===
;==============================================================================

SetPSGNoise:
	or.b	#$e0,d0
	move.b	d0,PSG
	rts

;==============================================================================

	END

;==============================================================================
