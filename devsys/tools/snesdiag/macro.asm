mode16	macro
	rep	#$30
	longa	on
	longi	on
	endm

mode8	macro
	sep	#$30
	longa	off
	longi	off
	endm

A16	macro
	rep	#LONGA
	longa	on
	endm

A8	macro
	sep	#LONGA
	longa	off
	endm

X16	macro
	rep	#LONGI
	longi	on
	endm

X8	macro
	sep	#LONGI
	longi	off
	endm

asm16	macro
	longa	on
	longi	on
	endm

asm8	macro
	longa	off
	longi	off
	endm

blank	macro
	pha
	LONGA	PUSH
	php
	A8
	lda	#$80
	sta	$2100
	plp
	LONGA	POP
	pla

	endm

unblank	macro
	pha
	LONGA	PUSH
	php
	A8
	lda	#$0f
	sta	$2100
	plp
	LONGA	POP
	pla

	endm
waitvb	macro
	pha
	LONGA	PUSH
	php
	A8
?{L}loop1
	lda	$4210
	and	#$80
	bne	?{L}loop1
?{L}loop2
	lda	$4210
	and	#$80
	beq	?{L}loop2
	lda	$4210
	plp
	LONGA	POP
	pla
	endm

	end

