.autodepend

CC = bcc
AS = tasm

CFLAGS = -2 -mh -c -C -v -G -w-par -w-rvl -w-pro -w-pia -It:\include -I..\lib -DGENESIS

#\bin\snes\maped.exe: maped.exe
#	copy maped.exe \bin\snes
# 	tdstrip \bin\snes\maped.exe

maped.exe: GUI.obj GUIinit.obj render.obj GUIdefs.obj mapfunc.obj \
	gfx.obj mouse.obj general.obj font6.obj \
        guifunc.obj maped.obj sgtools.obj maped.lnk\
	mapinpt.obj input.obj iff.obj writepic.obj print.obj\
	t.obj \
	closemsg.obj
        	tlink /v @maped.lnk

gui.obj: \gfxtools\lib\gui.c
    $(CC) $(CFLAGS) -ogui.obj \gfxtools\lib\gui.c

mouse.obj: \gfxtools\lib\mouse.c
    $(CC) $(CFLAGS) -omouse.obj \gfxtools\lib\mouse.c

general.obj: \gfxtools\lib\general.c
    $(CC) $(CFLAGS) -ogeneral.obj \gfxtools\lib\general.c

gfx.obj: \gfxtools\lib\gfx.c
    $(CC) $(CFLAGS) -ogfx.obj \gfxtools\lib\gfx.c

input.obj: \gfxtools\lib\input.c
    $(CC) $(CFLAGS) -oinput.obj \gfxtools\lib\input.c

t.obj: \gfxtools\lib\t.c
    $(CC) $(CFLAGS) -DMAPED -ot.obj \gfxtools\lib\t.c

.c.obj:
    $(CC) $(CFLAGS) $<

.asm.obj:
    $(AS) $<
