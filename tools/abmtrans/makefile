#
# transabm
#
#	Transition from one picture to another using an
#	AnimBrush as a stencil
#
# by William B. Norris IV
# (c) 1994 Cave Logic Studios.  All Rights Reserved.
#
#	28 Nov	WBNIV	Compiles under Phar Lap 286|DOS Extender
#

.AUTODEPEND

CC = bcc286
AS = tasm

MODEL = l

CFLAGS = -m$(MODEL) -v

.PATH.obj = obj

PROJ = transabm
PROG = $(PROJ).exe

#

ALL:	 $(PROG)

OBJS =	\
	grabber.obj \
	grabinpt.obj \
	quit.obj \
	version.obj \
\
	iffilbm.obj \
	iffpbm.obj \
	iffanim.obj \
	iffmisc.obj \


$(PROG): $(OBJS)
	cd $(.PATH.obj)
	tlink /m /v /Le:\run286\bc3\lib @&&!
c0pl.obj $(OBJS),..\$(PROG),nul,phapi bcl286 s:\pclib\pclibbl.lib,..\protmode
!
	cd ..

.cpp.obj:
    $(CC) -c $(CFLAGS) -o$(.PATH.obj)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(.PATH.obj)\$*.obj
