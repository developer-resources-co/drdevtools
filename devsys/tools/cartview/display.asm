; display.asm

Display	DEFS	ROM
Display	SEGMENT
Display_Mode		ds	1
Display_TM		ds	1
Display_Palette1		ds	3
Display_Palette1_NumColors	ds	1
Display_Palette1_Offset	ds	1
Display_Palette2		ds	3
Display_Palette2_NumColors	ds	1
Display_Palette2_Offset	ds	1
Display_Palette3		ds	3
Display_Palette3_NumColors	ds	1
Display_Palette3_Offset	ds	1
Display_Palette4		ds	3
Display_Palette4_NumColors	ds	1
Display_Palette4_Offset	ds	1
Display_SIZEOF



ZPAGE	SEGMENT
displayPtr	ds	3


CODE	SEGMENT

	asm16
;
; DisplayInit: (CALL DURING VIDEO BLANK)
; A -> DisplayStructure
;
DisplayInit:
	PUSH_ALL

	x8
;;	sta	displayPtr
	a8
	yLDA	[displayPtr],#Display_Mode
	sta	BG_MODE
	yLDA	[displayPtr],#Display_TM
	sta	TM

;;	jsr	BgZero		;???

	mode16

;?	jsr	CmdClearBg1
;?	jsr	CmdClearBg2

	lda	#384
	;----- clear BG's
	ldx	#(1024*2)-2
.clr3:
	sta	BG3_0,x
	dex
	dex
	bpl	.clr3


	;----- Palettes
	; 1
;;	a16
	x8
	yLDA	[displayPtr],#Display_Palette1
	sta	lpPalette
	yLDA	[displayPtr],#Display_Palette1+1
	sta	lpPalette+1
	ora	lpPalette
	beq	.p1
	a8
	yLDA	[displayPtr],#Display_Palette1_NumColors
	tax
	yLDA	[displayPtr],#Display_Palette1_Offset
	tay
	jsr	NewSetPalette
.p1:

	; 2
	yLDA	[displayPtr],#Display_Palette2
	sta	lpPalette
	yLDA	[displayPtr],#Display_Palette2+1
	sta	lpPalette+1
	ora	lpPalette
	beq	.p2
	a8
	yLDA	[displayPtr],#Display_Palette2_NumColors
	tax
	yLDA	[displayPtr],#Display_Palette2_Offset
	tay
	jsr	NewSetPalette
.p2:

	; 3
	yLDA	[displayPtr],#Display_Palette3
	sta	lpPalette
	yLDA	[displayPtr],#Display_Palette3+1
	sta	lpPalette+1
	ora	lpPalette
	beq	.p3
	a8
	yLDA	[displayPtr],#Display_Palette3_NumColors
	tax
	yLDA	[displayPtr],#Display_Palette3_Offset
	tay
	jsr	NewSetPalette
.p3:

	; 4
	yLDA	[displayPtr],#Display_Palette4
	sta	lpPalette
	yLDA	[displayPtr],#Display_Palette4+1
	sta	lpPalette+1
	ora	lpPalette
	beq	.p4
	a8
	yLDA	[displayPtr],#Display_Palette4_NumColors
	tax
	yLDA	[displayPtr],#Display_Palette4_Offset
	tay
	jsr	NewSetPalette
.p4:

	PULL_ALL
	rts


	END

