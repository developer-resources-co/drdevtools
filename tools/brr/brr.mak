
all: brr.exe unbrr.exe

brr.exe: brr.obj
	tlink /m /v c:\tc\lib\c0s.obj brr,brr.exe,, c:\tc\lib\cs.lib


unbrr.exe: unbrr.obj
	tlink /m /v c:\tc\lib\c0s.obj unbrr,unbrr.exe,, c:\tc\lib\cs.lib


.c.obj:
	 tcc -b- -w-par -wpro -Ic:\tc\include -v -r -c $<

.asm.obj:
    tasm /zi /m2 /ml $<
