#
#
# RomOpt
#
#

.AUTODEPEND

CC = bcc
AS = tasm

DEBUG =
MODEL = l

CFLAGS = -3 -It:\include -Is:\ -m$(MODEL) -c -C $(DEBUG) -G -w-par -w-rvl -w-pro -w-pia

PROJ = romopt
PROG = $(PROJ).exe

.PATH.obj = obj

#
#

ALL:	$(PROG)

OBJS =  romopt.obj \
	coff.obj \
	version.obj

$(PROG): $(OBJS)
	cd $(.PATH.obj)
	tlink /c /m $(DEBUG) @&&!
t:\lib\c0$(MODEL).obj $(OBJS),..\$(PROG),,t:\lib\c$(MODEL).lib+s:\pclib\pclibb$(MODEL).lib
!
	cd ..

#
#

.cpp.obj:
    $(CC) $(CFLAGS) -o$(.PATH.obj)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /m$(MODEL) $<,$(.PATH.obj)\$*.obj

