#
#
# Cardsave
#
#

CC = bcc
AS = tasm

MODEL = l

CFLAGS = -3 -m$(MODEL) -c -C -v -G -w-inl -w-par -w-rvl -w-pia -Is:\;t:\include -DSYSTEM$(SYSTEM)

PROJ = cardsave
PROG = $(PROJ).exe

#
#
#

ALL:	$(PROG)

OBJS =	\
	cardsave.obj \


cardsave.obj: cardsave.cpp version.h

$(PROG): $(OBJS)
	cd obj
        tlink /c /m /v /s /Lt:\lib @<<
c0$(MODEL).obj $(OBJS),..\$(PROG),,c$(MODEL).lib
<<
	cd ..

.cpp.obj:
    $(CC) $(CFLAGS) -oobj\$*.obj $<

.asm.obj:
    $(AS) /zi /m2 /ml $<,obj\$*.obj

