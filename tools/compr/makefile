
#
#
# Compr
#
#

.autodepend

CC = bcc
CFLAGS = -2 -mm -c -C -v -G -w-par -w-rvl -w-pro -w-pia -It:\include -I..\lib -DSYSTEM$(SYSTEM)

PROJ = compr
PROG = $(PREFIX)$(PROJ).exe
UNPROG = $(PREFIX)un$(PROJ).exe

OBJDIR = $(SYSTEM)OBJ
.path.obj = $(OBJDIR)

#
#

NOTHING:	$(PROG) $(UNPROG)

OBJS =	version.obj \
	compr.obj

$(PROG): $(OBJS)
	cd $(OBJDIR)
        tlink /c /m /v @&&!
t:\lib\c0m.obj $(OBJS),..\$(PROG),,t:\lib\cm.lib
!
	cd ..


UNOBJS = version.obj uncompr.obj

$(UNPROG): $(UNOBJS)
	cd $(OBJDIR)
        tlink /c /m /v @&&!
t:\lib\c0m.obj $(UNOBJS),..\$(UNPROG),,t:\lib\cm.lib
!
	cd ..

.cpp.obj:
    $(CC) $(CFLAGS) -o$(OBJDIR)\$*.obj $<

