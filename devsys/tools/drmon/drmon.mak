#==============================================================================
#   drmon.mak makefile for snesmon & genmon
#==============================================================================

.SWAP
.AUTODEPEND

DEBUG = -v

#DEBUGTYPE = DEBUGCOFF
DEBUGTYPE = DEBUGDR
#DEBUGTYPE = DEBUGZARDOZ

!if $d(EMUL)
MONEMUL = EMUL
!endif

!if $d(__OS2__)

CC = bcc
INCLUDE = c:\bcos2\include;s:\\
CFLAGS = -N -I$(INCLUDE) $(DEBUG) -c $(MODEL) -D$(DEBUGTYPE) -DSYSTEM$(SYSTEM) -D$(MONEMUL)
LIBDIR = t:\lib
LIBRARIES = c:\bcos2\lib\c2.lib c:\bcos2\lib\os2.lib
STARTUP = c:\bcos2\lib\c02.obj

!elif $d(__WATCOMC__)

CC = wpp386
INCLUDE = c:\watcom\h;s:\\
CFLAGS = -I=$(INCLUDE) -oo -d2 -c -D$(DEBUGTYPE) -DSYSTEM$(SYSTEM) -D$(MONEMUL)
LIBDIR = t:\lib
LIBRARIES = cl
STARTUP = c0l.obj


!elif $d(PHARLAP)

CC = bcc286
INCLUDE = t:\include;s:\;c:\run286\inc
MODEL = -ml
#CFLAGS = -I$(INCLUDE) -c $(MODEL) -D$(DEBUGTYPE) -DSYSTEM$(SYSTEM) -D$(MONEMUL)
CFLAGS = -3 -N -I$(INCLUDE) $(DEBUG) -c $(MODEL) -D$(DEBUGTYPE) -DSYSTEM$(SYSTEM) -D$(MONEMUL)
LIBDIR = c:\run286\bc3\lib
LIBRARIES = phapi bcl286
DEFINITION = ..\protmode
STARTUP = c0pl.obj

!else

CC = bcc
INCLUDE = t:\include;s:\\
MODEL = -ml
#CFLAGS = -I$(INCLUDE) -c $(MODEL) -D$(DEBUGTYPE) -DSYSTEM$(SYSTEM) -D$(MONEMUL)
CFLAGS = -3 -N -I$(INCLUDE) $(DEBUG) -c $(MODEL) -D$(DEBUGTYPE) -DSYSTEM$(SYSTEM) -D$(MONEMUL)
LIBDIR = t:\lib
LIBRARIES = cl
STARTUP = c0l.obj

!endif


OBJDIR = $(SYSTEM)$(EMUL)$(PHARLAP)$(OS)OBJ
.PATH.obj = $(OBJDIR)

EXENAME = $(SYSTEM)$(EMUL)$(OS)mon.exe

#==============================================================================
# order of execution

ALL:    $(OBJDIR)\. bin\. bin\$(EXENAME) bin\$(SYSTEM)mon.hlp bin\$(SYSTEM)mon.prc

#==============================================================================

bin\.:
	-mkdir bin

$(OBJDIR)\.:
	-mkdir $(OBJDIR)

OBJS = \
	symbol.obj \
	$(SYSTEM)$(EMUL)io.obj \
	board.obj \
	listrect.obj \
	menu.obj \
	object.obj \
	layer.obj \
	app.obj \
	info.obj drmon.obj display.obj screen.obj  \
	list.obj memory.obj reg.obj mouse.obj window.obj gadget.obj \
	input.obj \
	macro.obj manager.obj \
	menubar.obj message.obj memops.obj break.obj \
	lexyy.obj ytab.obj  expr.obj \
	control.obj shell.obj textview.obj console.obj help.obj \
	command.obj error.obj cmdmacro.obj charts.obj \
	source.obj which.obj watch.obj \
	mem.obj \
	sld.obj coff.obj zardoz.obj \
	monmenu.obj \
	about.obj \
	general.obj config.obj \
	filereq.obj \
	profile.obj filename.obj \

bin\$(EXENAME): $(OBJS) drmon.mak
	cd $(OBJDIR)
	tlink /x -v /L$(LIBDIR) @&&!
$(STARTUP) $(OBJS),..\bin\$(EXENAME),nul,$(LIBRARIES),$(DEFINITION)
!
	cd ..


bin\$(SYSTEM)mon.hlp: $(SYSTEM)mon.hlp
	copy $. $@

bin\$(SYSTEM)mon.prc: $(SYSTEM)mon.prc
	copy $. $@

#==============================================================================

.cpp.obj:
	 $(CC) $(CFLAGS) -o$(OBJDIR)\$*.obj $<

.c.obj:
	 $(CC) $(CFLAGS) -o$(OBJDIR)\$*.obj $<

.asm.obj:
    tasm /zi $*.asm,$(OBJDIR)\$*.obj

#==============================================================================

