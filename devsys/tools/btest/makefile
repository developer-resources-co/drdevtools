
.AUTODEPEND

MODEL = l

btest.exe: btest.obj
	tlink /m /v t:\lib\c0$(MODEL).obj btest ,btest.exe,, t:\lib\c$(MODEL).lib

.cpp.obj:
	 bcc -m$(MODEL) -b- -w-par -wpro -It:\include -v -r -c $<

.asm.obj:
    tasm /zi /m2 /ml $<
