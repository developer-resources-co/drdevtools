#
# Makefile -- Alexandria Libraries
#
# Date Created:	10 Sep 93
#
# Copyright 1993 Alexandria, Inc.  All Rights Reserved.
#

LIB = alx

ALL:	lib$(LIB)$(DEBUG).68

!INCLUDE Makefile.obj

!IFDEF DEBUG
CFLAGS=-Os4f0 -Xf1 $(DEBUGGING)
AFLAGS=-dDEBUG=1  -dGEMS=0 -dSEGA_CD=0 -dSUBCPU=0 -dMAINCPU=1
!ELSE
CFLAGS=-Os4f0 $(OPTIMIZE)
AFLAGS=-dDEBUG=0  -dGEMS=0 -dSEGA_CD=0 -dSUBCPU=0 -dMAINCPU=1
!ENDIF



lib$(LIB)$(DEBUG).68:	.\obj$(LIB)$(DEBUG) $(OBJS) Makefile
	-attrib -r $@
	-del $@
	lib68x -i <<
-C $@
$(OBJS)
<<
	attrib +R $@


# libalx.mak -- EOF
