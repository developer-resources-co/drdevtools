;==============================================================================
; genio.asm:
;==============================================================================
; functions called from C must preserve:
;	BP,SP,CS,DS,SS
; if register vars is on(it is)
;	SI,DI


MODEL LARGE

	extrn	C slaveBufferCmd:DWORD
	extrn 	C slaveCtrl:WORD
	extrn 	C slaveCtrlBits:BYTE
	extrn 	C slaveRunningCtrlBits:BYTE
	extrn 	C slaveBank:WORD
	extrn 	C slaveWorm:WORD
	extrn 	C slaveDead:WORD
	extrn 	C genStatus:WORD

;==============================================================================
; board equates

DB_CTRL_ENAB		equ	1	; 0 = mem off bus, 1 = mem on bus
DB_CTRL_WHO		equ	2	; 0 = target, 1 = host
DB_CTRL_INHIB		equ	4	; 0 = memory enables, 1 = memory disabled
DB_CTRL_EIGHTBIT	equ	8	; 0 = ?, 1 = ?
DB_CTRL_WP		equ	010h    ; 0 = target writes enabled, target writes inhibited
DB_CTRL_CLEAR		equ	020h	; 0 = clear wprot viol, 1 = normal
DB_CTRL_NMI		equ	040h	; 0 = NMI asserted, 1 = normal
DB_CTRL_RESET 		equ	080h	; 0 = reset asserted, 1 = normal

;==============================================================================

STATB_BYTEREADY	equ	0
STATF_BYTEREADY	equ	1

STATB_BYTERECD	equ	1
STATF_BYTERECD	equ	2

STATB_BUSY	equ	5
STATF_BUSY	equ	20h

STATB_ASYNC	equ	6
STATF_ASYNC	equ	40h

STATB_READY	equ	7
STATF_READY	equ	80h

;------------------------------------------------------------------------------

.CODE
LOCALS
GEN_GO equ  7

;------------------------------------------------------------------------------

Cprefix macro
	push    bp
	mov     bp,sp
	add     bp,6
endm

Cpostfix    macro
	pop bp
	ret
endm

;==============================================================================

Public C SwapWord
SwapWord    PROC FAR
    Cprefix
    mov     ax,[bp]
    xchg    ah,al
    Cpostfix
SwapWord    ENDP

Public C SwapLongWords
SwapLongWords    PROC FAR	; 1234 becomes 3412
    Cprefix
     mov dx,[bp]
     mov ax,[bp+2]
    Cpostfix
SwapLongWords    ENDP

Public C SwapLong
SwapLong    PROC FAR		; 1234 becomes 4321
    Cprefix
     mov dx,[bp]
     mov ax,[bp+2]
     xchg    ah,al
     xchg    dh,dl
    Cpostfix
SwapLong    ENDP

; assumes monitor is already running
Public C GetSlaveBus
GetSlaveBus    PROC FAR
	Cprefix
;;	mov	ax,seg slaveWorm
;;	assume	ds:seg slaveWorm
;;	mov	ds,ax
	mov	dx,slaveWorm

	mov	cx,0ffffh		; if we have monitor running, but it is busy, wait here
@@RdyDelay:
	in	al,dx			; check if busy
	and	al,STATF_READY
	jnz	short @@Ok
	loop	@@RdyDelay
	mov     slaveDead,1
	Cpostfix				; oh well, we just won't get the info
@@Ok:
	mov	dx,slaveCtrl
	mov	al,slaveCtrlBits
	or	al,DB_CTRL_WHO OR DB_CTRL_ENAB
;	mov	al,0e3h
	out	dx,al
	Cpostfix
GetSlaveBus    ENDP

Public C ReturnSlaveBus
ReturnSlaveBus    PROC FAR
	Cprefix
;;	mov	ax,seg slaveCtrl
;;	assume	ds:seg slaveCtrl
;;	mov	ds,ax
	mov	dx,slaveCtrl
	mov	al,slaveCtrlBits
;	mov	al,0e0h
	out	dx,al
	Cpostfix
ReturnSlaveBus    ENDP


; assumes ds is set correctly
NMI	proc	FAR
	push	dx
	mov	dx,slaveCtrl
	mov	al,slaveRunningCtrlBits
	and	al,NOT DB_CTRL_NMI
;	mov	ax,0a0h
	out	dx,al
	mov	al,slaveCtrlBits			; so that the monitor can save it variables
;	mov	ax,0e0h
	out	dx,al
	pop	dx
	ret
NMI	endp


Public C GetAttention
GetAttention PROC FAR
    Cprefix
;;	mov	ax,seg slaveWorm
;;	assume	ds:seg slaveWorm
;;	mov	ds,ax
	mov     dx,slaveWorm		; first check if already have attention
	in      al,dx
	and     al,STATF_READY
	jnz     @@ready			; already have attention

    ; short delay, to see if the monitor will come back soon
    mov     cx,20h			; then just wait in case a previous command is in progress
preDelay:
    in      al,dx
    and     al,STATF_READY
    jnz     short @@ready
    loop    preDelay

    ; apparenlty it's not coming back soon - we'll need to NMI it

	mov	cx,0ffffh		; if we have monitor running, but it is busy, wait here
BusyDelay:
	in	al,dx			; just for delay purposes
	in	al,dx			; check if busy
	and	al,STATF_BUSY
	jz	short @@NMI
	loop	BusyDelay

@@NMI:					; wait for attention to come back
    in      al,dx
    and     al,STATF_READY
    jnz     @@ready

	call	NMI

;    inc     dx      ; NMI		; ok, we have waited long enough, bang on it
;    out     dx,al
    mov     genStatus,1
    mov     ax,0
    mov     cx,0ffffh
;    dec	    dx						; kts added
delay:
    in      al,dx
    and     al,STATF_READY
    jnz     short @@hello
    loop    delay
    mov     ax,1
    mov     slaveDead,1
    Cpostfix
@@ready:
	mov	genStatus,0 ; it's ready
@@hello:
	mov	dx,slaveCtrl
	mov	al,slaveCtrlBits		; slave is now stopped, allow it to write to ram
	out	dx,al
    mov     slaveDead,0
    mov     ax,0
    Cpostfix
GetAttention ENDP

;============================================================================
; assumes already have attention

Public C SendCmd
SendCmd PROC FAR
	Cprefix
	push	di
;;	assume	ds:seg slaveBufferCmd
;;	mov	ax,seg slaveBufferCmd
;;	mov	ds,ax
	call	GetSlaveBus
	mov     ax,[bp]
	les	di,[slaveBufferCmd]
	stosb
	call	ReturnSlaveBus
	mov	dx,slaveWorm

	in	al,dx		; read command register
	and	al,STATF_READY
	jz	@@TimeOut
;	jnz	@@TimeOut

	MOV	AL,STATF_BYTEREADY
	OUT	DX,AL
	xor	cx,cx
@@1:
	IN	AL,DX           ; READ CSB
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
	jz	@@TimeOut
	AND	AL,2            ; WAIT FOR ack TO FALL
	JNZ	@@2
	pop	di
	Cpostfix
@@TimeOut:
	mov     slaveDead,1
	pop	di
	Cpostfix
SendCmd ENDP

;----------------------------------------------------------------------------

END

;----------------------------------------------------------------------------




