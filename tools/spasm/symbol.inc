;===============================================================================
;
; File:		symbol.inc
;
; Project:	Developer Resources Assembler
; (c) 1992 Adept Creations.  All Rights Reserved.
;
; History:
;	29 Nov 92	WBNIV	Created
;
;===============================================================================

;
; SYMBOL STRUCTURE FORMAT:
;	(add size)
;  0--26 symbol name
;  1B 27 (new) flags			; note: coincides with mac_sigoffset,
					; so cannot contain $ff if not a macro
;  1C 28\
;  1D 29 | symbol value (32-bits)
;  1E 30 |
;  1F 31/

SYMBOL_LENGTH	EQU	26

symbol STRUC
symbol_name	db 26+1 DUP (?)
symbol_flag	db ?			; 80:LA 40:LI
symbol_val_l		dw ?
symbol_val_h		dw ?
symbol_SizeOf	LABEL
symbol ENDS

;=============================================================================

; MACRO
;  0--$15 macro name
;  1A m
;  1B m
;  1C\ address of macro text (ENDM terminated)
;  1D/
;  1E
;  1F
;	MACRO TEXT SPACE IS $2F00 BYTES (WHITESPACE STRIPPED)

mac STRUC
mac_name	db MACRO_NAME_LEN+1 DUP (?)	; 0-1a
mac_sigoffset	dw ?				; 1b-1c
mac_pad		db ?				; 1d
mac_ptr		dw ?				; 1e-1f
mac_SizeOf	LABEL
mac ENDS

	ERRIF	mac_SizeOf NE symbol_SizeOf "macro structure size doesn't match symbol structure"

;=============================================================================

SEGM STRUC
symbol_segment_name		db 15 dup (?)
symbol_segment_type		db ?
symbol_segment_bottom_l	dw ?
symbol_segment_bottom_h	db ?

symbol_segment_top_l	dw ?
symbol_segment_top_h	db ?

symbol_segment_count_l	dw ?		; space left (set at SIZE directive)
symbol_segment_count_h	db ?

symbol_segment_delta_l	dw ?
symbol_segment_delta_h	db ?

symbol_segment_lorg_l	dw ?
symbol_segment_lorg_h	dw ?
symbol_segment_SizeOf	LABEL
SEGM ENDS

	ERRIF	symbol_segment_SizeOf NE symbol_SizeOf "segment nstructure size doesn't match symbol structure"


; different segment types

SEGTYPE ENUM {
	SEGTYPE_RAM = 1
	SEGTYPE_ROM
	SEGTYPE_DATA	      	; cannot assemble code into a data segment
	SEGTYPE_COUNT
;SEGTYPE_RAM	EQU	1
;SEGTYPE_ROM	EQU	2
;SEGTYPE_DATA	EQU	3		; cannot assemble code into a data segment
;SEGTYPE_MAX	EQU	4
}
;SEGTYPE_STRUCT	EQU	4		;!!!

;
; SEGMENT
;  0--0E segment name (up to 0-E)
;  0F segment type (was: next byte is segment type)
;	1: RAM
;	2: ROM
;
;	65816			68000
;  10 (unused)		10 \
;  11 \			11  | sskb ($FFFFFF)
;  12  | sskb ($FFFFFF)	12 /
;  13 /			13 \
;  14 \			14  | sskt ($000000)
;  15  | sskt ($000000)	15 /
;  16 /			16 \
;  17 \			17  | count (size)
;  18  | count (size)	18 /
;  19 /			19 \
;  1A \			1A  | LORG - PORG (delta)
;  1B  | LORG - PORG (delta)	1B /
;  1C /			1C \
;  1D \			1D  |
;  1E  | LORG 		1E  | LORG
;  1F /			1F /

