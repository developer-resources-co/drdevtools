;==============================================================================
; board.asm: board I/O code for spasm/65816
;==============================================================================

DATA	SEGMENT	'DATA'

;------------------------------------------------------------------------------

CMD_RESTOREVECT	EQU	01Ch

;------------------------------------------------------------------------------
					; kts added 2/4/92
IOBUFF		db	0			; buffer to write to disk
IOADDR		dw	0			; address of previous disk write
IOPAGE		db	0			; page of previous disk write
GBERR		DB	'Slave dead: assembly halted!$'

IOSIZEADDR	dw	0
IOSIZEPAGE	db	0
IOFILLBYTE	db	0ffh

outputBuffer	db	4097 dup (?)

;------------------------------------------------------------------------------
; bank address remap table

SBTable	label	BYTE
	db	0,8,1,9,2,0Ah,3,0Bh,4,0Ch,5,0Dh,6,0Eh,7,0Fh

DATA	ENDS

;==============================================================================

	extrn	C slaveBuffer:DWORD
	extrn	C slaveBufferCmd:DWORD
	extrn 	C slaveCtrl:WORD
	extrn 	C slaveBank:WORD
	extrn 	C slaveWorm:WORD

	include	gslib.asm
	assume	ds:data,es:nothing

;==============================================================================
.CODE

writeByteInitTable	dw	WriteByteInitDopple
		dw	WriteByteInitNone
		dw	WriteByteInitFile
		dw	WriteByteInitOld

;------------------------------------------------------------------------------

writeByteTable	dw	WriteByteDopple
		dw	WriteByteNone
		dw	WriteByteFile
		dw	WriteByteOld

writeBytePtr	dw	WriteByteNone

;==============================================================================
; init output
;------------------------------------------------------------------------------

WriteByteInitNone:				; do nothing
	ret

;------------------------------------------------------------------------------

WriteByteInitDopple:				; write to the dopple board
	call	ResetPort
	CALL	GETATTENTION    		; if 0, use devel system
	CMP	BYTE PTR CS:SlaveDead,1
	JNZ	@@Ok
	jmp	SB0ERR
@@Ok:
	call	GetAttention
	call	GetSlaveBus
	ret

;------------------------------------------------------------------------------

WriteByteInitFile:				; write to a disk file
	mov	ax,0ffffh
	mov	IOADDR,ax       ; make sure previous file address is invalid
	mov	IOPAGE,al

	MOV	AH,3CH		;OPEN FILE
	XOR	CX,CX
	MOV	DX,OFFSET _SYSTEM_FILE
	INT	21H
	JNC	@@Ok
	JMP	SEVERE
@@Ok:
	MOV	SYSTEMHAND,AX				; file is now open
	ret

;------------------------------------------------------------------------------

WriteByteInitOld:				; write to the old board(no longer supported)
	ret

;==============================================================================

InitOutput	proc
			; lookup correct byte write routine
	mov	bl,_SYSTEMTYPE
	xor	bh,bh
	add	bx,bx
	mov	ax,[writeByteTable+bx]
	mov	writeBytePtr,ax

			; now call correct init routine
	jmp	[writeByteInitTable+bx]
InitOutput	endp

;==============================================================================

SB0ERR:
	push	ax
	MOV	AH,9				; PRINT GREENBOARD I/O ERROR MESSAGE
	MOV	DX,OFFSET GBERR
	INT	21H
	pop	ax
	JMP	T1

;==============================================================================
; do all checking required for one source line
; 	calling order
;		LineCheck
;		WriteByte
;		.
;		WriteByte
;		EndLineCheck
; Inputs:
;
; Outputs:
;	dl:DI = starting address to write to
; Destroys:
;
;------------------------------------------------------------------------------

LineCheck	proc
					; first check if slave still alive
	CMP	BYTE PTR CS:SlaveDead,1
	JZ	SB0Err

	OR	SDB2,10H			; SET FLAG INDICATED A BYTE HAS BEEN ASSEMBLED
	  				; calc address to write to
	MOV	DI,OBJPTRL			;COMPUTE OBJPTR+DELTA
	ADD	DI,DELTA_L
	MOV	dx,OBJPTRH
	ADC	dx,DELTA_H
					; kts work on this for multiple bytes
	ifdef	SNES
					; IF "SFXMAP" = "ON" THEN CONVERT
	TEST	SDB2,80H
	JZ	@@NoSfxMap
	TEST	DI,8000H			;ILLEGAL ADDRESS?
	JNZ	@@AddrOk
	MOV	BP,33
	CALL	PrintError
	JMP	@@NoSfxMap
@@AddrOk:
	AND	DI,7FFFH
	SHR	dl,1				;DELETE THE BIT
	JNC	@@NoSfxMap
	OR	DI,8000H
@@NoSfxMap:
	endif
	ret
LineCheck	endp

;==============================================================================
; actually write out a byte
;------------------------------------------------------------------------------

WriteByteNone:				; do nothing
	add	di,1				; move to next address
	adc	dl,0
	ret

;------------------------------------------------------------------------------

WriteByteDopple:				; write to the dopple board
	; AL contains byte to write to file
	; dl:di -> addr to write to

	push	ax

	push	dx
	push	di

	ifdef	SNES
	; !!! only works up to 1MB (that's 8 Megabits for you marketing weenies)
	push	bx
	mov	bl,dl
	shr	bl,4
	rcl	di,1
	rcl	dl,1
	and	dl,0Fh
	pop	bx

;	shl	dl,1
;
;	; if di > $8000 { dl<<=1, ++dl, di &= $7FFF }
;	cmp	di,8000h
;	jb	@@NoOverflow
;
;	and	di,7FFFh
;	inc	dl
;@@NoOverflow:
;	shl	di,1
	endif

	push	si
	push	ax
	push	ds
	xor	dh,dh
	mov	si,dx
	mov	al,[SBtable+SI]
	mov	dx,slaveBank
	OUT	DX,AL		;SELECT BANK
	mov	ax,_DATA
	mov	ds,ax
	mov	es,WORD PTR [slaveBuffer+2] 	; read segment of board memory
	pop	ds
	pop	ax


	MOV	ES:[DI],AL
	pop	si

	pop	di
	pop	dx
	add	di,1				; move to next address
	adc	dl,0
	pop	ax
	ret

;------------------------------------------------------------------------------

WriteByteFile:				; write to a disk file
				; else write to disk file
	; AL contains byte to write to file
	; dl:di -> addr to write to

	push	dx
	push	di

	PUSH	BX
	PUSH	CX
	PUSH	SI

	mov	IOBUFF,al
	; check if we are in correct place in file

	cmp	di,IOADDR
	jnz	@@KtsNope
	cmp	dl,IOPAGE
	jnz	@@KtsNope
	jmp	@@KtsOk
@@KtsNope:
	; first seek to correct position in file

	TEST	assembleFlags,AFF_FILLFILE
	jz	@@NoFillFile

	cmp	dl,IOSIZEPAGE
	jnbe	@@NoFillFile			; if we are not at
	cmp	di,IOSIZEADDR
	jbe	@@NoFillFile

				; first, seek to end of file
	mov	ax,4200h			; fseek + mode 0
	mov	bx,SYSTEMHAND
	mov	cl,IOSIZEPAGE
	xor	ch,ch
	mov	dx,IOSIZEADDR
	int	21h
	jc	@@KtsErr
					; now loop filling until we arrive at the correct address
	mov	ax,di
	mov	bl,dl
	sub	ax,IOSIZEADDR
	sbb	bl,IOSIZEPAGE

	mov	IOSIZEADDR,di
	mov	IOSIZEPAGE,dl
	mov	IOADDR,di
	mov	IOPAGE,dl

	mov	cx,ax
@@FillLoop:
	push	cx
	push	bx
	mov	ah,40h
	mov	bx,SYSTEMHAND
	mov	cx,1
	mov	dx,offset IOFILLBYTE
	int	21h
	jc	@@KtsErr
	pop	bx
	pop	cx
	loop	@@FillLoop
	sub	bl,1
	jnc	@@FillLoop
	jmp	short @@KtsOk

@@NoFillFile:
	mov	IOADDR,di
	mov	IOPAGE,dl
	mov	ax,4200h			; fseek + mode 0
	mov	bx,SYSTEMHAND
	mov	cl,dl
	xor	ch,ch
	mov	dx,di
	int	21h
	jc	@@KtsErr

@@KtsOk:
	; now actually write the byte
	mov	ah,40h
	mov	bx,SYSTEMHAND
	mov	cx,1
	mov	dx,offset IOBUFF
	int	21h
	jnc	@@KtsNoErr
@@KtsErr:
	JMP	SEVERE
@@KtsNoErr:
	add	IOADDR,1
	adc	IOPAGE,0

; kts end of file checking
	mov	bl,IOPAGE
	cmp	bl,IOSIZEPAGE
	jc	@@NotLarger
	mov	ax,IOADDR
	cmp	ax,IOSIZEADDR
	jc	@@NotLarger
	mov	IOSIZEADDR,ax		; if larger, write it out
	mov	IOSIZEPAGE,bl
@@NotLarger:

	POP	SI
	POP	CX
	POP	BX

	pop	di
	pop	dx

	add	di,1				; move to next address
	adc	dl,0
	ret

;------------------------------------------------------------------------------

WriteByteOld:				; write to the old board(no longer supported)
	ret

;==============================================================================
; 	calling order
;		LineCheck
;		WriteByte
;		.
;		WriteByte
;		EndLineCheck
; Inputs:
;	al = byte to write
;------------------------------------------------------------------------------

WriteByte	proc
					; try to update listing file
	TEST	SDB2,8				;LIST ON?
	JZ	@@NoList
	push	di
	MOV	DI,LBPTR 			;IF SO THEN STORE THE BYTE
	CMP	DI,8
	JNL	@@NoList2
	MOV	[LIST_BYTES+DI],AL		; remember up to 8 bytes output on this line
	INC	LBPTR
@@NoList2:
	pop	di
@@NoList:
		; AL contains byte to write
		; dl:di -> addr to write to
	JMP	[writeBytePtr]
WriteByte	endp

;==============================================================================
; ax = # of bytes to skip

SkipBytes	proc	near
	ADD	[OBJPTRL],ax	;INC OBJPTR
	ADC	[OBJPTRH],0
				; kts genesis 6/19/92, snes 12-24-92 00:52am
	TEST	SDB1,11H	;ARE WE BACKPATCHING YET?
	JNZ	@@SizeOk		;forget the check if we are

	SUB	[CSCOUNTL],ax	;DEC CSCOUNT
	SBB	[CSCOUNTH],0
	JNC	@@SizeOk
	MOV	BP,35
	CALL	PrintError
@@SizeOk:
	ret
SkipBytes	endp

;==============================================================================
; 	calling order
;		LineCheck
;		WriteByte
;		.
;		WriteByte
;		EndLineCheck
; Inputs:
;	ax = # of bytes written
;------------------------------------------------------------------------------

EndLineCheck	proc
	call	SkipBytes
	CMP	CSTYPE,SEGTYPE_RAM	;IF RAM SEGMENT THEN BITCH
	JNZ	@@Ret
	MOV	BP,36
	CALL	PrintError
@@Ret:
	RET
EndLineCheck	endp

;==============================================================================
; Inputs:
;	es:si-> buffer containing bytes to write
;	cx = # of bytes to write ( 0 = 64K )
; Destroys:
;	dx,di,bp,es
;------------------------------------------------------------------------------

StoreBin	proc
	PUSH	DX                                   ;SAVE THESE REGISTERS!
	push	ax
	push	cx					; store byte count
	call	LineCheck

@@NextByte:
	mov	al,es:[si]
	push	es
	call	WriteByte
	pop	es
	inc	si
	loop	@@NextByte

	pop	ax					; get byte count back
	call	EndLineCheck
	pop	ax
	POP	DX
	ret
StoreBin        endp

;==============================================================================
; AX contains word to write
;------------------------------------------------------------------------------

STORE_WORD        PROC        NEAR
	PUSH	DX                                   ;SAVE THESE REGISTERS!
	push	ax
	call	LineCheck
	XCHG	AH,AL
	mov	bl,ah
	call	WriteByte
	mov	al,bl
	call	WriteByte
	mov	ax,2
	call	EndLineCheck
	pop	ax
	POP	DX
	ret
STORE_WORD        ENDP

;==============================================================================
; Inputs:
;        AL contains byte to write
;        ds-> segment DATA
; Destroys:
;        dx,di,bp,es
; Preserves:
;        ax,bx,cx,si,cs,ds
;------------------------------------------------------------------------------

STORE_BYTE        PROC        NEAR
	PUSH        DX                                   ;SAVE THESE REGISTERS!
	push        ax

	call	LineCheck
	call	WriteByte
	mov	ax,1
	call	EndLineCheck

	pop	ax
	POP	DX
	ret

;------------------------------------------------------------------------------

PrintError:
	PUSH	AX		;PUSH REGISTERS AND BITCH
	PUSH	BX
	PUSH	CX
	PUSH	DX
	MOV	AX,BP
	CALL	ERROR
	POP	DX
	POP	CX
	POP	BX
	POP	AX
	RET
STORE_BYTE	ENDP

;==============================================================================

PUBLIC	C	ReadMemFromC
ReadMemFromC	PROC	NEAR
	cprefix
	push	si
	push	di
	push	ds
	push	es

	mov	di,[bp]
	mov	dx,[bp+2]

	mov	ax,DATA
	mov	es,ax

	call	ReadMem
@@Nope:
	pop	es
	pop	ds
	pop	di
	pop	si
	cpostfix
ReadMemFromC	ENDP

;------------------------------------------------------------------------------

ReadMem:
	; dl:di -> addr to read from

	; read from dopple board
	push	ds
			; set correct bank #
	mov	ax,_DATA
	mov	ds,ax
	xor	dh,dh
	mov	si,dx
	mov	al,[SBtable+SI]
	mov	dx,slaveBank

	OUT	DX,AL		;SELECT BANK
	mov	es,WORD PTR [slaveBuffer+2] 	; just throw away offset
	MOV	ax,ES:[DI]
	MOV	dx,ES:[DI+2]
	pop	ds
	ret

;==============================================================================

SEGSIZE_KLUDGE    PROC    NEAR
	MOV	AX,OBJPTRL
	MOV	DX,OBJPTRH
	SUB	AX,1
	SBB	DX,0
	JC	@@SSK1
	CMP	DX,SSKTH
	JA      @@SSK3
	JNZ	@@SSK1
	CMP	AX,SSKTL
	JA	@@SSK3

@@SSK1:
	MOV	AX,OBJPTRL
	MOV	DX,OBJPTRH
	CMP	DX,SSKBH
	JC      @@SSK4
        JNZ     @@Ret
        CMP     AX,SSKBL
        JC      @@SSK4
@@Ret:
	RET

@@SSK3:
	MOV     SSKTH,DX
       	MOV     SSKTL,AX
        JMP     @@SSK1

@@SSK4:
	MOV     SSKBH,DX
       	MOV     SSKBL,AX
        RET
SEGSIZE_KLUDGE    ENDP

;==============================================================================
