#
#
# makefile for Untab
#
#

.autodepend

CC = bcc
AS = tasm

CFLAGS = -2 -ms -c -C -v -G -w-par -w-rvl -w-pro -w-pia -It:\include -I\gfxtools\lib -DSYSTEM$(SYSTEM)

PROJ = untab
PROG = $(PREFIX)$(PROJ).exe

OBJDIR = $(SYSTEM)OBJ
.path.obj = $(OBJDIR)

#
#

NOTHING:  $(PROG)

OBJS =	$(PROJ).obj \
	version.obj

$(PROG): $(OBJS)
	ss checkout version.cpp
	bumpver version.cpp
	ss update version.cpp -C-
	cd $(OBJDIR)
        tlink /c /m /v @&&!
t:\lib\c0s.obj $(OBJS),..\$(PROG),,t:\lib\cs.lib
!
	cd ..

.cpp.obj:
    $(CC) $(CFLAGS) -o$(OBJDIR)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(OBJDIR)\$*.obj

