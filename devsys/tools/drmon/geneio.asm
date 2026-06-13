;==============================================================================
; snesio.asm:
;==============================================================================
; functions called from C must preserve:
;	BP,SP,CS,DS,SS
; if register vars is on(it is)
;	SI,DI


MODEL LARGE
.code

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

Public C SwapLong
SwapLong    PROC FAR		; 1234 becomes 4321
    Cprefix
     mov dx,[bp]
     mov ax,[bp+2]
     xchg    ah,al
     xchg    dh,dl
    Cpostfix
SwapLong    ENDP


;----------------------------------------------------------------------------

	END

