;==============================================================================
; exit.asm
;  just to make the compiler happy
;==============================================================================

	TSECTION        .text           ;e changed from code segment

	XREF	_ResetGenesis


	XDEF	__exit
__exit:
	bra	_ResetGenesis

	end

