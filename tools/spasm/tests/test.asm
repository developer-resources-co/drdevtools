
	list    on

CODE    DEFS    ROM
CODE    SEGMENT

	org     $8000

	jmp     foo
	jmp     foo
	jmp     foo

	org     $18000

foo:


	END

