;==============================================================================
;===	scroll.asm: Genesis scrolling routines				    ===
;===	(c)1991,92 Developer Resources					    ===
;==============================================================================
;	Purpose:
;		Provide methods for scrolling playfields.
;		There are 3 intefaces in this library.
;		First, there is direct hardware manipulation.
;			SetVScroll
;			SetVScrollA
;			SetVScrollB
;			SetHScroll
;			SetHScrollA
;			SetHScrollB
;		Next, a two-way horizontal scrolling inteface.
;			InitHorizScroll
;			RedrawHorizScroll
;			ScrollHorizLeft
;			ScrollHorizRight
;		Finaly, a four-way scrolling interaface.
;			Init4WayScroll
;			Redraw4WayScroll
;			Scroll4WayUp
;			Scroll4WayDown
;			Scroll4WayLeft
;			Scroll4WayRight
;
;		Both the 2-way & 4-way scrolling interfaces use maps
;		as they come from CharGrab or Maped.
;
;------------------------------------------------------------------------------
;	Flow:
;		Once:
;			Download characters used by maps into VDP.
;			Init scroll interface
;		As desired, during VBLANK
;			scroll screen(ScrollHorizLeft,ScrollHorizRight,
;				Scroll4WayUp,Scroll4WayDown,Scroll4WayLeft,
;				Scroll4WayRight)
;------------------------------------------------------------------------------
;	(see sdemo, s2demo, s4demo for examples of how to use theses interfaces)
;------------------------------------------------------------------------------
; switches:
;	define SCROLLWRAP to get the scrolling to wrap at end of map
;	(currently only works on horiz scroll)
;==============================================================================

code	segment

;==============================================================================
; first, just the hardware scrolling interface

;==============================================================================
; 	SetVScroll: sets vert scroll assuming vert scroll in page mode
;	Inputs:
;		d0 = new vScroll value
;		d1 = which field to set(FIELD_A or FIELD_B)
;	Destroys:
;		d1.l
;------------------------------------------------------------------------------

SetVScroll:
	swap	d1
	clr.w	d1
	or.l	#VDP_MODE_VSW,d1
	move.l	d1,VDP_CMD
	move.w	d0,VDP_DATA
	rts

;==============================================================================
; 	SetVScrollA: sets vert scroll for field a assuming vert scroll in page mode
;	Inputs:
;		d0 = new vScroll value
;	Destroys:
;		none
;------------------------------------------------------------------------------

SetVScrollA:
	move.l	#VDP_MODE_VSW,VDP_CMD
	move.w	d0,VDP_DATA
	rts

;==============================================================================
; 	SetVScrollB: sets vert scroll for field b assuming vert scroll in page mode
;	Inputs:
;		d0 = new vScroll value
;	Destroys:
;		none
;------------------------------------------------------------------------------

SetVScrollB:
	move.l	#VDP_MODE_VSW+$20000,VDP_CMD
	move.w	d0,VDP_DATA
	rts

;==============================================================================
; 	SetHScroll: sets vert scroll assuming vert scroll in page mode
;	Inputs:
;		d0 = new vScroll value
;		d1 = which field to set(SCROLL_FIELDA or SCROLL_FIELDB)
;	Destroys:
;		d0.w,d2.w
;------------------------------------------------------------------------------

SetHScroll:
	move.w	d0,d2
	move.w	#VDP_HSCROLLBASE,d0
	or.w	d1,d0
	bsr	VDPAddress
	move.w	d2,VDP_DATA
	rts

;==============================================================================
; 	SetHScrollA: sets vert scroll for field a assuming vert scroll in page mode
;	Inputs:
;		d0 = new vScroll value
;	Destroys:
;		d0.l,d1.l,d2.w
;------------------------------------------------------------------------------

SetHScrollA:
	move.w	d0,d2
	move.w	#VDP_HSCROLLBASE,d0
	bsr	VDPAddress
	move.w	d2,VDP_DATA
	rts

;==============================================================================
; 	SetHScrollA: sets vert scroll for field b assuming vert scroll in page mode
;	Inputs:
;		d0 = new vScroll value
;	Destroys:
;		d0.l,d1.l,d2.w
;------------------------------------------------------------------------------

SetHScrollB:
	move.w	d0,d2
	move.w	#VDP_HSCROLLBASE+2,d0
	bsr	VDPAddress
	move.w	d2,VDP_DATA
	rts

;==============================================================================
;==============================================================================
; now a horizontal 2-way scroll interface
;------------------------------------------------------------------------------
;	Inputs:
;		a0->unitialized scroll struct
;		a1->map
;		d0.w = starting x coordinate
;		d1.w = y offset in chars(for plotting map lower on screen)
;		d2.w = character offset in VDP(plus palette selection)
;		d3.w = which field this will use(SCROLL_FIELDA or SCROLL_FIELDB)
;	Destroys:
;		VDP,d0.l,d1.l,d2.w-d6.w,a1.l
;	Restrictions:
;		Map must be at least the width of the screen
;------------------------------------------------------------------------------

InitHorizScroll:
	move.w	d0,Scroll_XPos(a0)		; starting x coordinate
	move.w	d2,Scroll_CharBase(a0)

	move.w	d3,d0
	jsr	InitFieldStruct
						; move down screen
	mulu.w	Field_Width(a0),d1
	add.l	d1,Field_Base(a0)
						; read map width & height
	move.w	(a1)+,d0
	move.w	d0,Scroll_MapHeight(a0)
	add.w	d0,d0
	add.w	d0,d0
	add.w	d0,d0
	move.w	d0,Scroll_MapHeightPixel(a0)

	sub.w	#VDP_SCREEN_HEIGHT,d0
	lsl.w	#3,d0
	move.w	d0,Scroll_BotLimit(a0)

	move.w	(a1)+,d0
	move.w	d0,Scroll_MapWidth(a0)
	add.w	d0,d0
	add.w	d0,d0
	add.w	d0,d0
	move.w	d0,Scroll_MapWidthPixel(a0)
	sub.w	#VDP_SCREEN_WIDTH,d0
	lsl.w	#3,d0
	move.w	d0,Scroll_RightLimit(a0)

	move.l	a1,Scroll_MapPtr(a0)
						; fall through

;==============================================================================
;	Inputs:
;		a0->scroll structure
;	Destroys:
;		VDP,d0.l,d1.l,d2.w-d5.w,a1.l
;------------------------------------------------------------------------------

RedrawHorizScroll:
	move.w	Scroll_XPos(a0),d0
	neg.w	d0
	move.w	Field_Num(a0),d1
	bsr	SetHScroll
	move.w	#VDP_SCREEN_WIDTH,d6		; # of actual columns on the screen + 1 column
.Loop:
	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place
	lsr.w	#3,d0				; convert x & y into char offsets
	add.w	d6,d0				; add in current column to draw
	move.w	d0,d2
	move.w	Field_Width(a0),d3
	subq.w	#1,d3
	and.w	d3,d2				; and with # of comlunms
	move.w	Scroll_YPos(a0),d3
	bsr	DownLoadColumn
	dbra	d6,.Loop

	rts

;==============================================================================
;	Inputs:
;		a0-> scroll structure
;		d0.w = x position in map to start from(char)
;		d2.w = column in field(in vdp) to plot to(char)
;		d3.w = y character position down the screen to start plotting map
;	Destroys:
;		VDP,d0.l,d1.l,d2.w-d5.w,a1.l
;------------------------------------------------------------------------------

DownloadColumn:
	move.w	#VDP_REG_AUTOINCSIZE+(VDP_FIELDA_WIDTH*2),VDP_CMD		; set up auto-increment
				; first find correct address in map
	move.l	Scroll_MapPtr(a0),a1
	add.w	d0,d0				; double since each entry is 2 bytes long
	mulu.w	Scroll_MapHeight(a0),d0
	add.w	d0,a1				; point to correct column in map
				; now set correct address on field(screen)

	add.w	d2,d2
	add.w	Field_Base(a0),d2		; add in base, d2 now contains starting position in field

	move.w	Scroll_CharBase(a0),d5
	move.w	Scroll_MapHeight(a0),d4		; since map could be shorter than screen
	move.w	Field_Width(a0),d3	; remember field width
	add.w	d3,d3
	subq.w	#1,d4
.Loop:
	move.w	d2,d0
	jsr	VDPAddress		; point vdp there
	add.w	d3,d2			; go to next row in screen memory

	move.w	(a1)+,d0				; read from map
	add.w	d5,d0					; add in character offset
	move.w	d0,VDP_DATA				; write to vdp
	dbra	d4,.Loop				; next

	move.w	#VDP_REG_AUTOINCSIZE+2,VDP_CMD
	rts

;==============================================================================

ScrollHorizLeft:
	move.w	Scroll_XPos(a0),d0
	subq.w	#1,d0
	ifdef	SCROLLWRAP
	bpl.s	.Pos
	move.w	Field_Width(a0),d0
	add.w	d0,d0			; * 2
	add.w	d0,d0                   ; * 4
	add.w	d0,d0                   ; * 8
.Pos:
	endif
	ifndef	SCROLLWRAP
	bmi.s	.Nope
	endif
	move.w	d0,Scroll_XPos(a0)
	neg.w	d0
	move.w	Field_Num(a0),d1
	bsr	SetHScroll
	move.w	Scroll_XPos(a0),d0
	move.w	d0,d1
	and.w	#$7,d1
	bne.s	.Nope

	move.w	d0,d2				; place xpos in d2
	lsr.w	#3,d2				; convert it into column address
	subq.w	#1,d2				; offset to just left of screen
	move.w	d2,d0				; make map offset
	move.w	Field_Width(a0),d1
	subq.w	#1,d1
	and.w	d1,d2				; and with # of comlunms

	move.w	Scroll_YPos(a0),d3
	bsr	DownLoadColumn
.Nope:
	rts

;==============================================================================

ScrollHorizRight:
	move.w	Scroll_XPos(a0),d0
	addq.w	#1,d0
	cmp.w	Scroll_RightLimit(a0),d0
	ifdef	SCROLLWRAP
	bcs.s	.Ok
	clr.w	d0
.Ok:
	endif
	ifndef	SCROLLWRAP
	bcc.s	.Nope
	endif
	move.w	d0,Scroll_XPos(a0)
	neg.w	d0
	move.w	Field_Num(a0),d1
	bsr	SetHScroll
	move.w	Scroll_XPos(a0),d0
	move.w	d0,d1
	and.w	#$7,d1				; are we on char boundry?
	bne.s	.Nope				; if not, we are done

	move.w	d0,d2				; place xpos in d2
	lsr.w	#3,d2				; convert xpos into column address
	add.w	#VDP_SCREEN_WIDTH,d2		; offset to right of screen
	ifdef	SCROLLWRAP
	cmp.w	Field_Width(a0),d2
	bcs.s	.Ok2
	sub.w	Field_Width(a0),d2
.Ok2:
	endif
	move.w	d2,d0
	move.w	Field_Width(a0),d1
	subq.w	#1,d1
	and.w	d1,d2				; and with # of comlunms

	move.w	Scroll_YPos(a0),d3		; get x offset so we start in right place
	bsr	DownLoadColumn
.Nope:
	rts

;==============================================================================
;==============================================================================
; now the 4-way scrolling code
; Note: at this time there is a bug in this code, it will not display the
; correct char(it misses it) in the upper left or lower right if you scroll
; diagonally with horizontal and vertical downloads on the same frame
;------------------------------------------------------------------------------
;	Inputs:
;		a0->scroll struct(this allows the same code to work on both playfields)
;		a1->map
;		d0.w = starting x coordinate
;		d1.w = starting y coordinate
;		d2.w = char base in vdp
;		d3.w = which field this will use(SCROLL_FIELDA or SCROLL_FIELDB)
;
;	Destroys:
;		??(lots of registers)
;	Restrictions:
;		Map must be at least the size of the screen
;------------------------------------------------------------------------------

Init4WayScroll:
	move.w	d0,Scroll_XPos(a0)
	move.w	d1,Scroll_YPos(a0)
	move.w	d2,Scroll_CharBase(a0)

	move.w	d3,d0
	jsr	InitFieldStruct
						; who cares, only happens during init
	move.l	#VDPScreenAddrB,Scroll_ScreenAddrRout(a0)
	tst.w	d3
	bne.s	.FB
	move.l	#VDPScreenAddrA,Scroll_ScreenAddrRout(a0)
.FB
	move.w	(a1)+,d0
	move.w	d0,Scroll_MapWidth(a0)
	sub.w	#VDP_SCREEN_WIDTH,d0
	lsl.w	#3,d0
	move.w	d0,Scroll_RightLimit(a0)

	move.w	(a1)+,d0
	move.w	d0,Scroll_MapHeight(a0)
	sub.w	#VDP_SCREEN_HEIGHT,d0
	lsl.w	#3,d0
	move.w	d0,Scroll_BotLimit(a0)

	move.l	a1,Scroll_MapPtr(a0)
						; fall through

;==============================================================================

Redraw4WayScroll:
	move.w	#VDP_SCREEN_HEIGHT,d5
.Loop:
	move.w	Scroll_YPos(a0),d3		; get x offset so we start in right place
	lsr.w	#3,d3				; convert ypos into column address
	add.w	d5,d3 				; current line offset
	move.w	d3,d1
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place
	lsr.w	#3,d0				; convert x & y into char offsets
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of comlunms
	bsr	DownLoad4WayRow
	dbra	d5,.Loop
	rts

;==============================================================================
; Inputs:
;	a0-> scroll struct
;------------------------------------------------------------------------------

Scroll4WayUp:
	move.w	Scroll_YPos(a0),d0
	subq.w	#1,d0
	bmi.s	.Nope
	move.w	d0,Scroll_YPos(a0)
	move.w	Field_Num(a0),d1
	bsr	SetVScroll
	move.w	d0,d1
	and.w	#$7,d1
	bne.s	.Nope

	move.w	d0,d3				; place xpos in d3
	lsr.w	#3,d3				; convert it into column address
	subq.w	#1,d3				; offset to above top of screen
	move.w	d3,d1				; make map offset
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	move.w	Scroll_XPos(a0),d0
	lsr.w	#3,d0
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2
	bsr	DownLoad4WayRow
.Nope:
	rts

;==============================================================================

Scroll4WayDown:
	move.w	Scroll_YPos(a0),d0
	addq.w	#1,d0
	cmp.w	Scroll_BotLimit(a0),d0
	bcc.s	.Nope
	move.w	d0,Scroll_YPos(a0)
	move.w	Field_Num(a0),d1
	bsr	SetVScroll
	move.w	d0,d1
	and.w	#$7,d1				; are we on char boundry?
	bne.s	.Nope				; if not, we are done

	move.w	d0,d3				; place xpos in d3
	lsr.w	#3,d3				; convert it into column address
	add.w	#VDP_SCREEN_HEIGHT,d3		; offset to bottom of screen
	move.w	d3,d1
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of comlunms

	move.w	Scroll_XPos(a0),d0		; get x offset so we start in right place
	lsr.w	#3,d0				; convert x & y into char offsets
	move.w	d0,d2
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of comlunms
	bsr	DownLoad4WayRow
.Nope:
	rts

;==============================================================================

Scroll4WayLeft:
	move.w	Scroll_XPos(a0),d0
	subq.w	#1,d0
	bmi.s	.Nope
	move.w	d0,Scroll_XPos(a0)
	neg.w	d0
	move.w	Field_Num(a0),d1
	bsr	SetHScroll
	move.w	Scroll_XPos(a0),d0
	move.w	d0,d1
	and.w	#$7,d1
	bne.s	.Nope

	move.w	d0,d2				; place xpos in d2
	lsr.w	#3,d2				; convert it into column address
	subq.w	#1,d2				; offset to above top of screen
	move.w	d2,d0				; make map offset
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of rows

	move.w	Scroll_YPos(a0),d1
	lsr.w	#3,d1
	move.w	d1,d3
	and.w	#VDP_FIELDA_HEIGHT-1,d3
	bsr	DownLoad4WayColumn
.Nope:
	rts

;==============================================================================

Scroll4WayRight:
	move.w	Scroll_XPos(a0),d0
	addq.w	#1,d0
	cmp.w	Scroll_RightLimit(a0),d0
	bcc.s	.Nope
	move.w	d0,Scroll_XPos(a0)
	neg.w	d0
	move.w	Field_Num(a0),d1
	bsr	SetHScroll
	move.w	Scroll_XPos(a0),d0
	move.w	d0,d1
	and.w	#$7,d1				; are we on char boundry?
	bne.s	.Nope				; if not, we are done

	move.w	d0,d2				; place xpos in d3
	lsr.w	#3,d2				; convert it into column address
	add.w	#VDP_SCREEN_WIDTH,d2		; offset to right of screen
	move.w	d2,d0
	and.w	#VDP_FIELDA_WIDTH-1,d2		; and with # of colunms

	move.w	Scroll_YPos(a0),d1		; get x offset so we start in right place
	lsr.w	#3,d1				; convert x & y into char offsets
	subq.w	#1,d3				; kts try
	move.w	d1,d3
	and.w	#VDP_FIELDA_HEIGHT-1,d3		; and with # of rows
	bsr.s	DownLoad4WayColumn
.Nope:
	rts

;==============================================================================
; Inputs:
;	a0-> scroll structure
;	d0.w = x position in map to start from(char)
;	d1.w = y positon in map to start from(char)
;	d2.w = column in field(in vdp) to draw to(char)
;	d3.w = row in field(in vdp) to draw to(char)
;------------------------------------------------------------------------------

Download4WayRow:
					; first find correct address in map
	move.l	Scroll_MapPtr(a0),a1
	mulu.w	Scroll_MapWidth(a0),d1
	add.w	d0,d1				; add in x offset
	add.w	d1,d1				; double since each entry is 2 bytes long
	add.w	d1,a1				; point to correct spot in map
					; now set correct address on field
	move.w	d3,d1
	move.w	d2,d0

	move.w	#VDP_SCREEN_WIDTH,d7
	move.w	d7,d6
	add.w	d2,d7				; get ending point
	sub.w	Field_Width(a0),d7
	bls.s	.Safe
						; d7 now contains # of columns past end of line to draw
	sub.w	d7,d6
	exg	d6,d7

	move.l	Scroll_ScreenAddrRout(a0),a2
	jsr	(a2)				; set screen address
					; now actually download new row
	move.w	Scroll_CharBase(a0),d1
	subq.w	#1,d7
.Loop:
	move.w	(a1)+,d0
	add.w	d1,d0
	move.w	d0,VDP_DATA
	dbra	d7,.Loop

	clr.w	d0
.Safe:
						; download second half
	move.w	d3,d1
	move.l	Scroll_ScreenAddrRout(a0),a2
	jsr	(a2)				; set field address
					; now actually download new row
	move.w	Scroll_CharBase(a0),d1
;	subq.w	#1,d6
.Loop2:
	move.w	(a1)+,d0
	add.w	d1,d0
	move.w	d0,VDP_DATA
	dbra	d6,.Loop2
	rts

;------------------------------------------------------------------------------
;	Inputs:
;	a0-> scroll structure
;	d0.w = x position in map to start from(char)
;	d1.w = y positon in map to start from(char)
;	d2.w = column in field(in vdp) to draw to(char)
;	d3.w = row in field(in vdp) to draw to(char)


Download4WayColumn:
	move.w	#VDP_REG_AUTOINCSIZE+(VDP_FIELDA_WIDTH*2),VDP_CMD		; set up auto-increment
				; first find correct address in map
	move.l	Scroll_MapPtr(a0),a1
	mulu.w	Scroll_MapWidth(a0),d1
	add.w	d0,d1				; add in x offset
	add.w	d1,d1				; double since each entry is 2 bytes long
	add.w	d1,a1				; point to correct spot in map
				; now set correct address on field
	move.w	d3,d1
	move.w	d2,d0

	move.w	#VDP_SCREEN_HEIGHT,d7
	move.w	d7,d6

	add.w	d3,d7				; get ending point
	move.w	d2,d3				; stash x offset
	sub.w	Field_Height(a0),d7
	bls.s	.Safe
						; d7 now contains # of columns past end of line to draw
	sub.w	d7,d6
	exg	d6,d7

	move.l	Scroll_ScreenAddrRout(a0),a2
	jsr	(a2)

	move.w	Scroll_CharBase(a0),d1
	move.w	Scroll_MapWidth(a0),d2
	add.w	d2,d2
	subq.w	#1,d7
.Loop:
	move.w	(a1),d0
	add.w	d2,a1				; skip to next row in map
	add.w	d1,d0
	move.w	d0,VDP_DATA
	dbra	d7,.Loop

	clr.w	d1
.Safe:
						; download second half
	move.w	d3,d0
	move.l	Scroll_ScreenAddrRout(a0),a2
	jsr	(a2)				; set field address
					; now actually download new row
	move.w	Scroll_CharBase(a0),d1
	move.w	Scroll_MapWidth(a0),d2
	add.w	d2,d2
;	subq.w	#1,d6
.Loop2:
	move.w	(a1),d0
	add.w	d2,a1
	add.w	d1,d0
	move.w	d0,VDP_DATA
	dbra	d6,.Loop2

	move.w	#VDP_REG_AUTOINCSIZE+2,VDP_CMD
	rts

;==============================================================================

	END

;==============================================================================
