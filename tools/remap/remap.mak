COMPOPT = -mh -c -C -N -v -G -w-par -w-rvl -wpro -Id:\tc\include

remap.exe: remap.obj reminpt.obj remap.mak remap.lnk
     tlink /m /v @remap.lnk

.c.obj:
    tcc $(COMPOPT) $<

.asm.obj:
    tasm $<

