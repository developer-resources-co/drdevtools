#
# dpshow
#	Deluxe Paint Viewer
#

.AUTODEPEND

CC = bcc
AS = tasm

MODEL = l

CFLAGS = -m$(MODEL) -v

.PATH.obj = obj

PROJ = dpshow
PROG = $(PROJ).exe

#

ALL:	 $(PROG)

OBJS =	\
	ilbm.obj \
	grabinpt.obj \
	quit.obj \
	grabber.obj \
	writepic.obj \
	version.obj


$(PROG): $(OBJS)
	cd $(.PATH.obj)
        tlink /c /m /v @&&!
t:\lib\c0$(MODEL).obj $(OBJS) s:\pclib\pclibb$(MODEL).lib,..\$(PROG),,t:\lib\c$(MODEL).lib
!
	cd ..

.cpp.obj:
    $(CC) -c $(CFLAGS) -o$(.PATH.obj)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(.PATH.obj)\$*.obj
