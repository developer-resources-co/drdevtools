#==============================================================================
#
# Makefile: spasm
#
# By Erik Anderson, Kevin T. Seghetti, and William B. Norris IV
# Copyright 1991,92,93,94 Developer Resources.  All Rights Reserved
#
#==============================================================================

AS = c:\borlandc\bin\tasm
CC = c:\borlandc\bin\bcc

OBJDIR = OBJ$(EXT)\\
DEBUG = -v

CFLAGS = -3 -w-par -wpro -w-pia -It:\include $(DEBUG) -r -c -D$(SYSTEM) -D$(PROCESSOR)
ASMFLAGS = /l /zi /d$(SYSTEM) /d$(PROCESSOR)


$(OBJDIR)spaz.obj: symbol.inc spasm.inc assemble.asm &
	board.asm patch.asm cint.asm parse.asm print.asm fileio.asm &
	symbol.asm error.asm list.asm macro.asm	pseudo.asm &
	opcode68.asm cam68.asm &
	opcode81.asm cam816.asm times816.asm &
	opcode70.asm cam700.asm

$(OBJDIR)spasm.obj: spasm.cpp version.h

OBJS =	$(OBJDIR)spasm.obj $(OBJDIR)eval.obj $(OBJDIR)token.obj &
	$(OBJDIR)spaz.obj $(OBJDIR)version.obj

#==============================================================================

NOTHING:


CLEAN:
	del *.map
	del /QY obj816
	rd obj816
	del /QY obj68k
	rd obj68k
	del /QY obj700
	rd obj700
	del err
	del *.lst

#==============================================================================

spasm.exe: $(OBJS)
#       tlink /m /3 $(DEBUG) @<<
#t:\lib\c0s.obj $(OBJS),$@,,t:\lib\cs.lib
#<<


spasm68k.exe: $(OBJS)
	echo /m /3 $(DEBUG) c:\borlandc\lib\c0s.obj $(OBJS),$@,,c:\borlandc\lib\cs.lib
	c:\borlandc\bin\tlink @spasm68k.ln


spasm700.exe: $(OBJS)
#        tlink /m /3 $(DEBUG) @<<
#t:\lib\c0s.obj $(OBJS),$@,,t:\lib\cs.lib
#<<



.cpp.obj:
	 $(CC) $(CFLAGS) -o$*.obj $<

.asm.obj:
    $(AS) $(ASMFLAGS) $<,$*.obj

#==============================================================================
