MODEL SMALL
.CODE
LOCALS

	ASSUME	DS:_DATA

GEN_GO equ  7



gendata     DW  300H
;gencsb      DW  301H


STATB_BYTEREADY        equ        0
STATF_BYTEREADY        equ        1

STATB_BYTERECD        equ        1
STATF_BYTERECD        equ        2

STATB_BUSY        equ        5
STATF_BUSY        equ        20h

STATB_ASYNC        equ        6
STATF_ASYNC        equ        40h

STATB_READY        equ        7
STATF_READY        equ        80h


COMMAND_GETV    EQU 1
VACK    EQU 2
VRDY    EQU 1

SlaveDead   dw  0

Public C CheckSlaveAlive
CheckSlaveAlive PROC NEAR
    Cprefix
    mov ax,cs:SlaveDead
    Cpostfix
CheckSlaveAlive ENDP

genstatus   dw  0


	IFDEF        GENESIS

	ifdef	GREENBOARD

OLDGETATTENTION PROC NEAR
;    Cprefix
	push        ds
	mov        dx,0301h                        ; first check if already have attention
	xor        al,al
	out        dx,al
	in        al,dx
	and        al,080h
	jnz        @@ready                        ; already have attention

    ; short delay, to see if the monitor will come back soon
    mov     cx,20h                        ; then just wait in case a previous command is in progress
@@preDelay:
    in      al,dx
    and     al,080h
    jnz     short @@ready
    loop    @@preDelay

    ; apparenlty it's not coming back soon - we'll need to NMI it

	mov        cx,0ffffh                ; if we have monitor running, but it is busy, wait here
@@BusyDelay:
	in	al,dx			; just for delay purposes
	in	al,dx			; check if busy
	and	al,STATF_BUSY
	jz	short @@NMI
	loop	@@BusyDelay

@@NMI:                                        ; wait for attention to come back
    in      al,dx
    and     al,080h
    jnz     @@ready


    inc     dx      ; NMI                ; ok, we have waited long enough, bang on it
    out     dx,al
    mov     cs:genstatus,1
    mov     ax,0
    mov     cx,0ffffh
    dec            dx                                                ; kts added
@@delay:
    in      al,dx
    and     al,080h
    jnz     short @@hello
    loop    @@delay
    mov     ax,1
    mov     cs:SlaveDead,1
;    Cpostfix
	pop        ds
	ret
@@ready:
    mov     cs:genstatus,0 ; it's ready
@@hello:
    mov     cs:SlaveDead,0
    mov     ax,0
;    Cpostfix
	pop        ds
	ret
OLDGETATTENTION ENDP


;============================================================================

OLDSENDCMD    PROC    NEAR
	MOV        DX,cs:gendata        ; get port address

	OUT        DX,AL           ; WRITE BYTE TO PORT
	INC        DX              ; POINT TO CSB

	in        al,dx                ; read command port
	and        al,STATF_READY        ; check if ready to receive command
	jnz        @@Ok                ; yes, genesis is waiting for a command

    ; short delay, to see if the monitor will come back soon
    mov     cx,500h                        ; then just wait in case a previous command is in progress
@@Loop:
	in        al,dx
	and        al,STATF_READY
	jnz        short @@Ok
	loop        @@Loop
	jmp        @@TimeOut
@@Ok:
	MOV        AL,STATF_BYTEREADY                ; byte ready to transfer
	OUT	DX,AL
	xor	cx,cx
@@1:
	IN         AL,DX           ; READ CSB
	dec	cx
	jz	@@TimeOut
	AND	AL,2            ; CHECK ack
	JZ	@@1              ; STILL WAITING

	XOR	AL,AL           ; CLEAR CSB
	OUT	DX,AL
	xor	cx,cx
@@2:
	IN	AL,DX           ; READ CSB
	dec	cx
	jz         @@TimeOut
	AND        AL,2            ; WAIT FOR ack TO FALL
	JNZ        @@2
	RET
@@TimeOut:
	mov     cs:SlaveDead,1
	ret
OLDSENDCMD    ENDP


SENDLONG    PROC    NEAR
    push    cx
    MOV     AL,BH
    CALL    SENDBYTE
    MOV     AL,BL
    CALL    SENDBYTE
    pop     cx
    push    cx
    MOV     AL,CH
    CALL    SENDBYTE
    pop     cx
    MOV     AL,CL
    CALL    SENDBYTE
    RET
SENDLONG    ENDP


;
; FASTSEND -- USE 2PHASE TRANSFER.  DATA AT DS:SI
; COUNT IN BP:CX
; DESTROYS AX, BX, DX
;
FASTSEND    PROC    NEAR
    inc bp
    MOV DX,cs:gendata              ; SET UP
    MOV BL,VACK                 ; TWOGGLE BITS
    MOV BH,VRDY
FAST2:
    LODSB                 ; GET BYTE TO WRITE
    OUT DX,AL                   ; PUT IT IN THE DATA REG
    INC DX          ; POINT TO CSB
    MOV AL,BH
    OUT DX,AL           ; SAY I'VE GOT IT
    XOR BH,VRDY         ; AND CHANGE TO NEXT STATE
    xor     di,di
FAST1:
    IN  AL,DX                   ; LOOK FOR ACK
    XOR     AL,BL                   ; DIDIT CHANGE?
    dec     di
    jz      @@timeout
    AND AL,VACK         ; IGNORE OTHER BITS
    JNE FAST1
    XOR BL,VACK         ; CHANGE STATE TO LOOK FOR
    DEC DX
    LOOP    FAST2
    dec     bp
    jnz     FAST2
    inc                dx
    xor                al,al
    out                dx,al
    RET
@@timeout:
    mov     cs:SlaveDead,1
    ret
FASTSEND    ENDP


SENDBYTE    PROC    NEAR
	MOV        DX,cs:gendata
	OUT        DX,AL           ; WRITE BYTE TO PORT
	INC        DX              ; POINT TO CSB

	in        al,dx                ; read command register
	and        al,STATF_READY
	jnz        @@TimeOut

	MOV        AL,STATF_BYTEREADY
	OUT        DX,AL
	xor        cx,cx
@@1:
	IN        AL,DX           ; READ CSB
	dec        cx
	jz        @@TimeOut
	AND        AL,2            ; CHECK ack
	JZ        @@1              ; STILL WAITING

	XOR        AL,AL           ; CLEAR CSB
	OUT        DX,AL
	xor        cx,cx
@@2:
	IN        AL,DX           ; READ CSB
	dec        cx
	jz	@@TimeOut
	AND	AL,2            ; WAIT FOR ack TO FALL
	JNZ	@@2
	RET
@@TimeOut:
	mov     cs:SlaveDead,1
	ret
SENDBYTE    ENDP

	ENDIF

	ENDIF

;==============================================================================
; new board io

	IFDEF	SPC700
NMI	proc	FAR
NMI	endp
	ENDIF

	IFDEF        SNES
DB_CTRL_NMI                equ        040h        ; 0 = NMI asserted, 1 = normal
DB_CTRL_RESET                 equ        080h        ; 0 = reset asserted, 1 = normal

; assumes ds is set correctly
NMI        proc        FAR
	push        dx
	mov        dx,slaveCtrl

	mov        al,020h
	out        dx,al

	mov        al,060h
	out        dx,al

	pop        dx
	ret
NMI        endp
	ENDIF

	IFDEF        GENESIS
NMI        proc        near

	push        dx
	mov        dx,slaveCtrl

	mov        ax,0a0h
	out        dx,al

	mov        ax,0e0h
	out        dx,al

	pop        dx
	ret
NMI        endp
	ENDIF


GetAttention PROC NEAR
	push        ds
	mov        ax,seg slaveWorm
	mov        ds,ax
	mov     dx,slaveWorm                ; first check if already have attention
	in      al,dx
	and     al,STATF_READY
	jnz     @@ready                        ; already have attention

	; short delay, to see if the monitor will come back soon
	mov     cx,20h                        ; then just wait in case a previous command is in progress
	@@preDelay:
	in      al,dx
	and     al,STATF_READY
	jnz     short @@ready
	loop    @@preDelay

	; apparenlty it's not coming back soon - we'll need to NMI it

	mov        cx,0ffffh                ; if we have monitor running, but it is busy, wait here
	@@BusyDelay:
	in        al,dx                        ; just for delay purposes
	in        al,dx                        ; check if busy
	and        al,STATF_BUSY
	jz        short @@NMI
	loop        @@BusyDelay

	@@NMI:                                        ; wait for attention to come back
	in      al,dx
	and     al,STATF_READY
	jnz     @@ready

	call        NMI

	;    inc     dx      ; NMI                ; ok, we have waited long enough, bang on it
	;    out     dx,al
	mov     genStatus,1
	mov     ax,0
	mov     cx,0ffffh
	;    dec            dx                                                ; kts added
	@@delay:
	in      al,dx
	and     al,STATF_READY
	jnz     short @@hello
	loop    @@delay
	mov     ax,1
	mov     cs:slaveDead,1
	pop        ds
	ret
	@@ready:
	mov     genStatus,0 ; it's ready
	@@hello:
	mov     cs:slaveDead,0
	mov     ax,0
	pop        ds
	ret
GetAttention ENDP

;==============================================================================

GetSlaveBus    PROC NEAR
	push        ds
	mov        ax,seg slaveWorm
	mov        ds,ax
	mov        dx,slaveWorm

	mov        cx,0ffffh                ; if we have monitor running, but it is busy, wait here
@@RdyDelay:
	in        al,dx                        ; check if busy
	and        al,STATF_READY
	jnz        short @@Ok
	loop        @@RdyDelay
@@Ok:
	mov        dx,slaveCtrl
	ifdef        GENESIS
	mov        al,0e3h
	endif
	ifdef        SNES
	mov        al,063h
	endif
	out        dx,al
	pop        ds
	ret
GetSlaveBus    ENDP

ReturnSlaveBus    PROC near
	Cprefix
	mov        ax,seg slaveCtrl
	assume        ds:seg slaveCtrl
	mov        ds,ax
	mov        dx,slaveCtrl
	ifdef        GENESIS
	mov        al,0e0h
	endif
	ifdef        SNES
	mov        al,060h
	endif
	out        dx,al
	Cpostfix
ReturnSlaveBus    ENDP

;==============================================================================

ResetPort PROC NEAR
	Cprefix
	push        ds
	mov        ax,seg slaveWorm
	assume        ds:seg slaveWorm
	mov        ds,ax
	mov        dx,slaveWorm
	xor        ax,ax
	out        dx,al                                        ; clear port
	pop        ds
	Cpostfix
ResetPort ENDP

SendCmd PROC NEAR
	Cprefix
	push        ds
	push        di
	assume        ds:seg slaveBufferCmd
	mov        ax,seg slaveBufferCmd
	mov        ds,ax
	call        GetSlaveBus
	mov        dx,slaveBank
	mov        al,080h
	out        dx,al                        ; point to com ram
	mov     ax,[bp]
	les        di,[slaveBufferCmd]
	stosb
	call        ReturnSlaveBus
	mov        dx,slaveWorm

	in        al,dx                ; read command register
	and        al,STATF_READY
	jz        @@TimeOut

	MOV        AL,STATF_BYTEREADY
	OUT        DX,AL
	xor        cx,cx
@@1:
	IN        AL,DX           ; READ CSB
	dec        cx
	jz        @@TimeOut
	AND        AL,2            ; CHECK ack
	JZ        @@1              ; STILL WAITING

	XOR        AL,AL           ; CLEAR CSB
	OUT        DX,AL
	xor        cx,cx
@@2:
	IN        AL,DX           ; READ CSB
	dec        cx
	jz        @@TimeOut
	AND        AL,2            ; WAIT FOR ack TO FALL
	JNZ        @@2
	pop	di
	pop	ds
	Cpostfix
@@TimeOut:
	mov     cs:slaveDead,1
	pop	di
	pop	ds
	Cpostfix
SendCmd ENDP

;==============================================================================
