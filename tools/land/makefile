#==============================================================================
#
# Makefile: land
#
# By William B. Norris IV
# Copyright 1993,94 Cave Logic Studios.  All Rights Reserved.
#
#==============================================================================

CC = t:\bin\bcc
MODEL = s
# set DEBUG to blank or -v
DEBUG =

OBJDIR = OBJ^\

CFLAGS = $(DEBUG) -3 -It:\include -m$(MODEL)


OBJS =	$(OBJDIR)land.obj $(OBJDIR)fopen_lj.obj

#==============================================================================

ALL:	land.exe

CLEAN:
	del *.map
	del /QY obj
	rd obj
	del err
	del *.lst

#==============================================================================

land.exe: $(OBJS)
        tlink /m $(DEBUG) /Lt:\lib @<<
c0$(MODEL).obj $(OBJS),$@,,wildargs.obj c$(MODEL).lib
<<


.cpp{$(OBJDIR)}.obj:
	 $(CC) -c $(CFLAGS) -o$*.obj $<

#==============================================================================
