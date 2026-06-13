;==============================================================================
;=== Hiscore.asm: high score library
;==============================================================================
;	incomplete, started for Pigskin, needs work
;==============================================================================

HISCORE_COUNT	EQU	10			; number of entries in high score table
HISCORE_NUMCHARS	EQU	3		; # of characters in each name
HISCORE_NUMDIGITS	EQU	5		; # of digits of score to print
HISCORE_FILLCHAR	EQU	'A'		; character to fill in un-entered names with

;==============================================================================

HiScEntry_Str	defs	ram
HiScEntry_Str	segment
	org	0
HiScEntry_Score	ds.l	1			; room for a pretty big score
HiScEntry_Name	ds.b	HISCORE_NUMCHARS	; space for high score name
		even				; align struct
HiScEntry_Size					; size of struct

;------------------------------------------------------------------------------

HiScore_Struct	defs	ram
HiScore_Struct	segment
	org	0
HiScore_Array:
	ds.b	HiScEntry_Size*HISCORE_COUNT
HiScore_Size

;==============================================================================

code	segment

;==============================================================================
; InitHiScore:
;	Inputs:
;		a0-> HiScore Struct
;		a1-> array of default names(HISCORE_NUMCHARS chars each)
;		a2-> array of default scores(4 bytes each)
;------------------------------------------------------------------------------

InitHiScore:
	add.w	#HiScore_Array,a0			; skip forward to array

	move.w	#HISCORE_COUNT-1,d2
.Loop:
	bsr.s	.InitHiScoreEntry
	add.w	#HiScEntry_Size,a0		; move forward to next entry
.Entry:
	dbra	d2,.Loop
	rts

;------------------------------------------------------------------------------

.InitHiScoreEntry:
	move.l	(a2)+,HIScEntry_Score(a0)			; copy default score

	move.w	#HISCORE_NUMCHARS-1,d0
	clr.w	d1
.InitLoop:
	move.b	(a1)+,HiScEntry_Name(a0,d1.w)			; copy default name
	addq.w	#1,d1
	dbra	d0,.InitLoop
	rts

;==============================================================================
; EnterHiScore: prompt user for name
;	Inputs:
;		a0-> high score structure
;		a1-> temporary high score entry containing player information
;			(I do it this way so the game can insert information into
;	       		the high score structure, and the library will preserve it,
;			since it does a bit copy into the high score arrray)
;		a2-> input structure
;		cursorX & cursorY = coordinate to begin entering name
;------------------------------------------------------------------------------

EnterHiScore:
	movem.l	a0/a1,-(sp)

	lea	HiScEntry_Name(a1),a0
	move.b	#HISCORE_FILLCHAR,d0
	move.w	#HISCORE_NUMCHARS,d1
	bsr	FillString

	move.l	a2,a1
	move.w	#HISCORE_NUMCHARS,d0
	jsr	GetTextString

	movem.l	(sp)+,a0/a1
	jsr	InsertHiScore
	rts

;==============================================================================
; IsHighScore: determine if new score is in top scores
; 	Inputs:
;		a0-> high score struct
;		a1-> new high score entry
;	Outputs:
;		cc = ok, cs = not in list
;           	d0 = ranking in high score area
;	Destroys:
;		d0.w,d1.w,d7.w,a0,a1
;==============================================================================

IsHighScore:
			; first, find where in the high score table to insert the entry
	move.w	#HISCORE_COUNT-1,d7
	move.w	#1,d0
.Loop:
	move.w	HiScEntry_Score(a1),d1
	cmp.w	HiScEntry_Score(a0),d1
	bcc.s	.FoundIt
	lea	HiScEntry_Size(a0),a0
	addq.w	#1,d0
	dbra	d7,.Loop
				; does not fit in list
	or	#CCRF_CARRY,CCR		; set carry flag
.FoundIt:
	rts

;==============================================================================
; InsertHiScore: insert high score into table
; 	Inputs:
;		a0-> high score struct
;		a1-> new high score entry
;------------------------------------------------------------------------------

InsertHiScore:
	bsr.s	IsHighScore
	bcs.s	.Nope

			; now, move all the other entries down
	movem.l	a0/a1,-(sp)
	move.w	d7,d0
	lea	HiScEntry_Size(a0),a1
	mulu	#HiScEntry_Size,d0
	jsr	ReverseMemCopy
	movem.l	(sp)+,a0/a1
			; finally, copy the high score into the table
	exg	a0,a1
	move.w	#HiScEntry_Size,d0
	jsr	MemCopy
.Nope:
	rts

;==============================================================================
; PrintHiScore:
;	Inputs:
;		a0->HiScore struct
;		d0.w = starting row to print scores & names
;		d1.w = starting column to print names
;		d2.w = starting column to print scores
;		textAttr = attributes to print scores & names with
;		text library must be properly initialized
;------------------------------------------------------------------------------

PrintHiScore:
	add.w	#HiScore_Array,a1			; skip forward to array

	move.w	d0,cursorY
	move.w	#HISCORE_COUNT-1,d7
	move.l	a0,a1
.Loop:
	movem.w	d1-d2,-(sp)				; save x positions
	move.w	d1,cursorX
	lea.l	HiScEntry_Name(a1),a0
	move.w	#HISCORE_NUMCHARS,d0
	jsr	PrintTextCount
	move.w	(sp),cursorX				; get d2 back
	move.l	HiScEntry_Score(a1),a0
	move.w	#HISCORE_NUMDIGITS,d1
;	jsr	PrintDecimalCount			; not used in pigskin
	movem.w	(sp)+,d1-d2				; restore x positions
	add.w	#HiScEntry_Size,a1			; move forward to next entry
	addq.w	#1,cursorY
	ifdef	HISCORE_ISOLATENUMBER1
	cmp.w	#HISCORE_COUNT-1,d7
	bne.s	.Nope
	addq.w	#1,cursorY
.Nope:
	endif
	dbra	d7,.Loop
	rts

;==============================================================================

	end

;==============================================================================
