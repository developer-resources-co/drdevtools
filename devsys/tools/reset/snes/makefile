
.AUTODEPEND

sr.exe: sr.obj
	tlink /m /v t:\lib\c0s.obj sr ,sr.exe,, t:\lib\cs.lib

.c.obj:
	 bcc -b- -w-par -wpro -It:\include -v -r -c $<

.asm.obj:
    tasm /zi /m2 /ml $<

