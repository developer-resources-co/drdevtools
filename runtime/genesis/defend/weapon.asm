;==============================================================================
; weapon.asm: weapon objects
;==============================================================================

CODE	segment

;==============================================================================
; Inputs:
;		d0 = x vector
;		d1 = y vector
;		d2 = weapon #
;		a6-> spawning object
;------------------------------------------------------------------------------

SpawnWeapon:
	push.l	a0
	push.w	d0/d1/d2
	move.l	a6,a5			; stash current object pointer
	lea	BulletObj,a0
	jsr	AddObject
	bne.s	.Nope				; if no objects available, forget it
	move.w	Obj_XPos(a5),Obj_XPos(a6)		; make object start
	move.w	Obj_YPos(a5),Obj_YPos(a6)		; its life where we are

	pop.w 	d0/d1/d2
	move.w	d0,Obj_xVector(a6)
	move.w	d1,Obj_yVector(a6)
	move.w	d2,Obj_SprNumber(a6)

	add.w	d0,Obj_XPos(a6)
	add.w	d1,Obj_YPos(a6)

	clr.w	Obj_Direction(a6)
	tst.w	d0
	bpl.s	.Ok
	move.w	#SPRITEF_XFLIP,Obj_Direction(a6)
.Ok:

.Nope:
	move.l	a5,a6			; get our object pointer back(VERY IMPORTANT!)
	pop.l	a0
	rts

;==============================================================================
; this is the weapons code

BulletObj:
					; display self
	move.w	Obj_xVector(a6),d0				; move to the right each frame
	add.w	d0,Obj_XPos(a6)			; move to the right each frame
	jsr	DisplayObjectSprite

	move.w	#ObjType_Enemy,d0
	bsr	ObjectCollide
	bne.s	.Nope
	exg	a0,a6
	bsr	KillBaddie
	exg	a0,a6
.Nope:
	bsr	KillIfOffScreen
	rts

;==============================================================================

KillIfOffScreen:
	move.w	Scroll_XPos+scrollB,d0
	cmp.w	Obj_XPos(a6),d0
	bcc.s	.Die
	add.w	#SCREENWIDTH,d0
	cmp.w	Obj_Xpos(a6),d0
	bcs.s	.Die
	rts

.Die:
	KillObject
	rts

;==============================================================================

	END

;==============================================================================
