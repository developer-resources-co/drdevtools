;============================================================================
;
;
;			Music Driver
;
;============================================================================

; Variables

SongPointer:	.dw	0		; Ptr to song
NotePointer:	.dw	0		; Ptr to current note
TempoCount:	.dw	0		; Counter for tempo
Tempo:		.db	120		; Song Tempo
Duration:	.db	1		; Command duration counter
Velocity:	.db	255		; Default velocity
Volume:		.db	255		; Global volume
Transpose:	.db	0		; Note transpose
Saved:		.dw	0		; Store for Gosub command


; Note equates

NOTE_C0  = 36
NOTE_CS0 = 37
NOTE_D0  = 38
NOTE_DS0 = 39
NOTE_E0  = 40
NOTE_F0  = 41
NOTE_FS0 = 42
NOTE_G0  = 43
NOTE_GS0 = 44
NOTE_A0  = 45
NOTE_AS0 = 46
NOTE_B0  = 47
NOTE_C1  = 48
NOTE_CS1 = 49
NOTE_D1  = 50
NOTE_DS1 = 51
NOTE_E1  = 52
NOTE_F1  = 53
NOTE_FS1 = 54
NOTE_G1  = 55
NOTE_GS1 = 56
NOTE_A1  = 57
NOTE_AS1 = 58
NOTE_B1  = 59

NOTE_C2  = 60
NOTE_CS2 = 61
NOTE_D2  = 62
NOTE_DS2 = 63
NOTE_E2  = 64
NOTE_F2  = 65
NOTE_FS2 = 66
NOTE_G2  = 67
NOTE_GS2 = 68
NOTE_A2  = 69
NOTE_AS2 = 70
NOTE_B2  = 71

NOTE_C3  = 72
NOTE_CS3 = 73
NOTE_D3  = 74
NOTE_DS3 = 75
NOTE_E3  = 76
NOTE_F3  = 77
NOTE_FS3 = 78
NOTE_G3  = 79
NOTE_GS3 = 80
NOTE_A3  = 81
NOTE_AS3 = 82
NOTE_B3  = 83

NOTE_C4  = 84
NOTE_CS4 = 85
NOTE_D4  = 86
NOTE_DS4 = 87
NOTE_E4  = 88
NOTE_F4  = 89
NOTE_FS4 = 90
NOTE_G4  = 91
NOTE_GS4 = 92
NOTE_A4  = 93
NOTE_AS4 = 94
NOTE_B4  = 95

NOTE_C5  = 96
NOTE_CS5 = 97
NOTE_D5  = 98
NOTE_DS5 = 99
NOTE_E5  = 100
NOTE_F5  = 101
NOTE_FS5 = 102
NOTE_G5  = 103
NOTE_GS5 = 104
NOTE_A5  = 105
NOTE_AS5 = 106
NOTE_B5  = 107

; Music channel equates

COM_CH0 = 0x0
COM_CH1 = 0x1
COM_CH2 = 0x2
COM_CH3 = 0x3
COM_CH4 = 0x4
COM_CH5 = 0x5
COM_CH6 = 0x6
COM_CH7 = 0x7
COM_CH8 = 0x8
COM_CH9 = 0x9
COM_CHa = 0xa
COM_CHb = 0xb
COM_CHc = 0xc
COM_CHd = 0xd
COM_CHe = 0xe
COM_CHf = 0xf

; Music command equates

COM_NOOP	=	0x00
COM_NOTE	=	0x10
COM_VNOTE	=	0x20
COM_PATCH	=	0x30
COM_VOLUME	=	0x40
COM_VELOCITY	=	0x50
COM_TEMPO	=	0x60
COM_TRANSPOSE	=	0x70
COM_SETLOOP	=	0x80
COM_GOTO	=	0x90
COM_GOSUB	=	0xa0
COM_RETURN	=	0xb0
COM_RESTART	=	0xc0
COM_END		=	0xd0


; Jump table for music commands
;
MusicJump:
	.dw	Noop
	.dw	NoteOn
	.dw	VNoteOn
	.dw	SetPatch
	.dw	SetVolume
	.dw	SetVelocity
	.dw	SetTempo
	.dw	SetTranspose
	.dw	SetLoop
	.dw	Goto
	.dw	Gosub
	.dw	Return
	.dw	Restart
	.dw	MusicEnd
	.dw	Noop
	.dw	Noop


;============================================================================
;
;
;			Start Song
;
;
;============================================================================

StartSong:
	call	SoundOff

	ld	hl,#SongBuffer
	ld	(SongPointer),hl
	ld	(NotePointer),hl
	ld	a,#1
	ld	(Duration),a
	ret


;============================================================================
;
;
;		Music driver Tick
;
;
;============================================================================

; Call once per frame (60Hz)
;
MusicTick:
	ld	hl,(SongPointer)	; Get pointer to song
	ld	a,h			; If 0, then no song playing
	or	l
	ret	Z

; Decrement command duration

	ld	a,(Duration)
	dec	a
	ld	(Duration),a
	ret	NZ

; Process next event

	ld	ix,(NotePointer)	; Get music PC

NextEvent:
	call	UpdateDAC
	ld	a,(ix)			; Get opcode
	and	#0xf0
	rrca
	rrca
	rrca

	ld	e,a		; Index for vector
	ld	d,#0
	ld	hl,#MusicJump
	add	hl,de
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	jp	(hl)		; Jump to vector


;============================================================================
;
;		No-op Command
;
;============================================================================

Noop:
	inc	ix
	ld	a,(ix)		; Get parameter
	ld	(Duration),a
	inc	ix
	and	a
	jr	Z,NextEvent
	ld	(NotePointer),ix
	ret


;============================================================================
;
;
;		NoteOn Command
;
;
;============================================================================

; Parameters,
; Channel No. (in Opcode lo nibble)
; Event Time
; Pitch (0-127)
; Note length
;
NoteOn:
	ld	a,1(ix)		; Store event duration
	ld	(Duration),a

	ld	a,(Transpose)	; Get transpose
	ld	b,2(ix)		; Get pitch
	add	a,b		; Combine & Store in B
	ld	b,a

	ld	c,3(ix)		; Get note length

	ld	a,(Velocity)	; Use defualt velocity
	ld	d,a

	ld	a,(ix)
	and	#0x0f		; Get channel no.
	ld	e,a

	ld	l,#0xc0		; Stereo = Centre
	ld	h,#0x80		; Priority = Centre

	push	ix
	call	StartNote	; Start note
	pop	ix

	ld	de,#4
	add	ix,de

	ld	a,(Duration)
	and	a
	jr	Z,NextEvent
	ld	(NotePointer),ix
	ret



;============================================================================
;
;
;		Velocity NoteOn Command
;
;
;============================================================================

; Parameters,
; Channel No. (in Opcode lo nibble)
; Event Time
; Pitch (0-127)
; Note length
; Velocity
;
VNoteOn:
	ld	a,1(ix)		; Store event duration
	ld	(Duration),a

	ld	a,(Transpose)	; Get transpose
	ld	b,2(ix)		; Get pitch
	add	a,b		; Combine & Store in B
	ld	b,a

	ld	c,3(ix)		; Get note length
	ld	d,4(ix)		; Get velocity

	ld	a,(ix)
	and	#0x0f		; Get channel no.
	ld	e,a

	ld	l,#0xc0		; Stereo = Centre
	ld	h,#0x80		; Priority = Centre

	push	ix
	call	StartNote	; Start note
	pop	ix

	ld	de,#5
	add	ix,de

	ld	a,(Duration)
	and	a
	jp	Z,NextEvent
	ld	(NotePointer),ix
	ret


;============================================================================
;
;
;		Set Patch Command
;
;
;============================================================================

SetPatch:
	ld	a,(ix)
	and	#0x0f
	ld	c,a
	ld	b,#0
	ld	hl,#LChanBase
	add	hl,bc
	ld	a,1(ix)
	ld	(hl),a
	inc	ix
	inc	ix
	jp	NextEvent


;============================================================================
;
;
;		Set Volume Command
;
;
;============================================================================

SetVolume:
	ld	a,1(ix)
	ld	(Volume),a
	inc	ix
	inc	ix
	jp	NextEvent

;============================================================================
;
;
;		Set Velocity Command
;
;
;============================================================================

SetVelocity:
	ld	a,1(ix)
	ld	(Velocity),a
	inc	ix
	inc	ix
	jp	NextEvent

;============================================================================
;
;
;		Set Tempo Command
;
;
;============================================================================

SetTempo:
	ld	a,1(ix)
	ld	(Tempo),a
	inc	ix
	inc	ix
	jp	NextEvent

;============================================================================
;
;
;		Set Transpose Command
;
;
;============================================================================

SetTranspose:
	ld	a,1(ix)
	ld	(Transpose),a
	inc	ix
	inc	ix
	jp	NextEvent

;============================================================================
;
;
;		Set Loop Command
;
;
;============================================================================

SetLoop:
	push	ix
	pop	hl
	ld	(SongPointer),hl	; Set new top of song
	inc	ix
	jp	NextEvent

;============================================================================
;
;
;		Goto Command
;
;
;============================================================================

Goto:
	ld	l,1(ix)		; Get address
	ld	h,2(ix)
	push	hl
	pop	ix
	jp	NextEvent


;============================================================================
;
;
;		Gosub Command
;
;
;============================================================================

Gosub:
	ld	l,1(ix)		; Get address
	ld	h,2(ix)
	inc	ix
	inc	ix
	inc	ix
	ld	(Saved),ix
	push	hl
	pop	ix
	jp	NextEvent

;============================================================================
;
;
;		Return Command
;
;
;============================================================================

Return:
	ld	ix,(Saved)
	jp	NextEvent

;============================================================================
;
;
;		Restart Command
;
;
;============================================================================

Restart:
	ld	ix,(SongPointer)
	jp	NextEvent

;============================================================================
;
;
;		MusicEnd Command
;
;
;============================================================================

MusicEnd:
	ld	hl,#0
	ld	(SongPointer),hl
	ret

