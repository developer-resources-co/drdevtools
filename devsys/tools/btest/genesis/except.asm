;==============================================================================
; expect.ASM: Developer Resources Debugger Exception vectors
;==============================================================================
;
; EXCEPTION VECTORS
; You may use any vector you want, except NMI, Trace and Illegal
; (these are used by the debugger)

; user defined variables

;STACK		equ	$ffff00

;Stack   address stack should begin at
;Start   address of code to begin execution at



;		LABEL				 ADDRESS/ NUMBER

EXTABLE
	DC.L	Stack
	dc.l	$200
	dc.l	Buserr,Adderr	; 00-0F / 00 » 03
;	DC.L	Stack,Start,Buserr,Adderr	; 00-0F / 00 » 03
	DC.L	Illegal,Diverr,Blank,Blank	; 10-1F / 04 » 07
	DC.L	Blank,Trace,LineA,LineF 	; 20-2F / 08 » 11
	DC.L	Blank,Blank,Blank,Blank 	; 30-3F / 12 » 15
	DC.L	Blank,Blank,Blank,Blank 	; 40-4F / 16 » 19
	DC.L	Blank,Blank,Blank,Blank 	; 50-5F / 20 » 23
	DC.L	Blank,IRQ01,IRQ02,IRQ03 	; 60-6F / 24 » 27
	DC.L	IRQ04,IRQ05,IRQ06,NMI		; 70-7F / 28 » 31
	DC.L	Blank,Blank,Blank,Blank 	; 80-8F / 32 » 35
	DC.L	Blank,Blank,Blank,Blank 	; 90-9F / 36 » 39
	DC.L	Blank,Blank,Blank,Blank 	; A0-AF / 40 » 43
	DC.L	Blank,Blank,Blank,Blank 	; B0-BF / 44 » 47
	DC.L	Blank,Blank,Blank,Blank 	; C0-CF / 48 » 51
	DC.L	Blank,Blank,Blank,Blank 	; D0-DF / 52 » 55
	DC.L	Blank,Blank,Blank,Blank 	; E0-EF / 60 » 63
	DC.L	Blank,Blank,Blank  ;,Blank 	; F0-FF / 64 » 67


Buserr
Adderr
Illegal
Diverr
Trace
LineA
LineF
Blank
IRQ01
IRQ02
IRQ03
IRQ04
IRQ05
IRQ06
;NMI
	rte
	dc.w	0

;==============================================================================

	END

;============================================================================
