#
#
# ROM Checksum
#
#

.autodepend

CC = bcc
AS = tasm

CFLAGS = -2 -ms -c -C -v -G -w-par -w-rvl -w-pro -w-pia -It:\include -DSYSTEM$(SYSTEM)

PROJ = chksum
PROG = $(PREFIX)$(PROJ)

OBJDIR = $(SYSTEM)OBJ
.path.obj = $(OBJDIR)

#
#

NOTHING:	UPDATE PREP $(PROG).exe

UPDATE:
	touch version.c
	checkin -i $(PROJ)

PREP:
	checkout -m $(PROJ) version.c


OBJS =	summer.obj \
	version.obj

$(PROG).exe: $(OBJS)
	cd $(OBJDIR)
        tlink /c /m /v @&&!
t:\lib\c0s.obj $(OBJS),..\$(PROG).exe,,t:\lib\cs.lib
!
	cd ..

.c.obj:
    $(CC) $(CFLAGS) -o$(OBJDIR)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(OBJDIR)\$*.obj

