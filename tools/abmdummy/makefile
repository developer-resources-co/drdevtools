#
# abmdummy
#
#	Creates "dummy" placeholder AnimBrushes
#
# by William B. Norris IV
# (c) 1994 Cave Logic Studios.  All Rights Reserved.
#
#	2 Dec 93   WBNIV	Started from abmtrans
#

.AUTODEPEND

CC = bcc286
AS = tasm

MODEL = l

CFLAGS = -m$(MODEL) -v

.PATH.obj = obj

PROJ = abmdummy
PROG = $(PROJ).exe

#

ALL:	 $(PROG)

OBJS =	\
	abmdummy.obj \
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
