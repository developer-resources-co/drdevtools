#
# compress
#
# by William B. Norris IV
# Copyright 1994 Cave Logic Studios.  All Rights Reserved.
#
#

CC = bcc

MODEL = l
DEBUG = -v

CFLAGS = -m$(MODEL) $(DEBUG)

PROJ = compress
PROG = $(PROJ).exe

#

ALL:	 $(PROG)

OBJS =	\
	compress.obj \
	lzse.obj lzsd.obj \
	progmetr.obj

$(PROG): $(OBJS)
	tlink /m $(DEBUG) /Lt:\lib @<<
c0$(MODEL).obj $(OBJS),$(PROG),nul,c$(MODEL)
<<

.cpp.obj:
    $(CC) -c $(CFLAGS) -o$*.obj $<
