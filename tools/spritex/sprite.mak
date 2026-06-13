#==============================================================================

.AUTODEPEND

#CFLAGS = -ml -C -N -v -G -w-par -w-rvl -wpro -Id:\tc\include

CC = bcc
MODEL = l
CFLAGS = -N -w-par -wpro -ml -It:\include;s:\ -v -c -3 -Z -DSYSTEM$(SYSTEM) -D$(EMUL)

OBJDIR = obj
.PATH.obj = $(OBJDIR)
EXENAME = sprite.exe

#==============================================================================
# order of execution

nothing: UPDATE PREP $(EXENAME)

UPDATE:
	touch version.cpp
	checkin -i sprite


PREP:
	checkout -m sprite version.cpp


#FINISH:
#	checkout -e sprite version.cpp

#==============================================================================

#	t.obj a.obj \
OBJS = \
	sprite.obj general.obj sq.obj sprinpt.obj scolor.obj \
	compilbm.obj mouse.obj input.obj \
	spgadget.obj gadget.obj image.obj output.obj panel.obj\
	menu.obj text.obj \
	filereqb.obj charset.obj sysfont.obj \
	filereq.obj spmain.obj 

$(EXENAME): $(OBJS) sprite.mak s:\pclib\pclibl.lib
	cd $(OBJDIR)
        tlink /c /m /v @&&!
t:\lib\c0l.obj $(OBJS) s:\pclib\pclibl.lib,..\$(EXENAME),, t:\lib\cl.lib
!
	cd ..

#==============================================================================

.cpp.obj:
	 $(CC) $(CFLAGS) -o$(OBJDIR)\$*.obj $<

.c.obj:
	 $(CC) $(CFLAGS) -o$(OBJDIR)\$*.obj $<

.asm.obj:
    tasm /zi /m2 /ml $<,$(OBJDIR)\$*.obj

#==============================================================================

