#
# AnimationSplitter
#
#

.AUTODEPEND

CC = bcc286
AS = tasm

MODEL = l

#CFLAGS = -m$(MODEL)

#final version, optimized
!if !$d(DEBUG)
CFLAGS  = -3 -vi -m$(MODEL) -Ox -C -G \
	  -w-par -w-rvl -wpro -w-pia \
	  -Is:\;t:\include
!endif

#debugging version, no optimazations
!if $d(DEBUG)
CFLAGS  = -v -3 -m$(MODEL) -C -G \
	  -w-par -w-rvl -wpro -w-pia \
	  -Is:\;t:\include
!endif

.PATH.obj = obj

PROJ = abmsplit
PROG = $(PROJ).exe

#

ALL:	$(PROG)

OBJS =	\
	anim.obj \
	writepic.obj \
	grabinpt.obj \
	quit.obj \
	abmsplit.obj \
	version.obj


$(PROG): $(OBJS)
	cd $(.PATH.obj)
	-tlink /c /m /v /Lc:\run286\bc4\lib @&&!
c0p$(MODEL).obj $(OBJS),..\$(PROG),,phapi bcl286 s:\pclib\pclibb$(MODEL),..\protmode.def
!
	cd ..

.cpp.obj:
    $(CC) -c $(CFLAGS) -o$(.PATH.obj)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(.PATH.obj)\$*.obj

