#
#
# BumpVer makefile
#

.AUTODEPEND

CC = bcc
AS = tasm

CFLAGS = -2 -ms -c -C -v -G -w-par -w-rvl -w-pro -w-pia -It:\include -I..

PROJ = BumpVer
PROG = $(PROJ).exe

OBJDIR = OBJ
.path.obj = $(OBJDIR)

#
#

NOTHING:	$(PROG)

OBJS =	$(PROJ).obj \
	version.obj

$(PROG): $(OBJS)
	bumpver version.cpp
    	$(CC) $(CFLAGS) -o$(OBJDIR)\version.obj version.cpp
	cd $(OBJDIR)
        tlink /c /m /v @&&!
t:\lib\c0s.obj $(OBJS),..\$(PROG),,t:\lib\cs.lib
!
	cd ..

.cpp.obj:
    $(CC) $(CFLAGS) -o$(OBJDIR)\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,$(OBJDIR)\$*.obj

