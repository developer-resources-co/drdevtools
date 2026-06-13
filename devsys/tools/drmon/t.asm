
	lORG	$C0_0000
	porg	$00_0000

	rep	#$30

	jsr	.1
	jsr	.2

	clc
	bcc	.3

	brk

.3:	lda	#0
	beq	.4

	brk

.4:
	lda	#4
	brk

.1:	lda	#1
	rts

.2:	lda	#2
	rts
