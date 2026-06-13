
;============================================================================

ZPAGE   SEGMENT
bounds_lower    ds      2
bounds_upper    ds      2


BOUND   MACRO
	ldx     {1}
	stx     bounds_lower

	ldx     {2}
	stx     bounds_upper

	cmp     bounds_lower
	bcs     .10{L}
	lda     bounds_lower

.10{L}
	IF 0
	cmp     bounds_upper
	bcc     .20{L}
	lda     bounds_upper

.20{L}
	ENDIF

	ENDM


	END

