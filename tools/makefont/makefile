
.autodepend

CC = bcc
AS = tasm
CFLAGS = -O1 -2 -mh -c -C -N -v -G -w-par -w-rvl -wpro -w-pia -It:\include -DSNES
 
ALL: lbm2raw.exe 

lbm2raw.exe: grabber.obj general.obj gfx.obj grabinpt.obj compilbm.obj lbm2raw.lnk Makefile
	tlink /m /v @lbm2raw.lnk

general.obj: \gfxtools\lib\general.c
    $(CC) $(CFLAGS) -ogeneral.obj \gfxtools\lib\general.c

gfx.obj: \gfxtools\lib\gfx.c
    $(CC) $(CFLAGS) -ogfx.obj \gfxtools\lib\gfx.c

t.obj: \gfxtools\lib\t.c
    $(CC) $(CFLAGS) -ot.obj \gfxtools\lib\t.c

color.obj: \gfxtools\lib\color.c
    $(CC) $(CFLAGS) -ocolor.obj \gfxtools\lib\color.c

gui.obj: \gfxtools\lib\gui.c
    $(CC) $(CFLAGS) -ogui.obj \gfxtools\lib\gui.c

input.obj: \gfxtools\lib\input.c
    $(CC) $(CFLAGS) -oinput.obj \gfxtools\lib\input.c

gadget.obj: \gfxtools\lib\gadget.c
    $(CC) $(CFLAGS) -ogadget.obj \gfxtools\lib\gadget.c

image.obj: \gfxtools\lib\image.c
    $(CC) $(CFLAGS) -oimage.obj \gfxtools\lib\image.c

mouse.obj: \gfxtools\lib\mouse.c
    $(CC) $(CFLAGS) -omouse.obj \gfxtools\lib\mouse.c

filereq.obj: \gfxtools\lib\filereq.c
    $(CC) $(CFLAGS) -ofilereq.obj \gfxtools\lib\filereq.c


.c.obj:
    $(CC) $(CFLAGS) $<

.asm.obj:
    $(AS) $<
