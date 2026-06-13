;==============================================================================
; fileio.asm: dos interface for spasm
;==============================================================================

DATA	SEGMENT	'DATA'


;SRCBUFFER_SIZE	EQU	01000H			; 4k

SLDBUF		DW	0
SLDPTR		DW	0

SRC_HANDLE	DW	INCLUDEMAX DUP(0)
SRC_PTR		DW	INCLUDEMAX DUP(0)
SRC_ENDPTR	DW	INCLUDEMAX DUP(0)
SRC_LEVEL		DW	0FFFFH

FILESPEC	DB	FILESPEC_LEN*(INCLUDEMAX+1) DUP ('$')		; one per include level, and one for root level

INCHAND		DW	0

; kts in progress, plan to make incdir different for each include level
INCDIR		DB	DIRSPEC_LEN*(INCLUDEMAX+1) DUP (0)		; one per include level, and one for root level
IDL		DW	0

LAST_IF_LEVEL	DB	0,0,0,0

MAP_HEADING	DB	'  Segment Name    Type   Start     End    Length    Free  ',13,10
             	DB	'----------------  ----  -------  -------  -------  -------',13,10
MAP_HEADING_LEN	EQU	$ - MAP_HEADING
MAP_DATA	DB	'123456789ABCDEF   RxM   $XXXXXX  $XXXXXX  $XXXXXX  $XXXXXX',13,10


DATA	ENDS

;==============================================================================

.code


OPEN_SOURCE_FILE	PROC	NEAR

	CMP	_SLDFLAG,0		;IF SLDFLAG ON THEN INIT SLD_FILE
	JZ	@@NoSldFile

				; open sld file
	MOV	AH,3CH
	xor	cx,cx
	MOV	DX,OFFSET _SLD_FILE
	INT	21H
	JNC	@@SLDOk
	JMP	SEVERE
@@SLDOk:
	MOV	SLD_HAND,AX
	XOR     SI,SI
@@HeaderLoop:
	MOV	AL,[SLD_HEADING+SI]
	INC	SI
	CALL	SLDWriteByte
	CMP	AL,26
	JNZ	@@HeaderLoop
	MOV	SLDPTR,100H

@@NoSldFile:
	XOR	BX,BX		;MOVE SRC_FILE INTO PARAM_FIELD
@@CopyLoop:
	MOV	AL,[_SRC_FILE+BX]
	MOV	[PARAM_FIELD+BX],AL
	INC	BX
	CMP	BX,MAX_PARAM_FIELD_LEN
	JNB	@@ErrorTooLarge
	OR	AL,AL
	JNZ	@@CopyLoop
	JMP	PUSH_FILE

@@ErrorTooLarge:
	MOV	DX,OFFSET ERROR44		;FILESPEC TOO LARGE
	MOV	AH,9
	INT	21H
	mov	_ERRORLEVEL,DOS_RETURNCODE_SEVERE
	JMP	T1

;=============================================================================

PUSH_FILE:
	INC	SRC_LEVEL
	CMP	SRC_LEVEL,INCLUDEMAX
	JBE	@@NotTooDeep
	MOV	SRC_LEVEL,INCLUDEMAX
	MOV	AX,17
	JMP	ERROR

@@NotTooDeep:
	MOV	AX,INT21_OPENFILE	;OPEN [PARAM_FIELD] FOR INPUT
	MOV	DX,OFFSET PARAM_FIELD
	CMP	IDL,0
	JZ	@@NoIncDir
	dec	SRC_LEVEL
	CALL	INC_DIR			; include in context of parent file
	inc	SRC_LEVEL
@@NoIncDir:
	mov	cx,1			; open read-only, kts 12-24-92 06:13am
	INT	21H
	JNC	@@Ok

	DEC	SRC_LEVEL		; put back to parent file level
	mov	ax,12		; FILE NOT FOUND
	call	Error

	CMP	SRC_LEVEL,0ffffh		; if -1, then there is no parent
	JNZ	@@IncludeNotFound

	mov	_ERRORLEVEL,DOS_RETURNCODE_SEVERE
	JMP	T1

@@IncludeNotFound:
	ret

@@Ok:
	MOV	BX,SRC_LEVEL			;FILE FOUND
	MOV	DL,IF_LEVEL
	MOV	[LAST_IF_LEVEL+BX],DL
	SHL	BX,1
	MOV	[SRC_HANDLE+BX],AX
	MOV	[SRC_PTR+BX],1000H
	MOV	[SRC_ENDPTR+BX],1002H
	MOV	[LINE_COUNT+BX],0

	XOR	SI,SI
	MOV	BX,SRC_LEVEL

	ERRIF	FILESPEC_LEN NE 80H "FILESPEC_LEN has changed, code must be updated"
	MOV	CL,7
	SHL	BX,CL
@@Loop:
	MOV	AL,[PARAM_FIELD+SI]
	MOV	[FILESPEC+BX+SI],AL
	INC	SI
	OR	AL,AL
	JNZ	@@Loop

	MOV	[FILESPEC+BX+SI-1],'$'    ;DELINEATE WITH '$'

	CMP	_SLDFLAG,0
	JZ	@@NoSld
	ifdef	_SPOOL_BACKPATCH
	OR	SDB2,20H		  ;ACTIVATE SLD FILE THING
	else
	JMP	SLDNewFile
	endif
@@NoSld:
	RET

;==============================================================================

POP_FILE:			;CLOSE A FILE
@@IfLoop:
	MOV	BX,SRC_LEVEL
	MOV	AL,[LAST_IF_LEVEL+BX]
	CMP	AL,IF_LEVEL
	JZ	@@IfLevelOk
	MOV	AX,54
	XOR	BH,BH
	MOV	BL,IF_LEVEL
	dec	bx
	add	bx,bx
	mov	dx,[IF_STARTINGLINE+BX]
	CALL	ERROR
	and	SDB2,not 1			; clear error flag so each error gets printed
	dec	IF_LEVEL
;	MOV	IF_LEVEL,AL
	jmp	short @@IfLoop
@@IfLevelOk:
	CALL	FLUSH_LOCAL
	MOV	AH,3EH
	MOV	BX,SRC_LEVEL
	SHL	BX,1
	MOV	BX,[SRC_HANDLE+BX]
	INT	21H
	MOV	LINE,0
	MOV	BX,SRC_LEVEL
	SUB	BX,1		;CF SET IF LAST FILE
	MOV	SRC_LEVEL,BX
	JC	@@Ret
	CMP	_SLDFLAG,0
	JZ	@@NoWriteSld
	CALL	SLDNewFile
@@NoWriteSld:
	CLC
@@Ret:
	RET
OPEN_SOURCE_FILE	ENDP

;==============================================================================

WRITE_MAP	PROC	NEAR

	MOV	AH,3CH		;OPEN FILE
	XOR	Cx,Cx			; kts 12-25-92 05:32am
	MOV	DX,OFFSET _MAP_FILE
	INT	21H
	JNC	@@Ok
	JMP	SEVERE
@@Ok:
	MOV	INCHAND,AX

	MOV	BX,CSPTR	;ANY LABELS?
	OR	BX,BX
	JZ	WM5

	MOV	AH,40H		;WRITE HEADING
	MOV	BX,INCHAND
	MOV	CX,MAP_HEADING_LEN	;102
	MOV	DX,OFFSET MAP_HEADING
	INT	21H
	JNC	WM2
	JMP	SEVERE

WM2:
	XOR	BP,BP

WM3:
	CMP	BP,SYMTOTAL
	JNC	WM5
	MOV	AX,BP
	INC	BP
	SHL	AX,1
	ADD	AX,SYMBASE
	MOV	ES,AX
	XOR	DI,DI
WM4:
	MOV	AL,ES:[DI]
	MOV	[MAP_DATA+DI],AL
	INC	DI
	CMP	DI,0FH
	JNC	WM3
	CMP	AL,SEGTYPE_COUNT
	JAE	WM4
	CMP	AL,SEGTYPE_ROM
	JZ	WriteSegmentTypeRom
	CMP	AL,SEGTYPE_RAM
	JZ	WriteSegmentTypeRam
	cmp	al,SEGTYPE_DATA
	je	WriteSegmentTypeData
	JMP	WM3

WM5:
	MOV	AH,3EH		;CLOSE FILE
	MOV	BX,INCHAND
	INT	21H
	RET

WriteSegmentTypeData:
	mov	word ptr [map_data+18],'AD'
	mov	word ptr [map_data+20],'AT'
	jmp	WM8

WriteSegmentTypeRam:
	mov	word ptr [map_data+18],'AR'
	mov	word ptr [map_data+20],'M'
	JMP	WM8

WriteSegmentTypeRom:
	mov	word ptr [map_data+18],'OR'
	mov	word ptr [map_data+20],'M'

WM8:
	MOV	BYTE PTR [MAP_DATA+DI-1],20H
	INC	DI
	CMP	DI,10H
	JC	WM8

	MOV	AL,ES:[symbol_segment_bottom_h]
	CALL	HEXOUT
	MOV	[MAP_DATA+25],AH
	MOV	[MAP_DATA+26],AL

	MOV	AL,BYTE PTR ES:[symbol_segment_bottom_l+1]
	CALL	HEXOUT
	MOV	[MAP_DATA+27],AH
	MOV	[MAP_DATA+28],AL

	MOV	AL,BYTE PTR ES:[symbol_segment_bottom_l]
	CALL	HEXOUT
	MOV	[MAP_DATA+29],AH
	MOV	[MAP_DATA+30],AL

	MOV	AL,ES:[symbol_segment_top_h]
	CALL	HEXOUT
	MOV	[MAP_DATA+34],AH
	MOV	[MAP_DATA+35],AL

	MOV	AL,BYTE PTR ES:[symbol_segment_top_l+1]
	CALL	HEXOUT
	MOV	[MAP_DATA+36],AH
	MOV	[MAP_DATA+37],AL

	MOV	AL,BYTE PTR ES:[symbol_segment_top_l]
	CALL	HEXOUT
	MOV	[MAP_DATA+38],AH
	MOV	[MAP_DATA+39],AL

	MOV	CX,ES:[symbol_segment_top_l]
	MOV	BL,ES:[symbol_segment_top_h]
	SUB	CX,ES:[symbol_segment_bottom_l]
	SBB	BL,ES:[symbol_segment_bottom_h]
	ADD	CX,1
	ADC	BX,0

	MOV	AL,BL
	CALL	HEXOUT
	MOV	[MAP_DATA+43],AH
	MOV	[MAP_DATA+44],AL

	MOV	AL,CH
	CALL	HEXOUT
	MOV	[MAP_DATA+45],AH
	MOV	[MAP_DATA+46],AL

	MOV	AL,CL
	CALL	HEXOUT
	MOV	[MAP_DATA+47],AH
	MOV	[MAP_DATA+48],AL


	MOV	AL,ES:[symbol_segment_count_h]
	CALL	HEXOUT
	MOV	[MAP_DATA+52],AH
	MOV	[MAP_DATA+53],AL

	MOV	AL,BYTE PTR ES:[symbol_segment_count_l+1]
	CALL	HEXOUT
	MOV	[MAP_DATA+54],AH
	MOV	[MAP_DATA+55],AL

	MOV	AL,BYTE PTR ES:[symbol_segment_count_l]
	CALL	HEXOUT
	MOV	[MAP_DATA+56],AH
	MOV	[MAP_DATA+57],AL


	MOV	AH,40H
	MOV	BX,INCHAND
	MOV	CX,51+9
	MOV	DX,OFFSET MAP_DATA
	INT	21H
	JNC	WM9
	JMP	SEVERE
WM9:
	JMP	WM3

WRITE_MAP	ENDP

;==============================================================================

WRITE_SLD	PROC	NEAR

SLDNewFile: 				 ;FILE NAME CHANGE
	AND	SDB2,NOT 20H
	MOV	AL,1
	CALL	SLDWriteByte

	CMP	SRC_LEVEL,0
	JZ	@@W10
	MOV	CX,IDL
	JCXZ	@@W10
	XOR	SI,SI
@@W9:
	MOV	AL,[INCDIR+SI]
	INC	SI
	CALL	SLDWriteByte
	LOOP	@@W9

@@W10:
	MOV	SI,SRC_LEVEL
	ERRIF	FILESPEC_LEN NE 80H "FILESPEC_LEN has changed, code must be updated"
	MOV	CL,7
	SHL	SI,CL
@@W11:
	MOV	AL,[FILESPEC+SI]
	INC	SI
	CMP	AL,'$'
	JZ	@@W12
	CALL	SLDWriteByte
	JMP	@@W11
	RET
@@W12:
	MOV	AL,0FFH
	CALL	SLDWriteByte
	MOV	AL,0FFH
	JMP	SLDWriteByte

SLDWriteLine:				 ;LINE ADDRESS DEFINITION
	MOV	AL,2
	CALL	SLDWriteByte

	MOV	AL,BYTE PTR [OBJPTRL_BFA]
	CALL	SLDWriteByte
	MOV	AL,BYTE PTR [OBJPTRL_BFA+1]
	CALL	SLDWriteByte
	MOV	AL,BYTE PTR [OBJPTRH_BFA]
	CALL	SLDWriteByte
	MOV	SI,SRC_LEVEL
	SHL	SI,1
	MOV	AL,BYTE PTR [LINE_COUNT+SI]
	CALL	SLDWriteByte
	MOV	AL,BYTE PTR [LINE_COUNT+SI+1]
	JMP	SLDWriteByte

;-----------------------------------------------------------------------------

SLDWriteByte:					; write a byte to sld buffer, flush too file when full
	MOV	ES,SLDBUF
	MOV	BX,SLDPTR
	MOV	ES:[BX],AL
	INC	BX
	MOV	SLDPTR,BX
	CMP	BX,SLD_BUFFER_SIZE
	JZ	SLDWriteBuffer
	RET

;-----------------------------------------------------------------------------

SLDWriteBuffer:				; flush buffer to disk
	MOV	AH,40H
	MOV	BX,SLD_HAND
	MOV	CX,SLDPTR
	MOV	BP,DS
	MOV	DS,SLDBUF
	XOR	DX,DX
	INT	21H
	MOV	DS,BP
	JNC	@@WSLD5
	JMP	SEVERE

;-----------------------------------------------------------------------------

@@WSLD5:
	MOV	SLDPTR,0
	RET
WRITE_SLD	ENDP

;==============================================================================
;
; Inputs:
;	es = segment to read into (fix later...)
;	si = offset into src_handle
; Outputs:
;	cx = size of buffer asked for
;	ax = size of buffer read
;
;------------------------------------------------------------------------------

ReadSourceChunk	PROC	NEAR

;	mov	si,src_level         	; look up correct handle
;	shl	si,1
	MOV	BX,[SRC_HANDLE+SI]
	MOV	CX,SOURCE_BUFFER_SIZE   ;1000H                ; 4K is buffer size
	PUSH	DS
	PUSH	ES
	POP	DS
	XOR	DX,DX
	call	FileRead
	POP	DS

	OR	AX,AX		;GET ANYTHING?
	JZ	@@EndOfFile
	cmp	ax,cx
	jz	GNB2
	MOV	[SRC_ENDPTR+SI],AX
GNB2:
	xor	ax,ax			; used later in following calls!
	MOV	WORD PTR [SRC_PTR+SI],ax
	ret

@@EndOfFile:
	;;mov	ax,43
	;;call	error
	jmp	pop_file

ReadSourceChunk	ENDP

;==============================================================================
; Inputs:
;	ds:dx -> buffer to read into
;	bx = file handle
;	cx = # of bytes to read
; Outputs:
;	ax = # of bytes actualy transfered
; Destroys:
;	ax,bx,cx
;------------------------------------------------------------------------------

; !!! potential bug -- DS was pushed before calling this routine.
; Therefore, JMP SEVERE is a leak

FileRead	PROC	NEAR
	MOV	AH,3FH		;READ FILE INTO BUFSEG2
	INT	21H
	JNC	@@Ok
	JMP	SEVERE
@@Ok:
	RET
FileRead	ENDP

;==============================================================================

WBUF2	PROC	NEAR		;WRITE BX BYTES IN BUFSEG2

	MOV	AH,40H
	MOV	CX,BX
	MOV	BX,INCHAND
	XOR	DX,DX
	PUSH	DS
	MOV	DS,BUFSEG2
	INT	21H
	POP	DS
	JNC	WBUF2_1
	JMP	SEVERE

WBUF2_1:
	XOR	BX,BX
	RET

WBUF2	ENDP

;==============================================================================
; copy current INCDIR into PARAM_FIELD

INC_DIR	PROC	NEAR
	XOR	SI,SI
	MOV	DI,IDL
@@Loop:
	MOV	AL,[PARAM_FIELD+SI]
	MOV	[INCDIR+DI],AL
	INC	SI
	INC	DI
	OR	AL,AL
	JNZ	@@Loop
	MOV	DX,OFFSET INCDIR
	RET
INC_DIR	ENDP

;==============================================================================

