;==============================================================================
;===	menu.asm: list selection library
;=== 	This is mostly for option screens, and is incomplete
;==============================================================================

; this structure can reside in ROM
MenuItemStruct	defs	ram
MenuItemStruct	segment
	org	0
MenuItem_MaxVal	ds.w	1		; largest value allowed in this selection
MenuItem_XCoord	ds.w	1
MenuItem_YCoord	ds.w	1
MenuItem_RoutinePtr	ds.l	1	; pointer to code to run to change this selection
MenuItem_ActionPtr	ds.l	1	; pointer to code to run for button c
MenuItem_DataPtr	ds.l	1		; user pointer
MenuItem_DataPtr2	ds.l	1		; user pointer
MenuItem_Size

;------------------------------------------------------------------------------

Menu_Struct	defs	ram
Menu_Struct	segment
	org	0

Menu_ItemPtr	ds.l	1
Menu_InputPtr	ds.l	1
Menu_MaxItem	ds.w	1
Menu_CurrentItem	ds.w	1
Menu_Size

;==============================================================================

code	segment

;==============================================================================
; InitMenu:
;	Inputs:
;		d0 = # of items-1(max item)
;		a0-> uninitialized menu struct
;		a1-> menu item array
;		a2-> input struct
;------------------------------------------------------------------------------

InitMenu:
	move.l	a1,Menu_ItemPtr(a0)
	move.l	a2,Menu_InputPtr(a0)
	clr.w	Menu_CurrentItem(a0)
	move.w	d0,Menu_MaxItem(a0)
	bsr	UpdateAllMenuItems
	rts

;==============================================================================
;===	call when menu input handling is required
;===	Inputs:
;===		a0-> Menu Structure
;===	Internal
;===		a3-> Menu Structure
;==============================================================================

UpdateMenu:
	move.l	a0,a3

	move.l	Menu_InputPtr(a3),a0
	jsr	ReadDigitalInput			; waits for keypress, and returns it
	move.b	Input_DownEdge(a0),d0
	lea	.buttonTable(pc),a0
	jsr	ButtonDispatch
	rts

;------------------------------------------------------------------------------

.buttonTable:
	dc.l	.Up
	dc.l	.Down
	dc.l	.Left
	dc.l	.Right
	dc.l	.B
	dc.l	.C
	dc.l	.A
	dc.l	.Start

;------------------------------------------------------------------------------

.Up:
	subq.w	#1,Menu_CurrentItem(a3)
	bcc.s	.UpOk
	move.w	Menu_MaxItem(a3),Menu_CurrentItem(a3)
.UpOk:
	rts

.Down
	move.w	Menu_CurrentItem(a3),d0
	addq.w	#1,d0
	cmp.w	Menu_MaxItem(a3),d0
	bls.s	.DownNope
	clr.w	d0
.DownNope:
	move.w	d0,Menu_CurrentItem(a3)
	rts

;------------------------------------------------------------------------------

.A
	move.w	#-1,d0
.LEntry:
	move.w	Menu_CurrentItem(a3),d1
	mulu	#MenuItem_Size,d1
	move.l	Menu_ItemPtr(a3),a0
	lea	(a0,d1.l),a0			; point to correct item
	move.l	MenuItem_RoutinePtr(a0),a2
	jsr	(a2)
	rts

.B
	moveq	#1,d0
	bra.s	.LEntry

;------------------------------------------------------------------------------

.Start
	sf	continue
.Right
.Left
	rts

;==============================================================================

.C
	move.w	Menu_CurrentItem(a3),d1
	mulu	#MenuItem_Size,d1
	move.l	Menu_ItemPtr(a3),a0
	lea	(a0,d1.l),a0			; point to correct item
	move.l	MenuItem_ActionPtr(a0),a2
	jsr	(a2)
	rts

;==============================================================================
;===		a0-> Menu Structure
;==============================================================================

UpdateAllMenuItems:
	move.w	Menu_MaxItem(a0),d7
	move.w	a0,a3
.Loop:
	move.w	d7,d0
	mulu	#MenuItem_Size,d0
	move.l	Menu_ItemPtr(a3),a0
	lea	(a0,d0.l),a0			; point to correct item
	move.l	MenuItem_RoutinePtr(a0),a2
	clr.w	d0
	jsr	(a2)
	dbra	d7,.Loop
	rts

;==============================================================================
;	d0.w = addend(-1,0,1)
;	a0-> MenuItem struct
;	MenuItem_DataPtr-> word value to change
;	MenuItem_DataPtr2-> array of text string ptrs to print

TextMenuItemRoutine:
	move.l	MenuItem_DataPtr(a0),a2
	move.w	(a2),d1

	add.w	d0,d1
	bpl.s	.DontWrap
	move.w	MenuItem_MaxVal(a0),d1
.DontWrap:
	cmp.w	MenuItem_MaxVal(a0),d1
	bls.s	.DontClear
	clr.w	d1
.DontClear:
	move.w	d1,(a2)
				; now update on-screen text image
	move.w	MenuItem_XCoord(a0),cursorX
	move.w	MenuItem_YCoord(a0),cursorY

	move.l	MenuItem_DataPtr2(a0),a2
	add.w	d1,d1
	add.w	d1,d1				; *4 , size of long pointer
	move.l	(a2,d1.w),a0			; get pointer to string
	jsr	PrintText

	rts

;==============================================================================

DoNothing:
	rts

;==============================================================================

ram	segment
continue	ds.b	1

;==============================================================================

	END

;==============================================================================
