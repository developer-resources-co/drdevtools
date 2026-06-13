COMPOPT = -mh -c -C -N -v -G -w-par -w-rvl -wpro -Id:\tc\include

chardiff.exe: chardiff.obj diffinpt.obj chardiff.mak chardiff.lnk
     tlink /m /v @chardiff.lnk

.c.obj:
    tcc $(COMPOPT) $<

.asm.obj:
    tasm $<

