;==============================================================================
;	Player.asm: player logic & ship display
;==============================================================================

CODE	segment

FIRERATE	equ	10		; larger #'s =  slower rate (1 = once per frame)

FRICTION	EQU	$1000		; rate at which ship slows down
MAXSPEED	EQU	$100000
SHIPYSPEED	EQU	2
SLOWMISSILESPEED	equ	6
FASTMISSILESPEED	equ	8
SCROLLEDGE	equ	14*8

SHIPXSIZE	equ	16
SHIPYSIZE	equ	16

SHIPXDELTA	equ	$4000

;==============================================================================

InitPlayers:
; init player one object(joystick 1)
	lea	PlayerObj,a0				; pointer to code to run each frame
	jsr	AddObject
	move.w	#ObjType_Player,Obj_Type(a6)
	move.l	#((SCREENWIDTH/2)-(SHIPXSIZE/2))<16,Obj_XPos(a6)				; starting x coordinate
	move.l	#((SCREENHEIGHT/2)-(SHIPYSIZE/2))<16,Obj_YPos(a6)				; starting y coordinate
	clr.l	Obj_xVector(a6)
	clr.l	Obj_yVector(a6)
	clr.w	Obj_Counter(a6)
	clr.w	Obj_Direction(a6)
	move.l	#ReadJoy1,Obj_DataPtr(a6)

; create friend
	lea	FriendObj,a0				; pointer to code to run each frame
	jsr	AddObject
	move.w	#ObjType_Friend,Obj_Type(a6)

	move.l	#((SCREENWIDTH/2)-(SHIPXSIZE/2) + 80)<16,Obj_XPos(a6)				; starting x coordinate
	move.l	#((SCREENHEIGHT/2)-(SHIPYSIZE/2) + 80)<16,Obj_YPos(a6)				; starting y coordinate
	move.w	#7,Obj_Counter(a6)
	move.l	#$50000,Obj_xVector(a6)
	clr.l	Obj_yVector(a6)
	clr.w	Obj_Direction(a6)
	clr.w	Obj_SprNumber(a6)
	rts

;==============================================================================
; this example object reads the joystick and positions a logical sprite
; based on the input. Note the same code works for both players
; Obj_DataPtr is used by this routine to choose which joystick routine to call

PlayerObj:
	move.l	Obj_DataPtr(a6),a0
	jsr	(a0)					; call joystick routine
	lea	JoyTable(pc),a0
	jsr	ButtonDispatch

	move.l	#MAXSPEED,d0
	move.l	d0,d1
	bsr	ClipObjectVelocities
	move.l	#FRICTION,d0
	move.l	d0,d1
	bsr	DoObjectFriction
	bsr	MoveObjectSprite

	move.w	Obj_XPos(a6),d0
	move.w	Obj_YPos(a6),d1
	move.w	#SPRITEF_PAL3,d2
	or.w	Obj_Direction(a6),d2
	move.w	#0,d3
	jsr	AddGameSprite
	rts


;==============================================================================

FriendObj:
	bsr	MoveTowardTarget
					; display self
	move.l	#MAXSPEED,d0
	move.l	#MAXENEMYSPEED,d1
	bsr	ClipObjectVelocities
	jsr	MoveObjectSprite
	jsr	DisplayObjectSprite

	move.l	Obj_DataPtr(a6),a0
	move.w	Obj_Direction(a0),Obj_Direction(a6)
	move.w	#ObjType_Enemy,d0
	bsr	ObjectCollide
	bne.s	.Nope
	exg	a0,a6
	bsr	KillBaddie
	exg	a0,a6
.Nope:
	rts

;=============================================================================

JoyTable:
	dc.l	MoveUp				; up
	dc.l	MoveDown			; down
	dc.l	MoveLeft			; left
	dc.l	MoveRight			; right
	dc.l	.Fire1			; button b
	dc.l	.Fire2			; button a
	dc.l	DoNothing   			; button c
	dc.l	DoNothing			; start button


.Fire1:
	move.w	#1,d2
	move.w	#SLOWMISSILESPEED,d0

.Entry:
	subq.w	#1,Obj_Counter(a6)
	bpl.s	.NotYet

	tst.w	Obj_Direction(a6)
	beq.s	.No
	neg.w	d0
.No:
	clr.w	d1
	add.w	Obj_xVector(a6),d0
	jsr	SpawnWeapon
	move.w	#FIRERATE,Obj_Counter(a6)
.NotYet
	rts

;------------------------------------------------------------------------------

.Fire2:
	move.w	#2,d2
	move.w	#FASTMISSILESPEED,d0
	bra.s	.Entry

;==============================================================================

MoveLeft:
	move.w	#SPRITEF_XFLIP,Obj_Direction(a6)
	sub.l	#SHIPXDELTA,Obj_xVector(a6)
	rts

MoveUp:
	move.w	Obj_YPos(a6),d0
	subq.w	#SHIPYSPEED,d0
	bcs.s	.Nope
	move.w	d0,Obj_YPos(a6)
.Nope:
	rts

MoveDown:
	move.w	Obj_YPos(a6),d0
	addq.w	#SHIPYSPEED,d0
	cmp.w	#SCREENHEIGHT-SHIPYSIZE,d0
	bcc.s	.Nope
	move.w	d0,Obj_YPos(a6)
.Nope:
	rts

MoveRight:
	clr.w	Obj_Direction(a6)
	add.l	#SHIPXDELTA,Obj_xVector(a6)
	rts

;==============================================================================

	END

;==============================================================================
