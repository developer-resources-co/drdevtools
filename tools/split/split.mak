
split.exe: split.obj
	tlink /m /v c:\tc\lib\c0s.obj split,split.exe,, c:\tc\lib\cs.lib

.c.obj:
	 bcc -b- -w-par -wpro -Ic:\tc\include -v -r -c $<

.asm.obj:
    tasm /zi /m2 /ml $<
