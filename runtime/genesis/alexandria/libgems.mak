#
# Makefile -- Alexandria Libraries
#
# Date Created:	10 Sep 93
#
# Copyright 1993 Alexandria, Inc.  All Rights Reserved.
#

LIB = gems

ALL:	lib$(LIB)$(DEBUG).68

!INCLUDE Makefile.obj

!IFDEF DEBUG
CFLAGS=-Os4f0 -Xf1 $(DEBUGGING) -DGEMS
AFLAGS=-dDEBUG=1  -dGEMS=1 -dSEGA_CD=0 -dSUBCPU=0 -dMAINCPU=1
!ELSE
CFLAGS=-Os4f0 $(OPTIMIZE) -DGEMS
AFLAGS=-dDEBUG=0  -dGEMS=1 -dSEGA_CD=0 -dSUBCPU=0 -dMAINCPU=1
!ENDIF



lib$(LIB)$(DEBUG).68:	.\obj$(LIB)$(DEBUG) $(GEMS_OBJS) lib$(LIB).mak
	-attrib -r $@
	-del $@
	lib68x -i <<
-C $@
$(GEMS_OBJS)
<<KEEP
	attrib +R $@


# libalx.mak -- EOF
