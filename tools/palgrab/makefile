##############################################################################
#
# PalGrab
#
##############################################################################

CC = bcc
AS = tasm


MODEL = l
DEBUG =
CFLAGS = $(DEBUG) -m$(MODEL) -w-inl -It:\include;s:\\

PROJ = palgrab
PROG = $(PROJ).exe

OBJDIR = OBJ
.path.obj = $(OBJDIR)

#
#

NOTHING:	$(PROG)

OBJS =	$(PROJ).obj \
	iff.obj iffilbm.obj target.obj profile.obj error.obj general.obj \
	version.obj

$(PROG): $(OBJS)
	cd $(OBJDIR)
        tlink /c /m $(DEBUG) @&&!
c0$(MODEL).obj $(OBJS),..\$(PROG),nul,c$(MODEL).lib
!
	cd ..

.cpp.obj:
    $(CC) $(CFLAGS) -c -o$(OBJDIR)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /m$(MODEL) $<,$(OBJDIR)\$*.obj

