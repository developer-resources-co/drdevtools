;==============================================================================
; enemy.asm: enemy logic
;==============================================================================

CODE	segment

MAXBADDIES	EQU	20
MAXENEMYSPEED	EQU	$40000

;==============================================================================

InitEnemies:
	clr.w	badCount

	lea	EnemyObj,a0
	jsr	AddObject

	move.w	#ObjType_Player,d0
	jsr	FindObject				; assume player object is found
	move.l	a0,Obj_DataPtr(a6)
	rts

;==============================================================================

EnemyObj:			; master enemy spawning code
	jsr	RandomNum
	and.w	#$3f,d0
	add.w	#60,d0
	move.w	d0,Obj_Delay(a6)			; random delay before spawning new enemy

				; spawn new enemy
	cmp.w	#MAXBADDIES,badCount
	bcc.s	.Nope
	bsr	SpawnBaddie1
.Nope:
	rts

;==============================================================================

SpawnBaddie1:
	move.l	a6,a5			; stash current object pointer
	lea	Baddie1Obj,a0
	jsr	AddObject
	bne.s	.Nope

	move.w	#ObjType_Enemy,Obj_Type(a6)
	bsr	FindTarget
	move.l	a0,Obj_DataPtr(a6)

	clr.w	d0
	move.w	Scroll_MapWidthPixel+scrollB,d0
	jsr	RandomRange
	move.w	d0,Obj_XPos(a6)

	move.l	#(SCREENHEIGHT/2)<16,Obj_YPos(a6)
	clr.l	Obj_xVector(a6)
	clr.l	Obj_yVector(a6)

	move.w	#16,Obj_XSize(a6)
	move.w	#16,Obj_YSize(a6)

	jsr	RandomNum
	and.w	#$f,d0
	add.w	#5,d0
	move.w	d0,Obj_Counter(a6)

	move.w	#3,Obj_SprNumber(a6)
	addq.w	#1,badCount
.Nope:
	move.l	a5,a6			; remove for fun bug #1
	rts

;==============================================================================

Baddie1Obj:
	bsr	MoveTowardTarget
					; display self
	move.l	#MAXENEMYSPEED,d0
	move.l	d0,d1
	bsr	ClipObjectVelocities
	jsr	MoveObjectSprite
	jsr	DisplayObjectSprite
	rts

;==============================================================================

KillBaddie:
	KillObject
	subq.w	#1,badCount
	rts

;==============================================================================
; support routines
;------------------------------------------------------------------------------


;==============================================================================
; Outputs:
;		a0-> target
FindTarget:
	move.w	#ObjType_Player,d0
	jsr	FindObject				; assume player object is found
	rts

;==============================================================================

MoveTowardTarget:
	move.w	Obj_Counter(a6),d2
	move.l	Obj_DataPtr(a6),a0

	move.w	Obj_Counter(a6),d2

	move.l	Obj_xPos(a6),d0
	sub.l	Obj_xPos(a0),d0
	asr.l	d2,d0
	sub.l	d0,Obj_xVector(a6)

	move.l	Obj_yPos(a6),d1
	sub.l	Obj_yPos(a0),d1
	asr.l	d2,d1
	sub.l	d1,Obj_yVector(a6)

	rts

;==============================================================================

ram	segment
badCount	ds.w	1

;==============================================================================

	END

;==============================================================================
