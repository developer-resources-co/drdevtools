; ccycle.asm

	INCLUDE	lib/snes.mac
	INCLUDE	lib/snes.equ

	LIST	ON


CODE	DEFS	ROM
CODE	SEGMENT
	size	$008000
	org	$008000
	porg	$000000

bootstrap:
	a8
   	LDA	#$30
	STA	|$2130
	lda	#%00001111
	sta	INIDISP

	a8
ColorCycle:
	lda	$FF
	sta	>$2122
	lda	$FF
	sta	>$2122
	inc	$FF
	bra	ColorCycle
ColorCycleEnd:

VBI:	rti

	include	lib/vector.asm

	END
