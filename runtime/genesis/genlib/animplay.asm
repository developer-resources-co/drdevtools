;==============================================================================
;=== animplay.asm: simple animation player				    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================
;	Purpose: allow user to render rectangular animations by simple
;		 sequencing of maps(all chars for all the animation frames
;		 must reside in the vdp)
;
;	Use:
;		First call InitAnim to set up the animation structure.
;			To cause an anim to play more than once, simply
;				change Anim_PlayCount after InitAnim.
;				(anims with a negative PlayCount will play forever)
; 			To cause the anim to play slower, place a larger
;				value in Anim_DelayCounter after InitAnim.
;		Then call AnimTick at a constant rate, and animation will play.
;
;
; (see anim.asm in the demo directory for an example of how to use this library)
;------------------------------------------------------------------------------

code	segment

;==============================================================================
;	Inputs:
;		a0-> uninitialized anim structure
;		a1-> array of map ptr's, zero terminated
;		d0.w = starting x coordinate
;		d1.w = starting y coordinate
;		d2.w = char base in vdp
;		d3.w = which field this will use(FIELD_A or FIELD_B)
;------------------------------------------------------------------------------

InitAnim:
	move.w	d0,Anim_XPos(a0)		; anim x coordinate
	move.w	d1,Anim_YPos(a0)		; anim y coordinate
	move.w	d2,Anim_CharBase(a0)

	move.w	d3,d0
	jsr	InitFieldStruct

	move.l	a1,Anim_Base(a0)
	clr.w	Anim_Frame(a0)
	move.w	#1,Anim_Delay(a0)
	move.w   #1,Anim_DelayCounter(a0)
	move.w   #1,Anim_PlayCount(a0)
	rts

;==============================================================================

AnimTick:
	subq.w	#1,Anim_DelayCounter(a0)
	bne.s	.NotYet
	move.w	Anim_Delay(a0),Anim_DelayCounter(A0)

	move.w	Anim_Frame(a0),d0
	addq.w	#1,Anim_Frame(a0)
	add.w	d0,d0				; * 4 for pointer lookup
	add.w	d0,d0				; note: this means you can only
						; have 16384 frames in an anim(big limitation, huh?)
	move.l	Anim_Base(a0),a2
	move.l	(a2,d0.w),a1			; now we have the pointer to the map
	move.l	a1,d1				; tst.l a1
	bne.s	.Doit
	clr.w	Anim_Frame(a0)			; stash new anim frame #
	tst.w	Anim_PlayCount(a0)
	beq.s	.Done				; if PlayCount is at zero, don't play anymore
	move.l	(a2),a1				; read new map ptr(note: this means each anim should have at least one frame)
	bmi.s	.Doit				; if PlayCount is negative, play forever
	subq.w	#1,Anim_PlayCount(a0)		; update # of times left to play anim
	beq.s	.Done				; if it becomes zero, we are done
.Doit:
					; now its time to plot the frame
	move.w	Anim_XPos(a0),d0
	move.w	Anim_YPos(a0),d1
	move.w	Anim_CharBase(a0),d2
	jsr	PlotMapField				; actually plot the frame
.NotYet:
.Done:
	rts

;==============================================================================

	end

;==============================================================================
