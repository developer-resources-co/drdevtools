;==============================================================================
; gameobj.asm: test objects for the example game
;==============================================================================

code	Segment

;==============================================================================

;==============================================================================
; 	Inputs:
;		d0.w = x position on field
;		d1.w = y position on field
;		d2 = sprite descriptor(use hardware equates)
;		d3.w = sprite # to display
;	Destroys:
;		d0.w-d7.w,a0.l-a3.l
;------------------------------------------------------------------------------

AddGameSprite:
	sub.w	Scroll_XPos+scrollB,d0
	bmi.s	.Nope
	cmp.w	#SCREENWIDTH,d0
	bcc.s	.Nope

	lea	spriteBase,a0
	lsl.w	#3,d3
	lea	(a0,d3.w),a0
	move.l	(a0)+,a3
	move.l	(a0),a0
	jsr	AddLSpriteBase
.Nope:
	rts

;==============================================================================

InitGameObjects
	jsr	InitPlayers
; field object
	lea	FieldObj,a0
	jsr	AddObject

	move.w	#ObjType_Player,d0
	jsr	FindObject				; assume player object is found
	move.l	a0,Obj_DataPtr(a6)

	jsr	InitEnemies
	rts

;==============================================================================

ScrollFieldRight:
	lea	scrollB,a0
	jsr	ScrollHorizRight
	rts

ScrollFieldLeft:
	lea	scrollB,a0
	jsr	ScrollHorizLeft
	rts

;==============================================================================
; cause field to track player
;------------------------------------------------------------------------------

FieldObj:
	move.l	Obj_DataPtr(a6),a0			; get pointer to object we are tracking(player 1)
.Loop:
						; first check if we should scrll left
	move.w	Obj_XPos(a0),d0				; get x coordinate of object we are tracking
	move.w	Scroll_XPos+scrollB,d1			; get current x coordinate of field
	beq.s	.No					; if already at left edge, no point in trying to go any further
	add.w	#SCROLLEDGE,d1				; offset by scroll bounds
	cmp.w	d1,d0
	bcc.s	.No
	push.l	a0
	bsr	ScrollFieldLeft                         ; actually scroll left
	pop.l	a0
	bra.s	.Loop
.No:
						; now check if we need to scroll right
	move.w	Scroll_XPos+scrollB,d0			; get x coordinate of object we are tracking
	cmp.w	Scroll_MapWidthPixel+scrollB,d0
	bcc.s	.No2					; if already at end, don't try to scroll further

	add.w	#SCREENWIDTH-SCROLLEDGE,d0		; this is a right edge check, so offset by scroll bounds and size of display

	cmp.w	Obj_XPos(a0),d0
	bcc.s	.No2
	push.l	a0
	bsr	ScrollFieldRight			; actually scroll
	pop.l	a0
	bra.s	.Loop
.No2:
	rts

;==============================================================================

MoveObjectSprite:
					; update x
	move.l	Obj_xVector(a6),d0
	add.l	Obj_xPos(a6),d0
	bpl.s	.OkX				; clip to left edge of universe
	clr.l	d0
.OkX
	lea	scrollB,a0


						; clip to right edge of universe
	swap	d0					; place fraction in upper word
	cmp.w	Scroll_MapWidthPixel(a0),d0
	bcs.s	.OkX2
	move.w	Scroll_MapWidthPixel(a0),d0
.OkX2:
	swap	d0					; get fracion back
						; store new x coordinate
	move.l	d0,Obj_XPos(a6)

					; update y
	move.l	Obj_yVector(a6),d0
	add.l	Obj_YPos(a6),d0
	bpl.s	.OkY				; clip to top edge of universe
	clr.l	d0
.OkY
						; clip to bottom edge of universe
	swap	d0
	lea	scrollB,a0
	cmp.w	Scroll_MapHeightPixel(a0),d0
	bcs.s	.OkY2
	move.w	Scroll_MapHeightPixel(a0),d0
.OkY2:
	swap	d0
						; store new y coordinate
	move.l	d0,Obj_YPos(a6)
	rts

;==============================================================================
;	d0.w = object max x velocity
;	d1.w = object max y velocity
;------------------------------------------------------------------------------

CLVit	macro
	cmp.l	{1},d2
	bmi.s	.Nope{L}
	move.l	{1},d2
.Nope{L}:
	endm


ClipObjectVelocities:
					; x
	move.l	Obj_xVector(a6),d2
	bpl.s	.Pos
	neg.l	d2
	CLVit	d0
	neg.l	d2
	bra.s	.Done
.Pos:
	CLVit	d0
.Done:
	move.l	d2,Obj_xVector(a6)
					; y
	move.l	Obj_yVector(a6),d2
	bpl.s	.PosY
	neg.l	d2
	CLVit	d1
	neg.l	d2
	bra.s	.DoneY
.PosY:
	CLVit	d1
.DoneY:
	move.l	d2,Obj_yVector(a6)
	rts

;==============================================================================
;	d0.w = object x friction
;	d1.w = object y friction
;------------------------------------------------------------------------------

DOFit	macro
	bpl.s	.Pos{L}
	neg.l	d0
	sub.l	d0,d2
	bcs.s	.Nope{L}
	clr.l	d2
	bra.s	.Done{L}
.Pos{L}:
	sub.l	d0,d2
	bcc.s	.Nope{L}
	clr.l	d2
.Nope{L}:

.Done{L}:
	endm

;------------------------------------------------------------------------------

DoObjectFriction:
	move.l	Obj_xVector(a6),d2
	DOFit	d0
	move.l	d2,Obj_xVector(a6)

	move.l	Obj_yVector(a6),d2
	DOFit	d1
	move.l	d2,Obj_yVector(a6)
	rts

;==============================================================================
;	Inputs:
;		d0 = ObjType to check, returns first one hit
;		a6-> object to check against
;	Outputs:
;		Z = collision, a0-> object hit
;		NZ = no object hit
;------------------------------------------------------------------------------

ObjectCollide:
	jsr	FindObject
	bne.s	.Nope
.Loop:
	; collision here
	move.w	Obj_XPos(a0),d2
	cmp.w	Obj_XPos(a6),d2
	bcc.s	.Next
	add.w	Obj_XSize(a0),d2
	cmp.w	Obj_XPos(a6),d2
	bcs.s	.Next

	move.w	Obj_YPos(a0),d2
	cmp.w	Obj_YPos(a6),d2
	bcc.s	.Next
	add.w	Obj_YSize(a0),d2
	cmp.w	Obj_YPos(a6),d2
	bcs.s	.Next
	clr.w	d2				; set Z flag
	rts

.Next:
;	jsr	FindNextObject
	MacFindNxtObj
	beq.s	.Loop
.Nope:
	rts

;==============================================================================

DisplayObjectSprite:
	move.w	Obj_XPos(a6),d0
	move.w	Obj_YPos(a6),d1
	move.w	#SPRITEF_PAL3,d2
	or.w	Obj_Direction(a6),d2
	move.w	Obj_SprNumber(a6),d3
	jsr	AddGameSprite
	rts

;==============================================================================

ram	segment

;==============================================================================

	END

;==============================================================================
