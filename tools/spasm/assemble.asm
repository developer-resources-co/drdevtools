;==============================================================================
; assemble.asm: general portion of assembler
;==============================================================================

;==============================================================================
DATA	SEGMENT	'DATA'
;------------------------------------------------------------------------------

MNE_FIELD	DB	MNE_FIELD_LEN DUP (0)
MNE_LEN		DW	0
OVERRIDE	DB	0

PARFL1		DB	0
PARAM_FIELD	DB	MAX_PARAM_FIELD_LEN+1 DUP (0)
PARAM_LAST	DW	0

;=============================================================================
; macro varaiables

MACSEG		DW	0
MACENDPTR	DW	0		; ptr to next availible byte in macro buffer

MACLEVEL	DW	0FFFFH		;4 LEVELS

MACPTR		DW	MACRO_NEST_MAX DUP (0)
MACPARAM	DB	MACRO_NEST_MAX*MACRO_MAX_PARAMS*MACRO_PARAM_LEN DUP(0)
MACLINE		DW	MACRO_NEST_MAX DUP (0)
MACPC		DW	MACRO_NEST_MAX DUP (0)	;"MACRO PARAMETER COUNTER"

	; local variable of MACPC when build macro
MACPA		DW	0		;"MACRO PARAMETER ACCUMULATOR" [KLUDGE](# OF PARAMETERS IN MACRO?)

	ifdef	GENESIS
MACEXT		DB	MACRO_NEST_MAX DUP(20H)	;MACRO EXTENSION
	endif
MACREPTLEVEL	DW	MACRO_NEST_MAX DUP(0)	; rept level when entering macro

;=============================================================================
; org's
OBJPTRL		DW	0
OBJPTRH		DW	0
;OBJPTRH		DB	0
;		DB	0	; ensure 32-bits for expression parser
	;NOTE: take out fakePC references (?)

OBJPTRL_BFA	DW	0
OBJPTRH_BFA	DW	0
;OBJPTRH_BFA	DB	0

OBJSEG		DW	0

AMNESIA		DB	'Not enough memory$'


BUFSEG		DW	0	; Buffer for code
BUFSEG2		DW	0	; Buffer for INCBIN / INCSYM

CSPTR		DW	0		; ?segment ptr?
CSTYPE		DB	2

CSCOUNTL	DW	0FFFFH
CSCOUNTH	DW	0FFFFH

DELTA_L		DW	0
DELTA_H		DW	0

EXPRESSION	DB	MAX_PARAM_FIELD_LEN+1 DUP (0)	; since PARAM_FIELD can be copied into EXPRESSION
EXPSTART	DW	0
EXPEND		DW	0

LFRSEG		DW	0	; LOCAL FORWARD REFERENCE
LFRPTR		DW	0
LFREND		DW	0

LBASE		DW	0
LINE		DB	100H DUP (0)	; Source code (whitespace compressed and capitalized)
LINE_COUNT	DW	5 DUP (0)	;5 LEVELS
LINELEN		DW	0
LLCHR		DB	'.'

SEYES		DB	0	;USUALLY:  00H=>BYTE, 40H=>WORD, 80H=>LONG

				; assembler bit fields
SDB1		DB	0	;80:LA 40:LI 20:ZT 10:FH 8:MC  4:FR  2:MT 1:LFH
SDB2		DB	0	;80:SM 40:?? 20:SF 10:BS 8:LST 4:LFR 2:SD 1:ERR

; lfh = local fill holes
; fr = forward reference
; mc = macro
; fh = fill holes(set by fill_holes)

; zt = zero truncate, used by decimal print routine to remove extra zeros at begining of printed number


; bs = byte assembled, set by store_bin
; err = errors flag, 1 = yes

; sf = source debug flag, if true, we are in a new file

; see opcode*.asm for bits definitions with ??

; sfxmap 0=off, 1 = on


SDB1B_MACRO    		EQU	3
SDB1F_MACRO		EQU	1 SHL SDB1B_MACRO

SDB1B_LOCALFILLHOLES	EQU	0
SDB1F_LOCALFILLHOLES	EQU	1 SHL SDB1B_LOCALFILLHOLES

SDB1B_FILLHOLES		EQU	4
SDB1F_FILLHOLES		EQU	1 SHL SDB1B_FILLHOLES

;==============================================================================
; new improved flags

assembleFlags	db	0


AFB_FILLFILE	equ	0
AFF_FILLFILE	equ	1 SHL AFB_FILLFILE

AFB_LISTLINE	equ	1		; output line to listing file
AFF_LISTLINE	equ	1 SHL AFB_LISTLINE


DATA	ENDS

;==============================================================================
.CODE
;------------------------------------------------------------------------------

SPAZ	PROC	FAR
	MOV	AX,DATA		;POINT DS @ DATA SEGMENT
	MOV	DS,AX

	CALL	INIT_XTRA		;INITIATE EXTRA SEGMENTS
;;	CALL	init_predefined_symbols	; make sure line #, etc.
				; are initialized to zero after here

	CALL	OPEN_SOURCE_FILE	;OPEN THE SOURCE CODE FILE

@@AssembleLoop:
	or	assembleFlags,AFF_LISTLINE

	CALL	GET_LINE		;READ ONE LINE
	JC	@@Done		;JUMP IF END OF TOP FILE

	TEST	SDB2,8		;IS LIST FILE ON?
	JZ	@@NoList
	CALL	LIST_START	;IF SO THEN PREPARE LIST_PREFIX

@@NoList:
	CALL	ASSEMBLE_LINE	;ASSEMBLE THE LINE
	ifdef	GENESIS
	TEST	OBJPTRL,1		;...IF ODD ADDRESS...
	JZ	@@NoBitch
	TEST	SDB2,40H		;...AND CODE ASSEMBLED?
	JNZ	@@NoBitch
	CMP	ODDADDR_BITCH,0	;CAN WE BITCH...
	JZ	@@NoBitch
	MOV	AX,55
	CALL	ERROR
@@NoBitch:
	endif

	TEST	SDB2,8                ;IF LIST FILE ON THEN FINISH
	JZ	@@NoListEnd
	test	assembleFlags,AFF_LISTLINE	; and this line hasn't
	jz	@@NoListEnd			;  inhibited listing file
	CALL	LIST_END
@@NoListEnd:
	CMP	SRC_LEVEL,0FFFFH	;JUMP IF END OF TOP FILE
	JZ	@@Done
	JMP	@@AssembleLoop		;LOOP
@@Done:
	CALL	FILL_HOLES	;FILL IN ANY FORWARD REFERENCES
	CMP	_SYMFLAG,0	;WRITE SYM FILE IF WANTED
	JZ	@@NoSym
	CALL	WRITE_SYM
@@NoSym:
	JMP	TERMINATE		;FINISHED
SPAZ	ENDP

;==============================================================================

ASSEMBLE_LINE	PROC	NEAR

	AND	SDB1,NOT 4		;CLEAR "FR" FLAG
	AND	SDB2,NOT 15H		;CLEAR "BS", "LFR", "ERR" FLAGS

	MOV	AX,OBJPTRL
	MOV	OBJPTRL_BFA,AX
	MOV	AX,OBJPTRH
	MOV	OBJPTRH_BFA,AX

	CALL	BLIF		;BREAK LINE INTO FIELDS
	JNC	@@no_error
	RET

@@no_error:
	MOV	BL,IF_LEVEL	;CONSTRAINED BY "IF" CONDITION?
	OR	BL,BL
	JZ	@@NotIf
					; in ifed out section of code
	XOR	BH,BH
	CMP	[IF_FLAG+BX],0
	JNZ	@@NotIf

	CMP	WORD PTR [MNE_FIELD],'NE'   ;"TURBOSCAN"
	JNZ	@@TurboScan
	CMP	WORD PTR [MNE_FIELD+2],'ID'
	JNZ	@@TurboScan
	CMP	[MNE_FIELD+4],'F'
	JNZ	@@TurboScan
	JMP	POP_IF
						; if here, code is if'ed out
@@TurboScan:    				; scan through source looking for end
						; to condition
							; check for else
	CMP	WORD	PTR	[MNE_FIELD],'LE'
	JNZ	@@NotELSE
	CMP	WORD	PTR	[MNE_FIELD+2],'ES'
	JNZ	@@NotELSE
	XOR	BH,BH
	MOV	BL,IF_LEVEL
	mov	al,[IF_FLAG+BX]
	xor	al,1
	mov	[IF_FLAG+BX],al
	jmp	@@Ret
@@NotELSE:

	CMP	WORD	PTR	[MNE_FIELD],'FI'
	jz	@@not_SkipLine
	jmp	@@SkipLine
@@not_SkipLine:
	CMP	[MNE_FIELD+2],0
	JZ	@@PushIf
							; check for ifndef
	CMP	WORD	PTR	[MNE_FIELD+2],'DN'
	JNZ	@@NotIFNDE
	CMP	BYTE	PTR	[MNE_FIELD+4],'E'
	JNZ	@@NotIFNDE
	jmp	@@PushIf
@@NotIFNDE:

	CMP	WORD	PTR	[MNE_FIELD+2],'ED'
	JNZ	@@NotIFDEF
	CMP	BYTE	PTR	[MNE_FIELD+4],'F'
	JNZ	@@NotIFDEF
	jmp	@@PushIf
@@NotIFDEF:
							; check for ifnot
	CMP	WORD	PTR	[MNE_FIELD+2],'ON'
	JNZ	@@SkipLine
	CMP	BYTE	PTR	[MNE_FIELD+4],'T'
	JNZ	@@SkipLine
@@PushIf:
	XOR	AL,AL
	JMP	PUSH_IF
@@NotIf:			; not if'ed out
	MOV	AL,[LINE]
	CMP	AL,20H		;CHECK SYMBOL FIELD FOR SYMBOL
	JZ	@@NotLabel
	OR	AL,AL
	JZ	@@NotLabel
					; create label entry
	CMP	AL,LLCHR
	JNZ	@@NotLocal
	MOV	[LINE],LOCAL_SIG
@@NotLocal:
	MOV	AX,OBJPTRL
	MOV	SVL,AX
	MOV	AX,OBJPTRH
	MOV	SVH,AX
;	MOV	BYTE PTR SVH+1,0	; kts objptrh should be a word
;	MOV	BYTE PTR SVH,AL
	CALL	ADD_SYMBOL

@@NotLabel:
	TEST	SDB1,2		;IF LINE IS "MT"(empty)
	JNZ	@@Ret		;THEN BYPASS

	cmp	CSTYPE,SEGTYPE_DATA
	JNZ	@@DoCode
	call	PARSE_PSEUDO
	jmp	short @@NotCode

@@DoCode:
	CALL	PARSE_MNE	;PARSE THE LINE AND ASSEMBLE IT(call processor specific portion)
@@NotCode:

	TEST	SDB2,10H	;IF ANY BYTES ASSEMBLED
	JZ	@@Ret

	CMP	_MAPFLAG,0	;THEN DO MAP THING
	JZ	@@NoKludge
	CALL	SEGSIZE_KLUDGE
@@NoKludge:

@@SkipLine:
	CMP	_SLDFLAG,0	;AND/OR SLD THING
	JZ	@@NoSLD
	CMP	CSTYPE,SEGTYPE_ROM	;BUT NOT IF RAM SEG
	JNZ	@@NoSLD
	TEST	SDB2,20H	;DO TYPE 1 IF FIRST BYTE IN NEW FILE
	JZ	@@NotNewFile
	CALL	SLDNewFile			; write out new file name
@@NotNewFile:
	JMP	SLDWriteLine		; write out sld entry

@@NoSld:
@@Ret:
	RET

ASSEMBLE_LINE	ENDP

;==============================================================================
