;==============================================================================
; spasm.inc: global equates for spasm
;==============================================================================

FILENAMELEN	EQU	64
MAX_PARAM_FIELD_LEN	EQU	255

LOCAL_SIG	EQU	''			; char used to indicate a symbol is local

;=============================================================================
; special file handles

FILE_HANDLES ENUM {
	STDIN	=	0
	STDOUT
	STDERR
	STDAUX
	STDPRN
}
;=============================================================================

INT21_OPENFILE	EQU	03D00h

MAX_LINE_LENGTH	EQU	250

MNE_FIELD_LEN	EQU	21

FILESPEC_LEN	EQU	80h
DIRSPEC_LEN	EQU	100h

;=============================================================================
; macro equates

MACRO_NAME_LEN	EQU	26	;21

MACRO_MAX_PARAMS	EQU	16
MACRO_PARAM_LEN	EQU	64
	ERRIF	MACRO_PARAM_LEN*MACRO_MAX_PARAMS NE 1024 "HARD CODED SHIFT NEEDS UPDATING"


	ERRIF	MACRO_PARAM_LEN NE 64 "HARD CODED SHIFT NEEDS UPDATING"
MACRO_PARAMLEN_SHIFTVAL	EQU	6

	ERRIF	MACRO_MAX_PARAMS NE 16 "HARD CODED SHIFT NEEDS UPDATING"
MACRO_PARAMNUM_SHIFTVAL	EQU	4

MACRO_PARAM_SHIFTVAL	EQU	MACRO_PARAMLEN_SHIFTVAL+MACRO_PARAMNUM_SHIFTVAL

MACRO_NEST_MAX	EQU	4	;4 LEVELS OF MACROS (SEE ALSO ERROR27!)

MACRO_SIG	EQU	0ffffh

;=============================================================================

INCLUDEMAX	EQU	4	; kts 12-30-92 06:22am, this may not work if changed

;=============================================================================

;PIGSKIN			; define if you want [] to work as ()

; buffer sizes, see cint for details, this may not work if changed

HOLE_BUFFER_SIZE	EQU	0F000H
SLD_BUFFER_SIZE		EQU	04000H
LOCAL_FWDREF_BUFFER_SIZE	EQU	02000H
MACRO_BUFFER_SIZE	EQU	03800H
SOURCE_BUFFER_SIZE	EQU	01000H
FILE_BUFFER_SIZE	EQU	04000H
SYMPTR_BUFFER_SIZE	EQU	06000H

;==============================================================================
;==============================================================================
;==============================================================================
