#
#
# DeltaBin makefile
#
#

.AUTODEPEND

CC = bcc
AS = tasm

CFLAGS = -2 -ms -c -C -v -G -w-par -w-rvl -w-pro -w-pia -It:\include

PROJ = DeltaBin
PROG = $(PROJ).exe

.PATH.obj = OBJ

#
#

ALL:	$(PROG)

OBJS =	$(PROJ).obj \
	version.obj

$(PROG): $(OBJS)
	cd $(.PATH.obj)
        tlink /c /m /v @&&!
t:\lib\c0s.obj $(OBJS),..\$(PROG),,t:\lib\cs.lib
!
	cd ..

.cpp.obj:
    $(CC) $(CFLAGS) -o$(.PATH.obj)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(.PATH.obj)\$*.obj
