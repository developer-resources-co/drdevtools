#
# AbmScale
#
#

.AUTODEPEND

CC = bcc
AS = tasm

MODEL = l

#CFLAGS = -m$(MODEL)

#final version, optimized
!if !$d(DEBUG)
CFLAGS  = -3 -m$(MODEL) -Ox -C -G \
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

PROJ = abmscale
PROG = $(PROJ).exe

#

ALL:    $(PROG)

UPDATE:
	touch version.cpp
	checkin -i $(PROJ)

PREP:
	checkout -m $(PROJ) version.cpp

OBJS =  \
	anim.obj \
	grabinpt.obj \
	quit.obj \
	abmscale.obj \
	version.obj


$(PROG): $(OBJS)
	cd $(.PATH.obj)
	tlink /c /m /v @&&!
c0$(MODEL).obj $(OBJS),..\$(PROG),,s:\pclib\pclibb$(MODEL).lib c$(MODEL).lib
!
	cd ..

.cpp.obj:
    $(CC) -c $(CFLAGS) -o$(.PATH.obj)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(.PATH.obj)\$*.obj

