;==============================================================================
; CSTUFF.asm -- Nothing but libraries + jump to _main
;       By Scott L. Statton & Eric Yiskis
;   3/22/93 T. Johnson
;     added call to load_tbl to copy data and create space for bss
;==============================================================================

	XDEF	Stack
	XDEF	Start
	XDEF	HBlankHandler
	XDEF	VBlankHandler
	xref	load_tbl
	XREF	_HBlank
	XREF	_VBlank

	TSECTION        .text

Stack   equ     $00ffef00

	XREF	_main
Start:
	jsr	load_tbl
	jmp	_main

HBlankHandler:
	jsr	_HBlank
	rte
VBlankHandler:
	jsr	_VBlank
	rte

	END



;==============================================================================
