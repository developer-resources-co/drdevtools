;
;		 ****************************
;		 * SPASM/68000/65816/SPC700 *
;		 ****************************
;
;		   The Single Pass ASeMbler
;
;			    or
;
;		     The SPeedy ASeMbler
;
;			    or
;
;		 The Super Phamicom ASeMbler
;		    The SPooling ASeMbler
;
;		      but in any case...
;
;      (C) 1991,92,93,94 Developer Resources.  All Rights Reserved.
;


; This update: 18 FEBRUARY 1992


; SPASM is to a conventional assembler
; as a microwave is to a conventional oven.

; yea, it's all hot in the middle, and cold on the edges.
; (read: super-fast, but no rept command :-)

; Confucious say: "You get what you pay for."

; It may not have cost me money, but it sure cost me time

; Ask and ye shall receive (sometimes)

;==============================================================================

	.286
	MODEL	SMALL
	LOCALS
	SMART
	JUMPS
.CODE

	ASSUME	DS:DATA,ES:NOTHING

;=============================================================================

	INCLUDE	processor.inc
	include	spasm.inc

;=============================================================================

Cprefix	macro
	push	bp
	mov	bp,sp
	add	bp,4
endm

Cpostfix	macro
	pop	bp
	ret
endm

push_all	macro
	pusha
endm

pop_all	macro
	popa
endm

;==============================================================================
; switches

_SPOOL_BACKPATCH:

	;----- Define switches here
	ifdef	GENESIS
MOTOROLA:				; word order
_68000:
	endif

	ifdef	SNES
INTEL:				; word order
_65816:
	endif

	ifdef	SPC700
INTEL:				; word order
_SPC700:
	endif

;=============================================================================

	INCLUDE	symbol.inc

	include	assemble.asm	; general portion of assembler

	include	patch.asm		; backpatching code
	include	cint.asm		; C interface
	include	parse.asm		; input line parsing
	include	print.asm		; text printing routines
	include	fileio.asm	; dos interface
	include	symbol.asm	; symbol handling
	include	error.asm		; error handling
	include	list.asm		; listing functions
	include	macro.asm		; macro handling functions

	ifdef	GENESIS
	include	opcode68.asm	; opcode assembler
	include	cam68.asm		; addressing mode validation
	endif

	ifdef	SNES
	include	opcode816.asm	; opcode assembler
	include	cam816.asm	; addressing mode validation
	include times816.asm	; opcode execution timings
	endif

	ifdef	SPC700
	include	opcode700.asm	; opcode assembler
	include	cam700.asm	; addressing mode validation
	endif

	include	board.asm		; board I/O
	include	pseudo.asm          ; pseudo ops

;==============================================================================

	end

;==============================================================================

