	list on
first:



test    macro
	nop
	endm


	nop
	cnop    2,15

	jmp     00

;fred   equ     5+test


	test
       jsr     test

;       jmp     first
;       jsr     second
;
;
;second:
	end


