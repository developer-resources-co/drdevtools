#==============================================================================
#
# pclib.mak
#
#==============================================================================

#.SWAP                          Swap make out during command execution

###############################################################################

.autodepend

!if $(COMPILER) == BORLANDC
COMPINITIAL = b

CC = bcc286
#CFLAGS = -3 -m$(MODEL) -c -C -G -w-par -w-rvl -w-pro -w-pia -It:\include
#final version, optimized
!if !$d(DEBUG)
CFLAGS  = -c -3 -vi -m$(MODEL) -Ox -C -G \
	  -w-par -w-rvl -wpro -w-pia -w-inl \
	  -DNDEBUG \
	  -Is:\;t:\include
!endif

#debugging version, no optimazations
!if $d(DEBUG)
CFLAGS  = -c -v -3 -m$(MODEL) -C -G \
	  -w-par -w-rvl -wpro -w-pia -w-inl \
	  -Is:\;t:\include;c:\run286\inc
!endif

CDEBUG =  -v
COMPOUT = -o

AS = tasm
AFLAGS = /zi /m2 /ml
ASDEBUG = /zi

LN = tlink
LFLAGS = /c
LDEBUG = /v

LIB = tlib
LIBFLAGS =
LIBDEBUG =

!endif

#-------------------------------------------------------------------------------

!if $(COMPILER) == WATCOMC

COMPINITIAL = w

CC = wpp386
CFLAGS = -3r -I=s:\\
CDEBUG = -d2
COMPOUT = -fo=

AS = tasm
AFLAGS = /zi /m2 /ml
ASDEBUG = /zi

LN = wlink
LFLAGS =
LDEBUG =

LIB = wlib
LIBFLAGS =
LIBDEBUG =

!endif

#-------------------------------------------------------------------------------

!ifndef CFLAGS
!error Choose a compiler, bonehead
!endif

#===============================================================================

CCOMPILE = $(CC) $(CDEBUG) $(CFLAGS)
ASSEMBLE = $(AS) $(ADEBUG) $(AFLAGS)
LINK = $(LN) $(LDEBUG) $(LFLAGS)
LIBRARIAN = $(LIB) $(LIBDEBUG) $(LIBFLAGS)

###############################################################################

OBJDIR = OBJ$(MODEL)$(COMPINITIAL)
.PATH.obj = $(OBJDIR)

PROJ    = pclib
OUTPUT  = $(PROJ)$(COMPINITIAL)$(MODEL)$(WINDOWS).lib

###############################################################################

ALL:    $(OUTPUT)

#       board.obj snesio.obj \
#	~iffanim.obj \
OBJS =  \
	~iff.obj \
	~iffpbm.obj \
	~iffilbm.obj \
	~progmetr.obj \
	~profile.obj \
	~ini.obj \
	~regexp.obj \
	~regsub.obj \
	~regerror.obj \
	~list.obj \
	~error.obj \
	~general.obj \
	~target.obj \
	~fileio.obj \
	~grphport.obj \
	~cgrport.obj \
	~color.obj \
	~fixed.obj \
	~filename.obj \
	~mstream.obj \
	~output.obj \
	~stdstrm.obj \
	~tile.obj \
	~number.obj \

$(OUTPUT): $(OBJS:~= )
	cd $(.PATH.obj)
	$(LIBRARIAN) ..\$* @&&!
$(OBJS:~=-+)
!
	cd ..

###############################################################################

#.obj.lib:
#       $(LIB) pclib$(MODEL).lib /C /E -+$* ,pclib.lst

.cpp.obj:
    	$(CCOMPILE) $(COMPOUT)$(.PATH.obj)\$*.obj $<

.asm.obj:
    	$(ASSEMBLE) $<,$(.PATH.obj)\$*.obj

###############################################################################
