#
# Makefile -- Alexandria Libraries
#
# Date Created:	10 Sep 93
#
# Copyright 1993 Alexandria, Inc.  All Rights Reserved.
#

LIB = cd
PROCESSOR = m

ALL:	lib$(LIB)$(PROCESSOR)$(DEBUG).68

!INCLUDE Makefile.obj

!IFDEF DEBUG
CFLAGS=-Os4f0 -Xf1 $(DEBUGGING)
AFLAGS=-dDEBUG=1  -dGEMS=0 -dSEGA_CD=1 -dMAINCPU=0 -dSUBCPU=1
!ELSE
CFLAGS=-Os4f0 $(OPTIMIZE)
AFLAGS=-dDEBUG=0  -dGEMS=0 -dSEGA_CD=1 -dMAINCPU=0 -dSUBCPU=1
!ENDIF



lib$(LIB)$(PROCESSOR)$(DEBUG).68:	.\obj$(LIB)$(PROCESSOR)$(DEBUG) $(CD_OBJS) Makefile
	-attrib -r $@
	-del $@
	lib68x -i <<
-C $@
$(CD_OBJS)
<<KEEP
	attrib +R $@


# libcd.mak -- EOF
