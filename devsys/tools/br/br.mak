
br.exe: br.obj
	tlink /m /v c:\tc\lib\c0s.obj br ,br.exe,, c:\tc\lib\cs.lib

.c.obj:
	 tcc -b- -w-par -wpro -Ic:\tc\include -v -r -c $<

.asm.obj:
    tasm /zi /m2 /ml $<
