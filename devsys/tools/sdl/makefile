#
#
# sdl -- SNES Downloader
#
# By William B. Norris IV
# (c) 1992,93 Developer Resources.  All Rights Reserved.
#
#

.AUTODEPEND

CC = bcc
AS = tasm

MODEL = l

#CFLAGS = -2 -m$(MODEL) -c -C -v -G -w-par -w-rvl -w-pia -Is:\;t:\include -DSYSTEM$(SYSTEM)
# Release:
CFLAGS = -O2 -d -w-par -wpro -m$(MODEL) -It:\include;s:\ -c -3 -Z -DSYSTEM$(SYSTEM)

PROJ = sdl
PROG = $(PROJ).exe

.PATH.obj = $(SYSTEM)OBJ

#
#
#

ALL:	$(PROG)

OBJS =	\
	sdl.obj \
	board.obj \
	version.obj

$(PROG): $(OBJS)
	cd $(.PATH.obj)
        tlink /c /m /s /Lt:\lib @&&!
c0$(MODEL).obj $(OBJS),..\$(PROG),,c$(MODEL).lib s:\pclib\pclib$(MODEL).lib
!
	cd ..

.cpp.obj:
    $(CC) $(CFLAGS) -o$(.PATH.obj)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(.PATH.obj)\$*.obj

