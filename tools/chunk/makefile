.AUTODEPEND

chunk.exe: chunk.obj
	tlink /m /v t:\lib\c0s.obj chunk,chunk.exe,, t:\lib\cs.lib

.c.obj:
	 bcc -b- -w-par -wpro -It:\include -v -r -c $<

.asm.obj:
    tasm /zi /m2 /ml $<
