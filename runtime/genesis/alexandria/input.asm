;==============================================================================
;===	Input.asm: high level input handling				    ===
;===	This library is under construction				    ===
;===	this version is in Moon until finished				    ===
;==============================================================================

	opt	llbl
	include genesis.equ
	include default.equ
	include macros.mac

	XDEF	_InitInput
	XDEF	_ReadInput

	xref	ReadJoy1
	xref    ReadJoy2

;==============================================================================

;	include	struct.asm

;	TInput structure offsets:
TInput_proportionalX	equ	0
TInput_proportionalY	equ	2
TInput_booleans	equ	4
TInput_xSensitivity	equ	6
TInput_ySensitivity	equ	8
TInput_controllerRoutine	equ	10
TInput_controllerGeneric	equ	14
TInput_oldBooleans	equ	18
TInput_edge	equ	20
TInput_downEdge	equ	22
TInput_upEdge	equ	24
TInput_type	equ	26
TInput__SIZEOF__	equ	28


;------------------------------------------------------------------------------

	OFFSET	0
INPUT_TYPENONE		ds.b	1
INPUT_TYPEOLDJOY	ds.b	1
INPUT_TYPEJOY		ds.b	1
INPUT_TYPEMENACER	ds.b	1
INPUT_TYPEACTIVATOR	ds.b	1
INPUT_TYPEMOUSE		ds.b	1
INPUT_TYPETRAKBALL	ds.b	1
INPUT_TYPEMAX		ds.b	1

	OFFSET 0
INPUTBOOLB_A		ds.b	1
INPUTBOOLB_B		ds.b	1
INPUTBOOLB_C		ds.b	1
INPUTBOOLB_D		ds.b	1
INPUTBOOLB_E		ds.b	1
INPUTBOOLB_F		ds.b	1
INPUTBOOLB_G		ds.b	1
INPUTBOOLB_H		ds.b	1
INPUTBOOLB_I		ds.b	1
INPUTBOOLB_J		ds.b	1
INPUTBOOLB_K		ds.b	1
INPUTBOOLB_PAUSE		ds.b	1
INPUTBOOLB_UP		ds.b	1
INPUTBOOLB_DOWN		ds.b	1
INPUTBOOLB_LEFT		ds.b	1
INPUTBOOLB_RIGHT		ds.b	1

INPUTBOOLF_A	equ	1<<INPUTBOOLB_A
INPUTBOOLF_B	equ     1<<INPUTBOOLB_B
INPUTBOOLF_C	equ     1<<INPUTBOOLB_C
INPUTBOOLF_D	equ     1<<INPUTBOOLB_D
INPUTBOOLF_E	equ     1<<INPUTBOOLB_E
INPUTBOOLF_F	equ     1<<INPUTBOOLB_F
INPUTBOOLF_G	equ     1<<INPUTBOOLB_G
INPUTBOOLF_H	equ     1<<INPUTBOOLB_H
INPUTBOOLF_I	equ     1<<INPUTBOOLB_I
INPUTBOOLF_J	equ     1<<INPUTBOOLB_J
INPUTBOOLF_K	equ     1<<INPUTBOOLB_K
INPUTBOOLF_PAUSE equ     1<<INPUTBOOLB_PAUSE
INPUTBOOLF_UP	equ     1<<INPUTBOOLB_UP
INPUTBOOLF_DOWN	equ     1<<INPUTBOOLB_DOWN
INPUTBOOLF_LEFT	equ     1<<INPUTBOOLB_LEFT
INPUTBOOLF_RIGHT equ     1<<INPUTBOOLB_RIGHT

;==============================================================================

	TSECTION	.text

;==============================================================================
;=== InitInput: 							    ===
;=== 	Inputs:								    ===
;===		a0-> input structure					    ===
;===		d0-> input port  0 ReadJoy1 1 ReadJoy2			    ===
;=== 	Destroys:                                             		    ===
;===		C Compliant.                                                ===
;=== 	Prototype:                                                          ===
;===		void InitInput (input *inStruct, UWORD port; 		    ===
;==============================================================================

_InitInput:
	move.l	4(sp),a0			; read ptr to input structure
	move.l  8(sp),d0
InitInput:
	clr.w	TInput_oldBooleans(a0)
	clr.w	TInput_edge(a0)
	clr.w	TInput_downEdge(a0)
	clr.w	TInput_upEdge(a0)

	clr.w	TInput_proportionalX(a0)
	clr.w	TInput_proportionalY(a0)

; kts temp code
	move.l	#ReadControllerJoy,TInput_controllerRoutine(a0)
	move.w	#1,TInput_xSensitivity(a0)
	move.w	#1,TInput_ySensitivity(a0)
	move.l	#ReadJoy1,TInput_controllerGeneric(a0)
	tst.w   d0
	beq.s   Done
	move.l  #ReadJoy2,TInput_controllerGeneric(a0)
Done:
	move.w	#INPUT_TYPEJOY,TInput_type(a0)
	rts

;==============================================================================
;=== ReadInput: fill out input structure				    ===
;=== 	Inputs:								    ===
;===		a0-> input structure					    ===
;=== 	Outputs:							    ===
;===		d0 = input booleans					    ===
;=== 	Destroys:							    ===
;===		a0-a1							    ===
;===	(maybe)	d0-d1							    ===
;==============================================================================

_ReadInput:
	move.l	4(sp),a0
ReadInput:
	move.l	TInput_controllerRoutine(a0),a1
	jsr	(a1)					; read input into struct
	rts

;-------------------------------------------------------------------------------
; joystick version of controller reader
;  cannot nuke anything other than d0-d1/a0-a1

ReadControllerJoy:
	move.l	TInput_controllerGeneric(a0),a1
	move.l	a0,-(sp)
	jsr	(a1)					; read joystick
	move.l	(sp)+,a0

;	clr.w	TInput_proportionalX(a0)
;	clr.w	TInput_proportionalY(a0)

	btst	#JOYB_UP,d0
	beq.s	.NotUp
	move.w	TInput_ySensitivity(a0),d1
	sub.w	d1,TInput_proportionalY(a0)
.NotUp:
	btst	#JOYB_DOWN,d0
	beq.s	.NotDown
	move.w	TInput_ySensitivity(a0),d1
	add.w	d1,TInput_proportionalY(a0)
.NotDown:
	btst	#JOYB_LEFT,d0
	beq.s	.NotLeft
	move.w	TInput_xSensitivity(a0),d1
	sub.w	d1,TInput_proportionalX(a0)
.NotLeft:
	btst	#JOYB_RIGHT,d0
	beq.s	.NotRight
	move.w	TInput_xSensitivity(a0),d1
	add.w	d1,TInput_proportionalX(a0)
.NotRight:
; now do buttons
	move.b	d0,d1

	ror.w	#4,d0			; move directions up to the top 4 bits
	and.w	#$f000,d0

	btst	#JOYB_START,d1		; do start
	beq.s	.NoStart
	bset	#INPUTBOOLB_PAUSE,d0
.NoStart:
	btst	#JOYB_A,d1
	beq.s	.NoA
	bset	#INPUTBOOLB_A,d0
.NoA:
	btst	#JOYB_B,d1
	beq.s	.NoB
	bset	#INPUTBOOLB_B,d0
.NoB:
	btst	#JOYB_C,d1
	beq.s	.NoC
	bset	#INPUTBOOLB_C,d0
.NoC:
	move.w	d0,TInput_booleans(a0)

; now do edges
	move.w	TInput_oldBooleans(a0),d1
	move.w	d0,TInput_oldBooleans(a0)

	eor.w	d0,d1				; generate edge
	move.w	d1,TInput_edge(a0)
	move.w	d1,TInput_upEdge(a0)

	and.w	d0,d1				; generate down edge
	move.w	d1,TInput_downEdge(a0)
	eor.w	#$ffff,d0
	and.w	d0,TInput_upEdge(a0)

;						; clear velocities if needed
	move.w	TInput_upEdge(a0),d0
	and.w	#INPUTBOOLF_LEFT|INPUTBOOLF_RIGHT,d0
	beq.s	.NoClrX
	clr.w	TInput_proportionalX(a0)
.NoClrX:
	and.w	#INPUTBOOLF_UP|INPUTBOOLF_DOWN,d0
	beq.s	.NoClrY
	clr.w	TInput_proportionalY(a0)
.NoClrY:
	rts

;------------------------------------------------------------------------------
; 0	A	primary action
; 1     B	secondary action
; 2     C	teritary action
; 3
; 4
; 5
; 6
; 7
; 8
; 9
; 10
; 11    pause (start)
; 12	up
; 13    down
; 14    left
; 15	right


	ifne	0

;==============================================================================
;=== ReadDigitalInput: return joystick compatible input bits immediately    ===
;=== 	Inputs:								    ===
;===		a0-> input structure					    ===
;===									    ===
;===									    ===
;==============================================================================

ReadDigitalInput:
	move.l	a1,-(sp)
	move.l	TInput_ControllerRoutine(a0),a1
	move.l	a0,-(sp)
	jsr	(a1)
	move.l	(sp)+,a0
	move.b	TInput_OldJoy(a0),d1
	eor.b	d0,d1				; generate edge
	move.b	d1,TInput_Edge(a0)
	move.b	d1,TInput_UpEdge(a0)

	and.b	d0,d1				; generate down edge
	move.b	d1,TInput_DownEdge(a0)
	move.b	d0,d2
	eor.b	#$ff,d2
	and.b	d2,TInput_UpEdge(a0)

	move.b	d0,TInput_OldJoy(a0)
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
	move.l	TInput_ControllerRoutine(a2),a1
	jsr	(a1)
	tst.b	d0
	bne.s	.Loop
; this is all for now, other input handling goes here
	rts

;============================================================================
;===	GetTextString: Get text string from player			  ===
;===	Inputs: 							  ===
;=== 		a0-> buffer to place string				  ===
;=== 		a1-> input structure					  ===
;===		d0 = # of characters to enter				  ===
;===		cursorX & cursorY = coordinate to begin entering string	  ===
;===	 								  ===
;===	Internal:							  ===
;===		maxchars = max # of chars				  ===
;===		d3.b = current letter being entered			  ===
;===		d4.w = current position in string			  ===
;===		d5.w = continue flag					  ===
;=== 		a4-> buffer to place string				  ===
;=== 		a3-> input structure					  ===
;===	 								  ===
;----------------------------------------------------------------------------

GetTextString:
	move.w	d0,maxChars
	move.w	cursorX,oldCursorX
	move.w	cursorY,oldCursorY
	move.l	a0,a4
	move.l	a1,a3
	move.w	#'A',d3
	clr.w	d4
	st	d5


	move.w	d3,-(sp)
	jsr	PrintTextCount
	move.w	(sp)+,d3
.Loop:
	move.l	a3,a0
	jsr	InputRelease
	move.l	a3,a0
	jsr	GetDigitalInput			; waits for keypress, and returns it
	lea	.buttonTable(pc),a0
	jsr	ButtonDispatch

	move.b	d3,(a4,d4.w)			; update string
	move.l	a4,a0
	move.w	maxChars,d0
	move.w	d3,-(sp)
	move.w	oldCursorX,cursorX
	move.w	oldCursorY,cursorY
	jsr	PrintTextCount
	move.w	(sp)+,d3

	tst.w	d5
	bne.s	.Loop
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

.Left
	subq.b	#1,d3
	cmp.b	#'A',d3
	bcc.s	.UpOk
	move.b	#'z',d3
.UpOk:
	rts

.Right
	addq.b	#1,d3
	cmp.b	#'z'+1,d3
	bcs.s	.DownOk
	move.b	#'A',d3
.DownOk:
	rts

.B
	subq.w	#1,d4
	bcc.s	.BOk
	addq.w	#1,d4

.BOk:
	move.b	(a4,d4.w),d3
.Down
.Up:
	rts

.C
.A
	addq.w	#1,d4
	cmp.w	maxChars,d4
	bne.s	.NotYet
	clr.w	d5				; last digit entered, all done
.NotYet:
	move.b	(a4,d4.w),d3
	rts

.Start
;	  				; fuck it, we are done
	clr.w	d5			; clear continue flag
	rts

;==============================================================================

	endc

;==============================================================================

	end

;==============================================================================
