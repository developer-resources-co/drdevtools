; ccycle.asm

	INCLUDE	lib/snes.mac

	LIST	ON


CODE	DEFS	ROM
CODE	SEGMENT
	size	$008000
	org	$008000
	porg	$000000

	dl	$400
	dw	ColorCycleEnd - ColorCycle

	LORG	$400
ColorCycle:
	a8
	lda	$FF
	sta	>$2122
	lda	$FF
	sta	>$2122
	inc	$FF
	bra	ColorCycle
ColorCycleEnd:

	END
