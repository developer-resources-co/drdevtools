
; memview.asm

ZPAGE	SEGMENT

memDumpAddrStart	ds	3
memDumpAddr	ds	3
memInc		ds	3


CODE	SEGMENT

			;12345678901234567890123456789012
szMemoryViewer:	dt	'- Memory Viewer ----------------'
		db	0

MemoryViewer:
	mode16
	blank
	BG_COLOR	#$0F,#0,#$0D

	; Shift over to the left 1/2 character
	lda	#4
	a8
	sta	BG3HOFS
	xba
	sta	BG3HOFS
	unblank

	CURSOR	#0,#1
	PRINT_STRING	szMemoryViewer

	mode16
	lda	#0
	sta	memDumpAddrStart
	sta	memDumpAddrStart+1

.diss:
	WAITVB
	jsr	VBI
	mode16
	lda	memDumpAddrStart
	sta	memDumpAddr
	lda	memDumpAddrStart+1
	sta	memDumpAddr+1

	stz	memInc+1
	lda	#1
	sta	memInc

	ldy	#3

.next_y:
	mode16
	phy
	ldx	#1
	jsr	MoveCursor

	a8
	lda	memDumpAddr+2
	jsr	PrintHex8
	lda	#':'
	jsr	PrintChar
	lda	memDumpAddr+1
	jsr	PrintHex8
	lda	memDumpAddr
	jsr	PrintHex8
	lda	#' '
	jsr	PrintChar

	ldx	#(8-1)
.next_x:	lda	[memDumpAddr]
	jsr	PrintHex8
	lda	#' '
	jsr	PrintChar

	;----- Increment 24-bit memory pointer by 1
	clc
	a16
	lda	memDumpAddr
	adc	memInc
	sta	memDumpAddr
	a8
	lda	memDumpAddr+2
	adc	memInc+2
	sta	memDumpAddr+2

	dex
	bpl	.next_x

	ply
	iny
	cpy	#(3+24)
	bne	.next_y

	;----- Read user input and adjust address accordingly
	a16
	stz	memInc
	stz	memInc+1

;;;	jsr	GetJoyInput

	lda	JoyShadow
	bit	#JOYDOWN
	beq	.not_down
	lda	#8
	sta	memInc
.not_down:

	lda	JoyShadow
	bit	#JOYUP
	beq	.not_up
	lda	#-1
	sta	memInc+1
	lda	#-8
	sta	memInc
.not_up:

	lda	JoyEdgeDown
	bit	#JOYLBUTTON
	beq	.not_l_button
	lda	#<$FF0000
	sta	memInc
	lda	#>$FF0000
	sta	memInc+1
.not_l_button:

	lda	JoyEdgeDown
	bit	#JOYRBUTTON
	beq	.not_r_button
	lda	#<$010000
	sta	memInc
	lda	#>$010000
	sta	memInc+1
.not_r_button:

	lda	JoyEdgeDown
	bit	#JOYXBUTTON
	beq	.not_x_button
	lda	memDumpAddrStart
	eor	#$8000
	sta	memDumpAddrStart
.not_x_button:

	lda	JoyShadow
	and	#JOYSTART|JOYSELECT
	cmp	#JOYSTART|JOYSELECT
	bne	.not_start_select
	stz	memInc			; Don't move
	stz	memInc+1
	lda	memDumpAddrStart		; back to beginning of
	and	#$8000			; "bank", either 0 or $8000
	sta	memDumpAddrStart
.not_start_select:

	lda	JoyShadow
	bit	#JOYSTART
	beq	.not_start
	a8
	lda	#0
	pha
	a16
	lda	>$FFEC
	dec
	pha
	sec
	xce
	rtl
.not_start:

	;----- Last button press (it modified variables that have been
	;----- set by the buttons routines above)
	lda	JoyShadow
	bit	#JOYBBUTTON
	beq	.not_b_button
	lda	memInc
	ldx	#24
	jsr	MultiplySigned
	sta	memInc

.not_b_button:

	clc
	a16
	lda	memDumpAddrStart
	adc	memInc
	sta	memDumpAddrStart
	a8
	lda	memDumpAddrStart+2
	adc	memInc+2
	sta	memDumpAddrStart+2

	brl	.diss


	END
