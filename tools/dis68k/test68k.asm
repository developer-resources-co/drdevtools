
;       68000 CROSS ASSEMBLER TEST ROUTINE.

                .LINKLIST
                .SYMBOLS
                .PL     60

                ;.EXTERNAL      ADDRESS1, ADDRESS2
                ;.EXTERNAL      DATA8, DATA16, DATA32
                ;.EXTERNAL      OFFSET2

                ORG     1234

                GLOBALS ON
LABEL           .DS     2
LABEL1          .DS     4
LABEL2          NOP
                .BLKB   10H

ADDRESS1:       .EQUAL  $
DATA8:          .EQUAL  ABH
DATA16:         .EQUAL  1234H
DATA32:         .EQUAL  89ABCDEFH
QUICK:          .EQUAL  2
OFFSET1:        .EQUAL  12H
OFFSET2:        .EQUAL  34H

ABCD:           ABCD    D0,D7
                ABCD    D7,D0
                ABCD    D4,D6
                ABCD    (A0),(A7)
                ABCD    (A7),(A0)
                ABCD    (A4),(A6)
                ABCD    -(A4),-(A6)

ADD:            ADD     D4,D6
                ADD     A4,D6
                ADD     (A4),D6
                ADD     (A4)+,D6
                ADD     -(A4),D6
                ADD     ADDRESS2(A4),D6
                ADD     OFFSET2(A4,A5),D6
                ADD     OFFSET2(A4,A5.L),D6
                ADD     OFFSET2(A4,D5),D6
                ADD     OFFSET2(A4,D5.L),D6
                ADD     ADDRESS1,D6
                ADD     ADDRESS2,D6
                ADD     OFFSET1(PC),D6
                ADD     OFFSET1(PC,A5),D6
                ADD     OFFSET1(PC,A5.L),D6
                ADD     OFFSET1(PC,D5),D6
                ADD     OFFSET1(PC,D5.L),D6
                ADD     #10,D6
                ADD     #ADDRESS1,D6

                ADD     D4,D6
                ADD     D4,A6
                ADD     D4,(A6)
                ADD     D4,(A6)+
                ADD     D4,-(A6)
                ADD     D4,ADDRESS2(A6)
                ADD     D4,OFFSET1(A6,A4)
                ADD     D4,OFFSET1(A6,A4.L)
                ADD     D4,OFFSET1(A6,D4)
                ADD     D4,OFFSET1(A6,D4.L)
//----------------------------------------------------------------------------
                ADD     D4,ADDRESS1
                ADD     D4,ADDRESS2

                ADD     D4,A6
                ADD     A4,A6
                ADD     (A4),A6
                ADD     (A4)+,A6
                ADD     -(A4),A6
                ADD     ADDRESS2(A4),A6
                ADD     OFFSET2(A4,A6),A6
                ADD     OFFSET2(A4,A6.L),A6
//----------------------------------------------------------------------------
                ADD     OFFSET2(A4,D6),A6
                ADD     OFFSET2(A4,D6.L),A6
                ADD     ADDRESS1,A6
                ADD     ADDRESS2,A6
                ADD     OFFSET1(PC),A6
                ADD     OFFSET1(PC,A6),A6
                ADD     OFFSET1(PC,A6.L),A6
                ADD     OFFSET1(PC,D6),A6
                ADD     OFFSET1(PC,D6.L),A6
                ADD     #10,A6
                ADD     #ADDRESS1,A6

//----------------------------------------------------------------------------
                ADD     #10,D4
                ADD     #ADDRESS1,D4
                ADD     #10,(A4)
                ADD     #ADDRESS1,(A4)
                ADD     #ADDRESS1,(A4)+
                ADD     #ADDRESS1,-(A4)
                ADD     #ADDRESS1,ADDRESS2(A4)
                ADD     #ADDRESS1,OFFSET2(A4,A6)
//----------------------------------------------------------------------------
                ADD     #ADDRESS1,OFFSET2(A4,A6.L)
                ADD     #ADDRESS1,OFFSET2(A4,D6)
                ADD     #ADDRESS1,OFFSET2(A4,D6.L)
                ADD     #ADDRESS1,ADDRESS1
                ADD     #ADDRESS1,ADDRESS2

                ADD     #1,D4
                ADDQ    #1,D4
                ADD     #8,D4
                ADDQ    #8,D4
                ADD     #QUICK,D4
//----------------------------------------------------------------------------
                ADDQ    #QUICK,D4
                ADD     #QUICK,A4
                ADDQ    #QUICK,A4
                ADD     #QUICK,(A4)
                ADDQ    #QUICK,(A4)
                ADD     #QUICK,(A4)+
                ADDQ    #QUICK,(A4)+
                ADD     #QUICK,-(A4)
//----------------------------------------------------------------------------
                ADDQ    #QUICK,-(A4)
                ADD     #QUICK,ADDRESS2(A4)
                ADDQ    #QUICK,ADDRESS2(A4)
                ADD     #QUICK,OFFSET2(A4,A6)
                ADDQ    #QUICK,OFFSET2(A4,A6)
                ADD     #QUICK,OFFSET2(A4,A6.L)
                ADDQ    #QUICK,OFFSET2(A4,A6.L)
                ADD     #QUICK,OFFSET2(A4,D6)
                ADDQ    #QUICK,OFFSET2(A4,D6)
                ADD     #QUICK,OFFSET2(A4,D6.L)
                ADDQ    #QUICK,OFFSET2(A4,D6.L)
//----------------------------------------------------------------------------


ADDA:           ADDA    D4,A6
                ADDA    A4,A6
                ADDA    (A4),A6
                ADDA    (A4)+,A6
                ADDA    -(A4),A6
                ADDA    ADDRESS2(A4),A6
                ADDA    OFFSET2(A4,A6),A6
                ADDA    OFFSET2(A4,A6.L),A6
                ADDA    OFFSET2(A4,D6),A6
//----------------------------------------------------------------------------
                ADDA    OFFSET2(A4,D6.L),A6
                ADDA    ADDRESS1,A6
                ADDA    ADDRESS2,A6
                ADDA    OFFSET1(PC),A6
                ADDA    OFFSET1(PC,A6),A6
                ADDA    OFFSET1(PC,A6.L),A6
                ADDA    OFFSET1(PC,D6),A6
                ADDA    OFFSET1(PC,D6.L),A6
                ADDA    #10,A6
                ADDA    #ADDRESS1,A6

//----------------------------------------------------------------------------

ADDI:           ADDI    #10,D4
                ADDI    #ADDRESS1,D4
                ADDI    #10,(A4)
                ADDI    #ADDRESS1,(A4)
                ADDI    #ADDRESS1,(A4)+
                ADDI    #ADDRESS1,-(A4)
                ADDI    #ADDRESS1,ADDRESS2(A4)
                ADDI    #ADDRESS1,OFFSET2(A4,A6)
                ADDI    #ADDRESS1,OFFSET2(A4,A6.L)
                ADDI    #ADDRESS1,OFFSET2(A4,D6)
//----------------------------------------------------------------------------

                ADDI    #ADDRESS1,ADDRESS1
                ADDI    #ADDRESS1,ADDRESS2


ADD.W:          ADD.W   D4,D6
                ADD.W   A4,D6
                ADD.W   (A4),D6
                ADD.W   (A4)+,D6
                ADD.W   -(A4),D6
                ADD.W   ADDRESS2(A4),D6
//----------------------------------------------------------------------------
                ADD.W   OFFSET2(A4,A5),D6
                ADD.W   OFFSET2(A4,A5.L),D6
                ADD.W   OFFSET2(A4,D5),D6
                ADD.W   OFFSET2(A4,D5.L),D6
                ADD.W   ADDRESS1,D6
                ADD.W   ADDRESS2,D6
                ADD.W   OFFSET1(PC),D6
                ADD.W   OFFSET1(PC,A5),D6
                ADD.W   OFFSET1(PC,A5.L),D6
                ADD.W   OFFSET1(PC,D5),D6
                ADD.W   OFFSET1(PC,D5.L),D6
//----------------------------------------------------------------------------
                ADD.W   #10,D6
                ADD.W   #ADDRESS1,D6

                ADD.W   D4,D6
                ADD.W   D4,A6
                ADD.W   D4,(A6)
                ADD.W   D4,(A6)+
                ADD.W   D4,-(A6)
                ADD.W   D4,ADDRESS2(A6)
                ADD.W   D4,OFFSET1(A6,A4)
                ADD.W   D4,OFFSET1(A6,A4.L)
//----------------------------------------------------------------------------
                ADD.W   D4,OFFSET1(A6,D4)
                ADD.W   D4,OFFSET1(A6,D4.L)
                ADD.W   D4,ADDRESS1
                ADD.W   D4,ADDRESS2

                ADD.W   D4,A6
                ADD.W   A4,A6
                ADD.W   (A4),A6
                ADD.W   (A4)+,A6
.                ADD.W   -(A4),A6
                ADD.W   ADDRESS2(A4),A6
                ADD.W   OFFSET2(A4,A6),A6
//----------------------------------------------------------------------------
                ADD.W   OFFSET2(A4,A6.L),A6
                ADD.W   OFFSET2(A4,D6),A6
                ADD.W   OFFSET2(A4,D6.L),A6
                ADD.W   ADDRESS1,A6
                ADD.W   ADDRESS2,A6
                ADD.W   OFFSET1(PC),A6
                ADD.W   OFFSET1(PC,A6),A6
                ADD.W   OFFSET1(PC,A6.L),A6
                ADD.W   OFFSET1(PC,D6),A6
                ADD.W   OFFSET1(PC,D6.L),A6
                ADD.W   #10,A6
//----------------------------------------------------------------------------
                ADD.W   #ADDRESS1,A6

                ADD.W   #10,D4
                ADD.W   #ADDRESS1,D4
                ADD.W   #10,(A4)
                ADD.W   #ADDRESS1,(A4)
                ADD.W   #ADDRESS1,(A4)+
                ADD.W   #ADDRESS1,-(A4)
                ADD.W   #ADDRESS1,ADDRESS2(A4)
                ADD.W   #ADDRESS1,OFFSET2(A4,A6)
                ADD.W   #ADDRESS1,OFFSET2(A4,A6.L)
                ADD.W   #ADDRESS1,OFFSET2(A4,D6)
                ADD.W   #ADDRESS1,OFFSET2(A4,D6.L)
//----------------------------------------------------------------------------
                ADD.W   #ADDRESS1,ADDRESS1
                ADD.W   #ADDRESS1,ADDRESS2

                ADD.W   #1,D4
                ADDQ.W  #1,D4
                ADD.W   #8,D4
                ADDQ.W  #8,D4
                ADD.W   #QUICK,D4
                ADDQ.W  #QUICK,D4
                ADD.W   #QUICK,A4
                ADDQ.W  #QUICK,A4
                ADD.W   #QUICK,(A4)
                ADDQ.W  #QUICK,(A4)
                ADD.W   #QUICK,(A4)+
                ADDQ.W  #QUICK,(A4)+
                ADD.W   #QUICK,-(A4)
                ADDQ.W  #QUICK,-(A4)
                ADD.W   #QUICK,ADDRESS2(A4)
                ADDQ.W  #QUICK,ADDRESS2(A4)
                ADD.W   #QUICK,OFFSET2(A4,A6)
                ADDQ.W  #QUICK,OFFSET2(A4,A6)
                ADD.W   #QUICK,OFFSET2(A4,A6.L)
                ADDQ.W  #QUICK,OFFSET2(A4,A6.L)
                ADD.W   #QUICK,OFFSET2(A4,D6)
                ADDQ.W  #QUICK,OFFSET2(A4,D6)
                ADD.W   #QUICK,OFFSET2(A4,D6.L)
                ADDQ.W  #QUICK,OFFSET2(A4,D6.L)


ADDA.W:         ADDA.W  D4,A6
                ADDA.W  A4,A6
                ADDA.W  (A4),A6
                ADDA.W  (A4)+,A6
                ADDA.W  -(A4),A6
                ADDA.W  ADDRESS2(A4),A6
                ADDA.W  OFFSET2(A4,A6),A6
                ADDA.W  OFFSET2(A4,A6.L),A6
                ADDA.W  OFFSET2(A4,D6),A6
                ADDA.W  OFFSET2(A4,D6.L),A6
                ADDA.W  ADDRESS1,A6
                ADDA.W  ADDRESS2,A6
                ADDA.W  OFFSET1(PC),A6
                ADDA.W  OFFSET1(PC,A6),A6
                ADDA.W  OFFSET1(PC,A6.L),A6
                ADDA.W  OFFSET1(PC,D6),A6
                ADDA.W  OFFSET1(PC,D6.L),A6
                ADDA.W  #10,A6
                ADDA.W  #ADDRESS1,A6


ADDI.W:         ADDI.W  #10,D4
                ADDI.W  #ADDRESS1,D4
                ADDI.W  #10,(A4)
                ADDI.W  #ADDRESS1,(A4)
                ADDI.W  #ADDRESS1,(A4)+
                ADDI.W  #ADDRESS1,-(A4)
                ADDI.W  #ADDRESS1,ADDRESS2(A4)
                ADDI.W  #ADDRESS1,OFFSET2(A4,A6)
                ADDI.W  #ADDRESS1,OFFSET2(A4,A6.L)
                ADDI.W  #ADDRESS1,OFFSET2(A4,D6)
                ADDI.W  #ADDRESS1,OFFSET2(A4,D6.L)
                ADDI.W  #ADDRESS1,ADDRESS1
                ADDI.W  #ADDRESS1,ADDRESS2


ADD.B:          ADD.B   D4,D6
                ADD.B   (A4),D6
                ADD.B   (A4)+,D6
                ADD.B   -(A4),D6
                ADD.B   ADDRESS2(A4),D6
                ADD.B   OFFSET2(A4,A5),D6
                ADD.B   OFFSET2(A4,A5.L),D6
                ADD.B   OFFSET2(A4,D5),D6
                ADD.B   OFFSET2(A4,D5.L),D6
                ADD.B   ADDRESS1,D6
                ADD.B   ADDRESS2,D6
                ADD.B   OFFSET1(PC),D6
                ADD.B   OFFSET1(PC,A5),D6
                ADD.B   OFFSET1(PC,A5.L),D6
                ADD.B   OFFSET1(PC,D5),D6
                ADD.B   OFFSET1(PC,D5.L),D6
                ADD.B   #10,D6
                ADD.B   #DATA8,D6

                ADD.B   D4,D6
                ADD.B   D4,(A6)
                ADD.B   D4,(A6)+
                ADD.B   D4,-(A6)
                ADD.B   D4,ADDRESS2(A6)
                ADD.B   D4,OFFSET1(A6,A4)
                ADD.B   D4,OFFSET1(A6,A4.L)
                ADD.B   D4,OFFSET1(A6,D4)
                ADD.B   D4,OFFSET1(A6,D4.L)
                ADD.B   D4,ADDRESS1
                ADD.B   D4,ADDRESS2

                ADD.B   #10,D4
                ADD.B   #DATA8,D4
                ADD.B   #10,(A4)
                ADD.B   #DATA8,(A4)
                ADD.B   #DATA8,(A4)+
                ADD.B   #DATA8,-(A4)
                ADD.B   #DATA8,ADDRESS2(A4)
                ADD.B   #DATA8,OFFSET2(A4,A6)
                ADD.B   #DATA8,OFFSET2(A4,A6.L)
                ADD.B   #DATA8,OFFSET2(A4,D6)
                ADD.B   #DATA8,OFFSET2(A4,D6.L)
                ADD.B   #DATA8,ADDRESS1
                ADD.B   #DATA8,ADDRESS2

                ADD.B   #1,D4
                ADDQ.B  #1,D4
                ADD.B   #8,D4
                ADDQ.B  #8,D4
                ADD.B   #QUICK,D4
                ADDQ.B  #QUICK,D4
                ADD.B   #QUICK,(A4)
                ADDQ.B  #QUICK,(A4)
                ADD.B   #QUICK,(A4)+
                ADDQ.B  #QUICK,(A4)+
                ADD.B   #QUICK,-(A4)
                ADDQ.B  #QUICK,-(A4)
                ADD.B   #QUICK,ADDRESS2(A4)
                ADDQ.B  #QUICK,ADDRESS2(A4)
                ADD.B   #QUICK,OFFSET2(A4,A6)
                ADDQ.B  #QUICK,OFFSET2(A4,A6)
                ADD.B   #QUICK,OFFSET2(A4,A6.L)
                ADDQ.B  #QUICK,OFFSET2(A4,A6.L)
                ADD.B   #QUICK,OFFSET2(A4,D6)
                ADDQ.B  #QUICK,OFFSET2(A4,D6)
                ADD.B   #QUICK,OFFSET2(A4,D6.L)
                ADDQ.B  #QUICK,OFFSET2(A4,D6.L)


ADDI.B:         ADDI.B  #10,D4
                ADDI.B  #DATA8,D4
                ADDI.B  #10,(A4)
                ADDI.B  #DATA8,(A4)
                ADDI.B  #DATA8,(A4)+
                ADDI.B  #DATA8,-(A4)
%ŸVp
                ADDI.B  #DATA8,OFFSET2(A4,A6)
%ŸVp
                ADDI.B  #DATA8,OFFSET2(A4,D6)
%ŸVp
                ADDI.B  #DATA8,ADDRESS1
                ADDI.B  #DATA8,ADDRESS2


ADD.L:          ADD.L   D4,D6
                ADD.L   A4,D6
                ADD.L   (A4),D6
                ADD.L   (A4)+,D6
%ŸVp
                ADD.L   ADDRESS2(A4),D6
%ŸVp
                ADD.L   OFFSET2(A4,A5.L),D6
%ŸVp
                ADD.L   OFFSET2(A4,D5.L),D6
                ADD.L   ADDRESS1,D6
%ŸVp
                ADD.L   OFFSET1(PC),D6
%ŸVp
                ADD.L   OFFSET1(PC,A5.L),D6
%ŸVp
                ADD.L   OFFSET1(PC,D5.L),D6
                ADD.L   #10,D6
%ŸVp

                ADD.L   D4,D6
%ŸVp
                ADD.L   D4,(A6)
%ŸVp
                ADD.L   D4,-(A6)
%ŸVp
                ADD.L   D4,OFFSET1(A6,A4)
%ŸVp
                ADD.L   D4,OFFSET1(A6,D4)
%ŸVp
                ADD.L   D4,ADDRESS1
                ADD.L   D4,ADDRESS2

                ADD.L   D4,A6
%ŸVp
                ADD.L   (A4),A6
%ŸVp
                ADD.L   -(A4),A6
%ŸVp
                ADD.L   OFFSET2(A4,A6),A6
%ŸVp
                ADD.L   OFFSET2(A4,D6),A6
%ŸVp
                ADD.L   ADDRESS1,A6
                ADD.L   ADDRESS2,A6
%ŸVp
                ADD.L   OFFSET1(PC,A6),A6
%ŸVp
                ADD.L   OFFSET1(PC,D6),A6
%ŸVp
                ADD.L   #10,A6
                ADD.L   #DATA32,A6

%ŸVp
                ADD.L   #DATA32,D4
%ŸVp
                ADD.L   #DATA32,(A4)
%ŸVp
                ADD.L   #DATA32,-(A4)
%ŸVp
                ADD.L   #DATA32,OFFSET2(A4,A6)
%ŸVp
                ADD.L   #DATA32,OFFSET2(A4,D6)
                ADD.L   #DATA32,OFFSET2(A4,D6.L)
%ŸVp
                ADD.L   #DATA32,ADDRESS2

%ŸVp
                ADDQ.L  #1,D4
%ŸVp
                ADDQ.L  #8,D4
%ŸVp
                ADDQ.L  #QUICK,D4
%ŸVp
                ADDQ.L  #QUICK,A4
%ŸVp
                ADDQ.L  #QUICK,(A4)
%ŸVp
                ADDQ.L  #QUICK,(A4)+
%ŸVp
                ADDQ.L  #QUICK,-(A4)
%ŸVp
                ADDQ.L  #QUICK,ADDRESS2(A4)
%ŸVp
                ADDQ.L  #QUICK,OFFSET2(A4,A6)
%ŸVp
                ADDQ.L  #QUICK,OFFSET2(A4,A6.L)
%ŸVp
                ADDQ.L  #QUICK,OFFSET2(A4,D6)
%ŸVp
                ADDQ.L  #QUICK,OFFSET2(A4,D6.L)


%ŸVp
                ADDA.L  A4,A6
%ŸVp
                ADDA.L  (A4)+,A6
%ŸVp
                ADDA.L  ADDRESS2(A4),A6
%ŸVp
                ADDA.L  OFFSET2(A4,A6.L),A6
%ŸVp
                ADDA.L  OFFSET2(A4,D6.L),A6
                ADDA.L  ADDRESS1,A6
%ŸVp
                ADDA.L  OFFSET1(PC),A6
%ŸVp
                ADDA.L  OFFSET1(PC,A6.L),A6
%ŸVp
                ADDA.L  OFFSET1(PC,D6.L),A6
                ADDA.L  #10,A6
%ŸVp


%ŸVp
                ADDI.L  #DATA32,D4
%ŸVp
                ADDI.L  #DATA32,(A4)
%ŸVp
                ADDI.L  #DATA32,-(A4)
%ŸVp
                ADDI.L  #DATA32,OFFSET2(A4,A6)
%ŸVp
                ADDI.L  #DATA32,OFFSET2(A4,D6)
%ŸVp
                ADDI.L  #DATA32,ADDRESS1
%ŸVp


ADDX:           ADDX    D0,D7
                ADDX    D7,D0
%ŸVp
                ADDX.W  D0,D7
%ŸVp
                ADDX.W  D4,D6
%ŸVp
                ADDX    -(A7),-(A0)
%ŸVp
                ADDX.W  -(A0),-(A7)
%ŸVp
                ADDX.W  -(A4),-(A6)

%ŸVp
                ADDX.B  D7,D0
%ŸVp
                ADDX.B  -(A0),-(A7)
%ŸVp
                ADDX.B  -(A4),-(A6)

%ŸVp
                ADDX.L  D7,D0
%ŸVp
                ADDX.L  -(A0),-(A7)
%ŸVp
                ADDX.L  -(A4),-(A6)


%ŸVp
                AND     (A4),D6
%ŸVp
                AND     -(A4),D6
%ŸVp
                AND     OFFSET2(A4,A5),D6
%ŸVp
                AND     OFFSET2(A4,D5),D6
%ŸVp
                AND     ADDRESS1,D6
%ŸVp
                AND     OFFSET1(PC),D6
%ŸVp
                AND     OFFSET1(PC,A5.L),D6
%ŸVp
                AND     OFFSET1(PC,D5.L),D6
                AND     #10,D6
%ŸVp

                AND     D4,D6
%ŸVp
                AND     D4,(A6)+
%ŸVp
                AND     D4,ADDRESS2(A6)
%ŸVp
                AND     D4,OFFSET1(A6,A4.L)
%ŸVp
                AND     D4,OFFSET1(A6,D4.L)
                AND     D4,ADDRESS1
%ŸVp

                AND     #10,D4
%ŸVp
                AND     #10,(A4)
%ŸVp
                AND     #ADDRESS1,(A4)+
%ŸVp
                AND     #ADDRESS1,ADDRESS2(A4)
%ŸVp
                AND     #ADDRESS1,OFFSET2(A4,A6.L)
%ŸVp
                AND     #ADDRESS1,OFFSET2(A4,D6.L)
%ŸVp
                AND     #ADDRESS1,ADDRESS2


%ŸVp
                ANDI    #ADDRESS1,D4
%ŸVp
                ANDI    #ADDRESS1,(A4)
%ŸVp
                ANDI    #ADDRESS1,-(A4)
%ŸVp
                ANDI    #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                ANDI    #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                ANDI    #ADDRESS1,ADDRESS1
%ŸVp


AND.W:          AND.W   D4,D6
                AND.W   (A4),D6
%ŸVp
                AND.W   -(A4),D6
%ŸVp
                AND.W   OFFSET2(A4,A5),D6
%ŸVp
                AND.W   OFFSET2(A4,D5),D6
%ŸVp
                AND.W   ADDRESS1,D6
                AND.W   ADDRESS2,D6
%ŸVp
                AND.W   OFFSET1(PC,A5),D6
%ŸVp
                AND.W   OFFSET1(PC,D5),D6
%ŸVp
                AND.W   #10,D6
                AND.W   #ADDRESS1,D6

                AND.W   D4,D6
                AND.W   D4,(A6)
%ŸVp
                AND.W   D4,-(A6)
%ŸVp
                AND.W   D4,OFFSET1(A6,A4)
%ŸVp
                AND.W   D4,OFFSET1(A6,D4)
%ŸVp
                AND.W   D4,ADDRESS1
                AND.W   D4,ADDRESS2

                AND.W   #10,D4
                AND.W   #ADDRESS1,D4
%ŸVp
                AND.W   #ADDRESS1,(A4)
%ŸVp
                AND.W   #ADDRESS1,-(A4)
%ŸVp
                AND.W   #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                AND.W   #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                AND.W   #ADDRESS1,ADDRESS1
%ŸVp


ANDI.W:         ANDI.W  #10,D4
                ANDI.W  #ADDRESS1,D4
%ŸVp
                ANDI.W  #ADDRESS1,(A4)
%ŸVp
                ANDI.W  #ADDRESS1,-(A4)
%ŸVp
                ANDI.W  #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                ANDI.W  #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                ANDI.W  #ADDRESS1,ADDRESS1
%ŸVp


AND.B:          AND.B   D4,D6
                AND.B   (A4),D6
%ŸVp
                AND.B   -(A4),D6
%ŸVp
                AND.B   OFFSET2(A4,A5),D6
%ŸVp
                AND.B   OFFSET2(A4,D5),D6
%ŸVp
                AND.B   ADDRESS1,D6
                AND.B   ADDRESS2,D6
                AND.B   OFFSET1(PC),D6
%ŸVp
                AND.B   OFFSET1(PC,A5.L),D6
%ŸVp
                AND.B   OFFSET1(PC,D5.L),D6
                AND.B   #10,D6
%ŸVp

                AND.B   D4,D6
                AND.B   D4,(A6)
%ŸVp
                AND.B   D4,-(A6)
%ŸVp
                AND.B   D4,OFFSET1(A6,A4)
%ŸVp
                AND.B   D4,OFFSET1(A6,D4)
%ŸVp
                AND.B   D4,ADDRESS1
                AND.B   D4,ADDRESS2

                AND.B   #10,D4
                AND.B   #DATA8,D4
%ŸVp
                AND.B   #DATA8,(A4)
%ŸVp
                AND.B   #DATA8,-(A4)
%ŸVp
                AND.B   #DATA8,OFFSET2(A4,A6)
%ŸVp
                AND.B   #DATA8,OFFSET2(A4,D6)
%ŸVp
                AND.B   #DATA8,ADDRESS1
                AND.B   #DATA8,ADDRESS2


%ŸVp
                ANDI.B  #DATA8,D4
%ŸVp
                ANDI.B  #DATA8,(A4)
%ŸVp
                ANDI.B  #DATA8,-(A4)
%ŸVp
                ANDI.B  #DATA8,OFFSET2(A4,A6)
%ŸVp
                ANDI.B  #DATA8,OFFSET2(A4,D6)
%ŸVp
                ANDI.B  #DATA8,ADDRESS1


AND.L:          AND.L   D4,D6
                AND.L   (A4),D6
%ŸVp
                AND.L   -(A4),D6
%ŸVp
                AND.L   OFFSET2(A4,A5),D6
%ŸVp
                AND.L   OFFSET2(A4,D5),D6
%ŸVp
                AND.L   ADDRESS1,D6
                AND.L   ADDRESS2,D6
%ŸVp
                AND.L   OFFSET1(PC,A5),D6
%ŸVp
                AND.L   OFFSET1(PC,D5),D6
%ŸVp
                AND.L   #10,D6
                AND.L   #DATA32,D6

%ŸVp
                AND.L   D4,(A6)
%ŸVp
                AND.L   D4,-(A6)
%ŸVp
                AND.L   D4,OFFSET1(A6,A4)
%ŸVp
                AND.L   D4,OFFSET1(A6,D4)
%ŸVp
                AND.L   D4,ADDRESS1
                AND.L   D4,ADDRESS2

%ŸVp
                AND.L   #DATA32,D4
%ŸVp
                AND.L   #DATA32,(A4)
%ŸVp
                AND.L   #DATA32,-(A4)
%ŸVp
                AND.L   #DATA32,OFFSET2(A4,A6)
%ŸVp
                AND.L   #DATA32,OFFSET2(A4,D6)
%ŸVp
                AND.L   #DATA32,ADDRESS1
%ŸVp


ANDI.L:         ANDI.L  #10,D4
                ANDI.L  #DATA32,D4
%ŸVp
                ANDI.L  #DATA32,(A4)
%ŸVp
                ANDI.L  #DATA32,-(A4)
%ŸVp
                ANDI.L  #DATA32,OFFSET2(A4,A6)
%ŸVp
                ANDI.L  #DATA32,OFFSET2(A4,D6)
%ŸVp
                ANDI.L  #DATA32,ADDRESS1
%ŸVp


ASL:            ASL     D4,D6
                ASL     D6,D4
                ASL     #8,D6
%ŸVp
                ASL     (A4)
%ŸVp
                ASL     -(A4)
%ŸVp
                ASL     OFFSET2(A4,A5)
%ŸVp
                ASL     OFFSET2(A4,D5)
%ŸVp
                ASL     ADDRESS1
%ŸVp

ASL.B:          ASL.B   D6,D4
                ASL.B   D6,D4
%ŸVp
                ASL.B   #8,D4

%ŸVp
                ASL.W   D6,D4
%ŸVp
                ASL.W   #8,D4
%ŸVp
                ASL.W   (A4)+
%ŸVp
                ASL.W   ADDRESS2(A4)
%ŸVp
                ASL.W   OFFSET2(A4,A5.L)
                ASL.W   OFFSET2(A4,D5)
%ŸVp
                ASL.W   ADDRESS1
%ŸVp

ASL.L:          ASL.L   D4,D6
                ASL.L   D6,D4
%ŸVp
                ASL.L   #8,D4

%ŸVp
                ASR     D6,D4
%ŸVp
                ASR     #1,D6
%ŸVp
                ASR     (A4)+
%ŸVp
                ASR     ADDRESS2(A4)
%ŸVp
                ASR     OFFSET2(A4,A5.L)
%ŸVp
                ASR     OFFSET2(A4,D5.L)
%ŸVp
                ASR     ADDRESS2

%ŸVp
                ASR.B   D6,D4
%ŸVp
                ASR.B   #8,D4

%ŸVp
                ASR.W   D6,D4
%ŸVp
                ASR.W   #8,D4
%ŸVp
                ASR.W   (A4)+
%ŸVp
                ASR.W   ADDRESS2(A4)
%ŸVp
                ASR.W   OFFSET2(A4,A5.L)
                ASR.W   OFFSET2(A4,D5)
%ŸVp
                ASR.W   ADDRESS1
%ŸVp

ASR.L:          ASR.L   D4,D6
%ŸVp
                ASR.L   #1,D6
%ŸVp


BCHG:           BCHG    D6,D4
%ŸVp
                BCHG    D6,(A4)+
%ŸVp
                BCHG    D6,ADDRESS2(A4)
%ŸVp
                BCHG    D6,OFFSET2(A4,A5.L)
%ŸVp
                BCHG    D6,OFFSET2(A4,D5.L)
                BCHG    D6,ADDRESS1
%ŸVp

                BCHG    #0,D4
%ŸVp
                BCHG    #1,(A4)
%ŸVp
                BCHG    #3,-(A4)
%ŸVp
                BCHG    #5,OFFSET2(A4,A5)
%ŸVp
                BCHG    #7,OFFSET2(A4,D5)
%ŸVp
                BCHG    #7,ADDRESS1
                BCHG    #7,ADDRESS2

%ŸVp
                BCHG.B  #2,(A4)+
%ŸVp
                BCHG.B  #4,ADDRESS2(A4)
%ŸVp
                BCHG.B  #6,OFFSET2(A4,A5.L)
%ŸVp
                BCHG.B  #7,OFFSET2(A4,D5.L)
                BCHG.B  #7,ADDRESS1
%ŸVp

BCHG.L:         BCHG.L  D6,D4


%ŸVp
                BCLR    D6,(A4)
%ŸVp
                BCLR    D6,-(A4)
%ŸVp
                BCLR    D6,OFFSET2(A4,A5)
%ŸVp
                BCLR    D6,OFFSET2(A4,D5)
%ŸVp
                BCLR    D6,ADDRESS1
                BCLR    D6,ADDRESS2

%ŸVp
                BCLR    #31,D4
%ŸVp
                BCLR    #2,(A4)+
%ŸVp
                BCLR    #4,ADDRESS2(A4)
%ŸVp
                BCLR    #6,OFFSET2(A4,A5.L)
%ŸVp
                BCLR    #7,OFFSET2(A4,D5.L)
                BCLR    #7,ADDRESS1
%ŸVp

                BCLR.B  #1,(A4)
%ŸVp
                BCLR.B  #3,-(A4)
%ŸVp
                BCLR.B  #5,OFFSET2(A4,A5)
%ŸVp
                BCLR.B  #7,OFFSET2(A4,D5)
%ŸVp
                BCLR.B  #7,ADDRESS1
                BCLR.B  #7,ADDRESS2

%ŸVp


%ŸVp
                BSET    D6,(A4)
%ŸVp
                BSET    D6,-(A4)
%ŸVp
                BSET    D6,OFFSET2(A4,A5)
%ŸVp
                BSET    D6,OFFSET2(A4,D5)
                BSET    D6,OFFSET2(A4,D5.L)
                BSET    D6,ADDRESS1
%ŸVp

                BSET    #0,D4
                BSET    #31,D4
%ŸVp
                BSET    #2,(A4)+
%ŸVp
                BSET    #4,ADDRESS2(A4)
%ŸVp
                BSET    #6,OFFSET2(A4,A5.L)
%ŸVp
                BSET    #7,OFFSET2(A4,D5.L)
                BSET    #7,ADDRESS1
%ŸVp

                BSET.B  #1,(A4)
                BSET.B  #2,(A4)+
%ŸVp
                BSET.B  #4,ADDRESS2(A4)
%ŸVp
                BSET.B  #6,OFFSET2(A4,A5.L)
%ŸVp
                BSET.B  #7,OFFSET2(A4,D5.L)
                BSET.B  #7,ADDRESS1
%ŸVp

BSET.L:         BSET.L  D6,D4


%ŸVp
                BTST    D6,(A4)
%ŸVp
                BTST    D6,-(A4)
%ŸVp
                BTST    D6,OFFSET2(A4,A5)
%ŸVp
                BTST    D6,OFFSET2(A4,D5)
%ŸVp
                BTST    D6,ADDRESS1
                BTST    D6,ADDRESS2

                BTST    #0,D4
                BTST    #31,D4
%ŸVp
                BTST    #2,(A4)+
%ŸVp
                BTST    #4,ADDRESS2(A4)
%ŸVp
                BTST    #6,OFFSET2(A4,A5.L)
%ŸVp
                BTST    #7,OFFSET2(A4,D5.L)
                BTST    #7,ADDRESS1
%ŸVp

                BTST.B  #1,(A4)
%ŸVp
                BTST.B  #3,-(A4)
%ŸVp
                BTST.B  #5,OFFSET2(A4,A5)
%ŸVp
                BTST.B  #7,OFFSET2(A4,D5)
%ŸVp
                BTST.B  #7,ADDRESS1
                BTST.B  #7,ADDRESS2

%ŸVp


%ŸVp
                BRA     CLR
%ŸVp

BHI:            BHI     BRA
%ŸVp
                BHI.S   BCC

%ŸVp
                BLS     CLR
%ŸVp

BCC:            BCC     BRA
%ŸVp
                BCC.S   BNE

%ŸVp
                BCS     CLR
%ŸVp

BNE:            BNE     BRA
%ŸVp
                BNE.S   BVC

%ŸVp
                BEQ     CLR
%ŸVp

BVC:            BVC     BRA
%ŸVp
                BVC.S   BPL

%ŸVp
                BVS     CLR
%ŸVp

BPL:            BPL     BRA
%ŸVp
                BPL.S   BGE

%ŸVp
                BMI     CLR
%ŸVp

BGE:            BGE     BRA
%ŸVp
                BGE.S   BGT

%ŸVp
                BLT     CLR
%ŸVp

BGT:            BGT     BRA
%ŸVp
                BGT.S   BSR

%ŸVp
                BLE     CLR
%ŸVp

BSR:            BSR     BRA
%ŸVp
                BSR.S   BSR


%ŸVp
                CHK     (A4),D6
%ŸVp
                CHK     -(A4),D6
%ŸVp
                CHK     OFFSET2(A4,A5),D6
%ŸVp
                CHK     OFFSET2(A4,D5),D6
%ŸVp
                CHK     ADDRESS1,D6
%ŸVp
                CHK     OFFSET1(PC,A5),D6
%ŸVp
                CHK     OFFSET1(PC,D5),D6
%ŸVp
                CHK     #1,D6
%ŸVp


CLR:            CLR     D4
%ŸVp
                CLR     (A4)
%ŸVp
                CLR     -(A4)
%ŸVp
                CLR     OFFSET2(A4,A5)
%ŸVp
                CLR     OFFSET2(A4,D5)
%ŸVp
                CLR     ADDRESS1
%ŸVp

CLR.B:          CLR.B   D6
%ŸVp
                CLR.B   (A4)
%ŸVp
                CLR.B   -(A4)
%ŸVp
                CLR.B   OFFSET2(A4,A5)
%ŸVp
                CLR.B   OFFSET2(A4,D5)
%ŸVp
                CLR.B   ADDRESS1
%ŸVp

CLR.W:          CLR.W   D4
%ŸVp
                CLR.W   (A4)
%ŸVp
                CLR.W   -(A4)
                CLR.W   ADDRESS2(A4)
%ŸVp
                CLR.W   OFFSET2(A4,A5.L)
                CLR.W   OFFSET2(A4,D5)
%ŸVp
                CLR.W   ADDRESS1
%ŸVp

CLR.L:          CLR.L   D4
                CLR.L   D6
%ŸVp
                CLR.L   (A4)+
%ŸVp
                CLR.L   ADDRESS2(A4)
%ŸVp
                CLR.L   OFFSET2(A4,A5.L)
                CLR.L   OFFSET2(A4,D5)
%ŸVp
                CLR.L   ADDRESS1
%ŸVp


CMP:            CMP     D4,D6
%ŸVp
                CMP     (A4),D6
%ŸVp
                CMP     -(A4),D6
%ŸVp
                CMP     OFFSET2(A4,A5),D6
%ŸVp
                CMP     OFFSET2(A4,D5),D6
%ŸVp
                CMP     ADDRESS1,D6
%ŸVp
                CMP     OFFSET1(PC),D6
%ŸVp
                CMP     OFFSET1(PC,A5.L),D6
%ŸVp
                CMP     OFFSET1(PC,D5.L),D6
                CMP     #10,D6
%ŸVp

                CMP     D4,A6
%ŸVp
                CMP     (A4),A6
%ŸVp
                CMP     -(A4),A6
%ŸVp
                CMP     OFFSET2(A4,A6),A6
%ŸVp
                CMP     OFFSET2(A4,D6),A6
%ŸVp
                CMP     ADDRESS1,A6
%ŸVp
                CMP     OFFSET1(PC),A6
%ŸVp
                CMP     OFFSET1(PC,A6.L),A6
%ŸVp
                CMP     OFFSET1(PC,D6.L),A6
                CMP     #10,A6
%ŸVp

                CMP     #10,D4
%ŸVp
                CMP     #10,(A4)
%ŸVp
                CMP     #DATA16,(A4)+
%ŸVp
                CMP     #DATA16,ADDRESS2(A4)
%ŸVp
                CMP     #DATA16,OFFSET2(A4,A6.L)
%ŸVp
                CMP     #DATA16,OFFSET2(A4,D6.L)
%ŸVp
                CMP     #DATA16,ADDRESS2

%ŸVp
                CMPA    A4,A6
%ŸVp
                CMPA    (A4)+,A6
%ŸVp
                CMPA    ADDRESS2(A4),A6
%ŸVp
                CMPA    OFFSET2(A4,A6.L),A6
%ŸVp
                CMPA    OFFSET2(A4,D6.L),A6
                CMPA    ADDRESS1,A6
%ŸVp
                CMPA    OFFSET1(PC),A6
%ŸVp
                CMPA    OFFSET1(PC,A6.L),A6
%ŸVp
                CMPA    OFFSET1(PC,D6.L),A6
                CMPA    #10,A6
%ŸVp

CMPI:           CMPI    #10,D4
%ŸVp
                CMPI    #10,(A4)
%ŸVp
                CMPI    #DATA16,(A4)+
%ŸVp
                CMPI    #DATA16,ADDRESS2(A4)
%ŸVp
                CMPI    #DATA16,OFFSET2(A4,A6.L)
%ŸVp
                CMPI    #DATA16,OFFSET2(A4,D6.L)
%ŸVp
                CMPI    #DATA16,ADDRESS2

%ŸVp
                CMP.W   A4,D6
%ŸVp
                CMP.W   (A4)+,D6
%ŸVp
                CMP.W   ADDRESS2(A4),D6
%ŸVp
                CMP.W   OFFSET2(A4,A5.L),D6
%ŸVp
                CMP.W   OFFSET2(A4,D5.L),D6
                CMP.W   ADDRESS1,D6
%ŸVp
                CMP.W   OFFSET1(PC),D6
%ŸVp
                CMP.W   OFFSET1(PC,A5.L),D6
%ŸVp
                CMP.W   OFFSET1(PC,D5.L),D6
                CMP.W   #10,D6
%ŸVp

                CMP.W   D4,A6
%ŸVp
                CMP.W   (A4),A6
%ŸVp
                CMP.W   -(A4),A6
%ŸVp
                CMP.W   OFFSET2(A4,A6),A6
%ŸVp
                CMP.W   OFFSET2(A4,D6),A6
%ŸVp
                CMP.W   ADDRESS1,A6
                CMP.W   ADDRESS2,A6
%ŸVp
                CMP.W   OFFSET1(PC,A6),A6
%ŸVp
                CMP.W   OFFSET1(PC,D6),A6
                CMP.W   OFFSET1(PC,D6.L),A6
                CMP.W   #10,A6
                CMP.W   #DATA16,A6

                CMP.W   #10,D4
                CMP.W   #DATA16,D4
%ŸVp
                CMP.W   #DATA16,(A4)
%ŸVp
                CMP.W   #DATA16,-(A4)
%ŸVp
                CMP.W   #DATA16,OFFSET2(A4,A6)
%ŸVp
                CMP.W   #DATA16,OFFSET2(A4,D6)
%ŸVp
                CMP.W   #DATA16,ADDRESS1
%ŸVp

CMPA.W:         CMPA.W  D4,A6
                CMPA.W  A4,A6
%ŸVp
                CMPA.W  (A4)+,A6
%ŸVp
%ŸVp
                CMPA.W  OFFSET2(A4,A6),A6
%ŸVp
                CMPA.W  OFFSET2(A4,D6),A6
%ŸVp
                CMPA.W  ADDRESS1,A6
                CMPA.W  ADDRESS2,A6
%ŸVp
                CMPA.W  OFFSET1(PC,A6),A6
%ŸVp
                CMPA.W  OFFSET1(PC,D6),A6
%ŸVp
                CMPA.W  #10,A6
                CMPA.W  #DATA16,A6

CMPI.W:         CMPI.W  #10,D4
%ŸVp
                CMPI.W  #10,(A4)
%ŸVp
                CMPI.W  #DATA16,(A4)+
%ŸVp
                CMPI.W  #DATA16,ADDRESS2(A4)
%ŸVp
                CMPI.W  #DATA16,OFFSET2(A4,A6.L)
%ŸVp
                CMPI.W  #DATA16,OFFSET2(A4,D6.L)
%ŸVp
                CMPI.W  #DATA16,ADDRESS2

%ŸVp
                CMP.B   (A4),D6
%ŸVp
                CMP.B   -(A4),D6
%ŸVp
                CMP.B   OFFSET2(A4,A5),D6
%ŸVp
                CMP.B   OFFSET2(A4,D5),D6
%ŸVp
                CMP.B   ADDRESS1,D6
                CMP.B   ADDRESS2,D6
%ŸVp
                CMP.B   OFFSET1(PC,A5),D6
%ŸVp
                CMP.B   OFFSET1(PC,D5),D6
%ŸVp
                CMP.B   #10,D6
                CMP.B   #DATA8,D6

%ŸVp
                CMP.B   #DATA8,D4
%ŸVp
                CMP.B   #DATA8,(A4)
%ŸVp
                CMP.B   #DATA8,-(A4)
%ŸVp
                CMP.B   #DATA8,OFFSET2(A4,A6)
%ŸVp
                CMP.B   #DATA8,OFFSET2(A4,D6)
%ŸVp
                CMP.B   #DATA8,ADDRESS1
                CMP.B   #DATA8,ADDRESS2

%ŸVp
                CMPI.B  #DATA8,D4
%ŸVp
                CMPI.B  #DATA8,(A4)
%ŸVp
                CMPI.B  #DATA8,-(A4)
%ŸVp
                CMPI.B  #DATA8,OFFSET2(A4,A6)
%ŸVp
                CMPI.B  #DATA8,OFFSET2(A4,D6)
%ŸVp
                CMPI.B  #DATA8,ADDRESS1
                CMPI.B  #DATA8,ADDRESS2

CMP.L:          CMP.L   D4,D6
                CMP.L   A4,D6
%ŸVp
                CMP.L   (A4)+,D6
%ŸVp
                CMP.L   ADDRESS2(A4),D6
%ŸVp
                CMP.L   OFFSET2(A4,A5.L),D6
%ŸVp
                CMP.L   OFFSET2(A4,D5.L),D6
                CMP.L   ADDRESS1,D6
%ŸVp
                CMP.L   OFFSET1(PC),D6
%ŸVp
                CMP.L   OFFSET1(PC,A5.L),D6
%ŸVp
                CMP.L   OFFSET1(PC,D5.L),D6
                CMP.L   #10,D6
%ŸVp

                CMP.L   D4,A6
%ŸVp
                CMP.L   (A4),A6
%ŸVp
                CMP.L   -(A4),A6
%ŸVp
                CMP.L   OFFSET2(A4,A6),A6
%ŸVp
                CMP.L   OFFSET2(A4,D6),A6
%ŸVp
                CMP.L   ADDRESS1,A6
                CMP.L   ADDRESS2,A6
%ŸVp
                CMP.L   OFFSET1(PC,A6),A6
%ŸVp
                CMP.L   OFFSET1(PC,D6),A6
%ŸVp
                CMP.L   #10,A6
                CMP.L   #DATA32,A6

%ŸVp
                CMP.L   #DATA32,D4
%ŸVp
                CMP.L   #DATA32,(A4)
%ŸVp
                CMP.L   #DATA32,-(A4)
%ŸVp
                CMP.L   #DATA32,OFFSET2(A4,A6)
%ŸVp
                CMP.L   #DATA32,OFFSET2(A4,D6)
%ŸVp
                CMP.L   #DATA32,ADDRESS1
%ŸVp

CMPA.L:         CMPA.L  D4,A6
                CMPA.L  A4,A6
%ŸVp
                CMPA.L  (A4)+,A6
%ŸVp
                CMPA.L  ADDRESS2(A4),A6
%ŸVp
%ŸVp
                CMPA.L  OFFSET2(A4,D6),A6
%ŸVp
                CMPA.L  ADDRESS1,A6
                CMPA.L  ADDRESS2,A6
                CMPA.L  OFFSET1(PC),A6
                CMPA.L  OFFSET1(PC,A6),A6
%ŸVp
                CMPA.L  OFFSET1(PC,D6),A6
%ŸVp
                CMPA.L  #10,A6
                CMPA.L  #DATA32,A6

CMPI.L:         CMPI.L  #10,D4
                CMPI.L  #DATA32,D4
%ŸVp
                CMPI.L  #DATA32,(A4)
%ŸVp
                CMPI.L  #DATA32,-(A4)
%ŸVp
                CMPI.L  #DATA32,OFFSET2(A4,A6)
%ŸVp
                CMPI.L  #DATA32,OFFSET2(A4,D6)
%ŸVp
                CMPI.L  #DATA32,ADDRESS1
%ŸVp


CMPM:           CMPM    (A4)+,(A6)+
                CMPM.W  (A4)+,(A6)+
%ŸVp
                CMPM.L  (A4)+,(A6)+


%ŸVp
                DBT     D4,10
%ŸVp

DBF:            DBF     D4,DBT
%ŸVp
                DBF     D4,EOR

%ŸVp
                DBHI    D4,10
%ŸVp

DBLS:           DBLS    D4,DBT
%ŸVp
                DBLS    D4,EOR

%ŸVp
                DBCC    D4,10
%ŸVp

DBCS:           DBCS    D4,DBT
%ŸVp
                DBCS    D4,EOR

%ŸVp
                DBNE    D4,10
%ŸVp

DBEQ:           DBEQ    D4,DBT
%ŸVp
                DBEQ    D4,EOR

%ŸVp
                DBVC    D4,10
%ŸVp

DBVS:           DBVS    D4,DBT
%ŸVp
                DBVS    D4,EOR

%ŸVp
                DBPL    D4,10
%ŸVp

DBMI:           DBMI    D4,DBT
%ŸVp
                DBMI    D4,EOR

%ŸVp
                DBGE    D4,10
%ŸVp

DBLT:           DBLT    D4,DBT
%ŸVp
                DBLT    D4,EOR

%ŸVp
                DBGT    D4,10
%ŸVp

DBLE:           DBLE    D4,DBT
%ŸVp
                DBLE    D4,EOR


%ŸVp
                DIVS    (A4),D6
%ŸVp
                DIVS    -(A4),D6
%ŸVp
                DIVS    OFFSET2(A4,A5),D6
%ŸVp
                DIVS    OFFSET2(A4,D5),D6
%ŸVp
                DIVS    ADDRESS1,D6
                DIVS    ADDRESS2,D6
%ŸVp
                DIVS    OFFSET1(PC,A5),D6
%ŸVp
                DIVS    OFFSET1(PC,D5),D6
%ŸVp
                DIVS    #DATA16,D6


DIVU:           DIVU    D4,D6
%ŸVp
                DIVU    (A4)+,D6
%ŸVp
                DIVU    ADDRESS2(A4),D6
%ŸVp
                DIVU    OFFSET2(A4,A5.L),D6
%ŸVp
                DIVU    OFFSET2(A4,D5.L),D6
                DIVU    ADDRESS1,D6
%ŸVp
                DIVU    OFFSET1(PC),D6
%ŸVp
                DIVU    OFFSET1(PC,A5.L),D6
%ŸVp
                DIVU    OFFSET1(PC,D5.L),D6
                DIVU    #DATA16,D6


%ŸVp
                EOR     #10,D6

%ŸVp
                EOR     D4,(A6)
%ŸVp
                EOR     D4,-(A6)
%ŸVp
                EOR     D4,OFFSET1(A6,A4)
%ŸVp
                EOR     D4,OFFSET1(A6,D4)
%ŸVp
                EOR     D4,ADDRESS1
%ŸVp

                EOR     #10,D4

%ŸVp
                EORI    #ADDRESS1,D4
%ŸVp
                EORI    #ADDRESS1,(A4)
%ŸVp
                EORI    #ADDRESS1,-(A4)
%ŸVp
                EORI    #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                EORI    #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                EORI    #ADDRESS1,ADDRESS1
%ŸVp

EOR.W:          EOR.W   D4,D6
                EOR.W   #10,D6

%ŸVp
                EOR.W   D4,(A6)
%ŸVp
                EOR.W   D4,-(A6)
%ŸVp
                EOR.W   D4,OFFSET1(A6,A4)
%ŸVp
                EOR.W   D4,OFFSET1(A6,D4)
%ŸVp
                EOR.W   D4,ADDRESS1
                EOR.W   D4,ADDRESS2

%ŸVp
                EOR.W   #ADDRESS1,D4
%ŸVp
                EOR.W   #ADDRESS1,(A4)
%ŸVp
                EOR.W   #ADDRESS1,-(A4)
%ŸVp
                EOR.W   #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                EOR.W   #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                EOR.W   #ADDRESS1,ADDRESS1
%ŸVp

EORI.W:         EORI.W  #10,D4
                EORI.W  #ADDRESS1,D4
                EORI.W  #10,(A4)
%ŸVp
                EORI.W  #ADDRESS1,(A4)+
                EORI.W  #ADDRESS1,-(A4)
%ŸVp
                EORI.W  #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                EORI.W  #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                EORI.W  #ADDRESS1,ADDRESS1
%ŸVp

EOR.B:          EOR.B   D4,D6
                EOR.B   #10,D6
%ŸVp

                EOR.B   D4,D6
%ŸVp
                EOR.B   D4,(A6)+
%ŸVp
                EOR.B   D4,ADDRESS2(A6)
%ŸVp
                EOR.B   D4,OFFSET1(A6,A4.L)
%ŸVp
                EOR.B   D4,OFFSET1(A6,D4.L)
                EOR.B   D4,ADDRESS1
%ŸVp

                EOR.B   #10,D4
%ŸVp
                EOR.B   #10,(A4)
%ŸVp
                EOR.B   #DATA8,(A4)+
%ŸVp
                EOR.B   #DATA8,ADDRESS2(A4)
%ŸVp
                EOR.B   #DATA8,OFFSET2(A4,A6.L)
%ŸVp
                EOR.B   #DATA8,OFFSET2(A4,D6.L)
                EOR.B   #DATA8,ADDRESS1
%ŸVp

EORI.B:         EORI.B  #10,D4
%ŸVp
                EORI.B  #10,(A4)
%ŸVp
                EORI.B  #DATA8,(A4)+
%ŸVp
                EORI.B  #DATA8,ADDRESS2(A4)
%ŸVp
                EORI.B  #DATA8,OFFSET2(A4,A6.L)
%ŸVp
                EORI.B  #DATA8,OFFSET2(A4,D6.L)
                EORI.B  #DATA8,ADDRESS1

%ŸVp
                EOR.L   #10,D6
%ŸVp

                EOR.L   D4,D6
%ŸVp
                EOR.L   D4,(A6)+
%ŸVp
                EOR.L   D4,ADDRESS2(A6)
%ŸVp
                EOR.L   D4,OFFSET1(A6,A4.L)
%ŸVp
                EOR.L   D4,OFFSET1(A6,D4.L)
                EOR.L   D4,ADDRESS1
%ŸVp

                EOR.L   #10,D4
%ŸVp
                EOR.L   #10,(A4)
%ŸVp
                EOR.L   #DATA32,(A4)+
%ŸVp
                EOR.L   #DATA32,ADDRESS2(A4)
%ŸVp
                EOR.L   #DATA32,OFFSET2(A4,A6.L)
%ŸVp
                EOR.L   #DATA32,OFFSET2(A4,D6.L)
%ŸVp
                EOR.L   #DATA32,ADDRESS2

%ŸVp
                EORI.L  #DATA32,D4
%ŸVp
                EORI.L  #DATA32,(A4)
%ŸVp
                EORI.L  #DATA32,-(A4)
%ŸVp
                EORI.L  #DATA32,OFFSET2(A4,A6)
%ŸVp
                EORI.L  #DATA32,OFFSET2(A4,D6)
%ŸVp
                EORI.L  #DATA32,ADDRESS1
%ŸVp


EXG:            EXG     D0,D7
                EXG     D7,D0
%ŸVp
                EXG     A7,A0
%ŸVp
                EXG     D0,A7
%ŸVp
                EXG     D7,A7
%ŸVp
                EXG     A0,D7
%ŸVp
                EXG     A7,D7


%ŸVp
                EXT     D7

%ŸVp
                EXT.W   D7

%ŸVp
                EXT.L   D7

%ŸVp

%ŸVp
                JMP     ADDRESS2(A4)
%ŸVp
                JMP     OFFSET2(A4,A5.L)
%ŸVp
                JMP     OFFSET2(A4,D5.L)
%ŸVp
                JMP     ADDRESS2
%ŸVp
                JMP     OFFSET1(PC,A5)
%ŸVp
                JMP     OFFSET1(PC,D5)
%ŸVp


JSR:            JSR     (A4)
%ŸVp
                JSR     OFFSET2(A4,A5)
%ŸVp
                JSR     OFFSET2(A4,D5)
%ŸVp
                JSR     ADDRESS1
%ŸVp
                JSR     OFFSET1(PC)
%ŸVp
                JSR     OFFSET1(PC,A5.L)
%ŸVp
                JSR     OFFSET1(PC,D5.L)


%ŸVp
LEA:            LEA     (A4),A6
%ŸVp
                LEA     OFFSET2(A4,A5),A6
%ŸVp
                LEA     OFFSET2(A4,D5),A6
%ŸVp
                LEA     ADDRESS1,A6
%ŸVp
                LEA     ADDRESS2(PC),A6
%ŸVp
                LEA     OFFSET1(PC,A5),A6
%ŸVp
                LEA     OFFSET1(PC,D5),A6
%ŸVp
                list    off


%ŸVp
                LINK    A7,#0
%ŸVp
                LINK    A7,#DATA16


%ŸVp
                LSL     D6,D4
%ŸVp
                LSL     #1,D6
%ŸVp
                LSL     (A4)+
%ŸVp
                LSL     ADDRESS2(A4)
%ŸVp
                LSL     OFFSET2(A4,A5.L)
%ŸVp
                LSL     OFFSET2(A4,D5.L)
%ŸVp
                LSL     ADDRESS2

%ŸVp
                LSL.B   D6,D4
%ŸVp
                LSL.B   #8,D4

%ŸVp
                LSL.W   D6,D4
%ŸVp
                LSL.W   #8,D4
%ŸVp
                LSL.W   (A4)+
%ŸVp
                LSL.W   ADDRESS2(A4)
%ŸVp
                LSL.W   OFFSET2(A4,A5.L)
                LSL.W   OFFSET2(A4,D5)
%ŸVp
                LSL.W   ADDRESS1
%ŸVp

LSL.L:          LSL.L   D4,D6
%ŸVp
                LSL.L   #1,D6
%ŸVp


LSR:            LSR     D4,D6
%ŸVp
                LSR     #8,D6
%ŸVp
                LSR     (A4)
%ŸVp
                LSR     -(A4)
%ŸVp
                LSR     OFFSET2(A4,A5)
%ŸVp
                LSR     OFFSET2(A4,D5)
%ŸVp
                LSR     ADDRESS1
%ŸVp

LSR.B:          LSR.B   D6,D4
%ŸVp
                LSR.B   #1,D6
%ŸVp

LSR.W:          LSR.W   D4,D6
%ŸVp
                LSR.W   #1,D6
%ŸVp
                LSR.W   (A4)
%ŸVp
                LSR.W   -(A4)
%ŸVp
                LSR.W   OFFSET2(A4,A5)
%ŸVp
                LSR.W   OFFSET2(A4,D5)
%ŸVp
                LSR.W   ADDRESS1
%ŸVp

LSR.L:          LSR.L   D4,D6
%ŸVp
                LSR.L   #1,D6
%ŸVp


MOVE:           MOVE    D0,D7
%ŸVp
                MOVE    A0,D0
%ŸVp
                MOVE    A7,D0
%ŸVp
                MOVE    (A4),D6
%ŸVp
                MOVE    -(A4),D6
%ŸVp
                MOVE    OFFSET2(A4,A5),D6
%ŸVp
                MOVE    OFFSET2(A4,D5),D6
%ŸVp
                MOVE    ADDRESS1,D6
                MOVE    ADDRESS2,D6
%ŸVp
                MOVE    OFFSET1(PC,A5),D6
%ŸVp
                MOVE    OFFSET1(PC,D5),D6
%ŸVp
                MOVE    #10,D6
                MOVE    #DATA16,D6

%ŸVp
                MOVE    D7,A7
%ŸVp
                MOVE    A0,A7
%ŸVp
                MOVE    A7,A7
%ŸVp
                MOVE    (A4)+,A6
%ŸVp
                MOVE    ADDRESS2(A4),A6
%ŸVp
                MOVE    OFFSET2(A4,A5.L),A6
%ŸVp
                MOVE    OFFSET2(A4,D5.L),A6
                MOVE    ADDRESS1,A6
%ŸVp
                MOVE    OFFSET1(PC),A6
%ŸVp
                MOVE    OFFSET1(PC,A5.L),A6
%ŸVp
                MOVE    OFFSET1(PC,D5.L),A6
                MOVE    #10,A6
%ŸVp

                MOVE    D4,(A6)
%ŸVp
                MOVE    (A4)+,(A6)
%ŸVp
                MOVE    ADDRESS2(A4),(A6)
%ŸVp
                MOVE    OFFSET2(A4,A5.L),(A6)
%ŸVp
                MOVE    OFFSET2(A4,D5.L),(A6)
                MOVE    ADDRESS1,(A6)
%ŸVp
                MOVE    OFFSET1(PC),(A6)
%ŸVp
                MOVE    OFFSET1(PC,A5.L),(A6)
%ŸVp
                MOVE    OFFSET1(PC,D5.L),(A6)
                MOVE    #10,(A6)
%ŸVp

                MOVE    D4,(A6)+
%ŸVp
                MOVE    (A4)+,(A6)+
%ŸVp
                MOVE    ADDRESS2(A4),(A6)+
%ŸVp
                MOVE    OFFSET2(A4,A5.L),(A6)+
%ŸVp
                MOVE    OFFSET2(A4,D5.L),(A6)+
                MOVE    ADDRESS1,(A6)+
%ŸVp
                MOVE    OFFSET1(PC),(A6)+
%ŸVp
                MOVE    OFFSET1(PC,A5.L),(A6)+
%ŸVp
                MOVE    OFFSET1(PC,D5.L),(A6)+
                MOVE    #10,(A6)+
%ŸVp

                MOVE    D4,-(A6)
%ŸVp
                MOVE    (A4)+,-(A6)
%ŸVp
                MOVE    ADDRESS2(A4),-(A6)
%ŸVp
                MOVE    OFFSET2(A4,A5.L),-(A6)
%ŸVp
                MOVE    OFFSET2(A4,D5.L),-(A6)
                MOVE    ADDRESS1,-(A6)
%ŸVp
                MOVE    OFFSET1(PC),-(A6)
%ŸVp
                MOVE    OFFSET1(PC,A5.L),-(A6)
%ŸVp
                MOVE    OFFSET1(PC,D5.L),-(A6)
                MOVE    #10,-(A6)
%ŸVp

                MOVE    D4,ADDRESS2(A6)
%ŸVp
                MOVE    (A4)+,ADDRESS2(A6)
%ŸVp
                MOVE    ADDRESS2(A4),ADDRESS2(A6)
%ŸVp
                MOVE    OFFSET2(A4,A5.L),ADDRESS2(A6)
%ŸVp
                MOVE    OFFSET2(A4,D5.L),ADDRESS2(A6)
%ŸVp
%ŸVp
                MOVE    OFFSET1(PC),ADDRESS2(A6)
%ŸVp
                MOVE    OFFSET1(PC,A5.L),ADDRESS2(A6)
%ŸVp
                MOVE    OFFSET1(PC,D5.L),ADDRESS2(A6)
%ŸVp
                MOVE    #DATA16,ADDRESS2(A6)

                MOVE    D4,OFFSET1(A6,A7)
%ŸVp
                MOVE    (A4)+,OFFSET1(A6,A7)
%ŸVp
                MOVE    ADDRESS2(A4),OFFSET1(A6,A7)
%ŸVp
                MOVE    OFFSET2(A4,A5.L),OFFSET1(A6,A7)
%ŸVp
                MOVE    OFFSET2(A4,D5.L),OFFSET1(A6,A7)
%ŸVp
                MOVE    ADDRESS2,OFFSET1(A6,A7)
%ŸVp
                MOVE    OFFSET1(PC,A5),OFFSET1(A6,A7)
%ŸVp
                MOVE    OFFSET1(PC,D5),OFFSET1(A6,A7)
%ŸVp
                MOVE    #10,OFFSET1(A6,A7)
%ŸVp

                MOVE    D4,OFFSET1(A6,A7.L)
%ŸVp
                MOVE    (A4)+,OFFSET1(A6,A7.L)
%ŸVp
                MOVE    ADDRESS2(A4),OFFSET1(A6,A7.L)
%ŸVp
                MOVE    OFFSET2(A4,A5.L),OFFSET1(A6,A7.L)
                MOVE    OFFSET2(A4,D5),OFFSET1(A6,A7.L)
%ŸVp
                MOVE    ADDRESS1,OFFSET1(A6,A7.L)
                MOVE    ADDRESS2,OFFSET1(A6,A7.L)
%ŸVp
                MOVE    OFFSET1(PC,A5),OFFSET1(A6,A7.L)
%ŸVp
                MOVE    OFFSET1(PC,D5),OFFSET1(A6,A7.L)
                MOVE    OFFSET1(PC,D5.L),OFFSET1(A6,A7.L)
                MOVE    #10,OFFSET1(A6,A7.L)
%ŸVp

                MOVE    D4,OFFSET1(A6,D7)
%ŸVp
                MOVE    (A4)+,OFFSET1(A6,D7)
%ŸVp
                MOVE    ADDRESS2(A4),OFFSET1(A6,D7)
%ŸVp
                MOVE    OFFSET2(A4,A5.L),OFFSET1(A6,D7)
%ŸVp
                MOVE    OFFSET2(A4,D5.L),OFFSET1(A6,D7)
%ŸVp
                MOVE    ADDRESS2,OFFSET1(A6,D7)
%ŸVp
                MOVE    OFFSET1(PC,A5),OFFSET1(A6,D7)
%ŸVp
                MOVE    OFFSET1(PC,D5),OFFSET1(A6,D7)
%ŸVp
                MOVE    #10,OFFSET1(A6,D7)
%ŸVp

                MOVE    D4,OFFSET1(A6,D7.L)
%ŸVp
                MOVE    (A4)+,OFFSET1(A6,D7.L)
%ŸVp
                MOVE    ADDRESS2(A4),OFFSET1(A6,D7.L)
%ŸVp
                MOVE    OFFSET2(A4,A5.L),OFFSET1(A6,D7.L)
                MOVE    OFFSET2(A4,D5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE    ADDRESS1,OFFSET1(A6,D7.L)
                MOVE    ADDRESS2,OFFSET1(A6,D7.L)
%ŸVp
                MOVE    OFFSET1(PC,A5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE    OFFSET1(PC,D5),OFFSET1(A6,D7.L)
                MOVE    OFFSET1(PC,D5.L),OFFSET1(A6,D7.L)
                MOVE    #10,OFFSET1(A6,D7.L)
%ŸVp

                MOVE    D4,ADDRESS1
                MOVE    (A4),ADDRESS1
%ŸVp
                MOVE    -(A4),ADDRESS1
%ŸVp
                MOVE    OFFSET2(A4,A5),ADDRESS1
%ŸVp
                MOVE    OFFSET2(A4,D5),ADDRESS1
%ŸVp
                MOVE    ADDRESS1,ADDRESS1
%ŸVp
                MOVE    OFFSET1(PC),ADDRESS1
%ŸVp
                MOVE    OFFSET1(PC,A5.L),ADDRESS1
%ŸVp
                MOVE    OFFSET1(PC,D5.L),ADDRESS1
                MOVE    #10,ADDRESS1
%ŸVp

                MOVE    D4,ADDRESS2
%ŸVp
                MOVE    (A4)+,ADDRESS2
%ŸVp
                MOVE    ADDRESS2(A4),ADDRESS2
%ŸVp
                MOVE    OFFSET2(A4,A5.L),ADDRESS2
%ŸVp
                MOVE    OFFSET2(A4,D5.L),ADDRESS2
%ŸVp
                MOVE    ADDRESS2,ADDRESS2
%ŸVp
                MOVE    OFFSET1(PC,A5),ADDRESS2
%ŸVp
                MOVE    OFFSET1(PC,D5),ADDRESS2
%ŸVp
                MOVE    #10,ADDRESS2
                MOVE    #DATA16,ADDRESS2


%ŸVp
                MOVE.W  D7,D0
%ŸVp
                MOVE.W  A0,D7
%ŸVp
                MOVE.W  A7,D7
%ŸVp
                MOVE.W  (A4)+,D6
%ŸVp
                MOVE.W  ADDRESS2(A4),D6
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),D6
%ŸVp
                MOVE.W  OFFSET2(A4,D5.L),D6
                MOVE.W  ADDRESS1,D6
%ŸVp
                MOVE.W  OFFSET1(PC),D6
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),D6
%ŸVp
                MOVE.W  OFFSET1(PC,D5.L),D6
                MOVE.W  #10,D6
%ŸVp

                MOVE.W  D0,A0
%ŸVp
                MOVE.W  A0,A0
%ŸVp
                MOVE.W  A7,A0
%ŸVp
                MOVE.W  (A4),A6
%ŸVp
                MOVE.W  -(A4),A6
%ŸVp
                MOVE.W  OFFSET2(A4,A5),A6
%ŸVp
                MOVE.W  OFFSET2(A4,D5),A6
%ŸVp
                MOVE.W  ADDRESS1,A6
                MOVE.W  ADDRESS2,A6
%ŸVp
                MOVE.W  OFFSET1(PC,A5),A6
%ŸVp
                MOVE.W  OFFSET1(PC,D5),A6
%ŸVp
                MOVE.W  #10,A6
                MOVE.W  #DATA16,A6

%ŸVp
                MOVE.W  (A4),(A6)
%ŸVp
                MOVE.W  -(A4),(A6)
                MOVE.W  ADDRESS2(A4),(A6)
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),(A6)
%ŸVp
                MOVE.W  OFFSET2(A4,D5.L),(A6)
                MOVE.W  ADDRESS1,(A6)
%ŸVp
                MOVE.W  OFFSET1(PC),(A6)
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),(A6)
%ŸVp
                MOVE.W  OFFSET1(PC,D5.L),(A6)
                MOVE.W  #10,(A6)
%ŸVp

                MOVE.W  D4,(A6)+
%ŸVp
                MOVE.W  (A4)+,(A6)+
%ŸVp
                MOVE.W  ADDRESS2(A4),(A6)+
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),(A6)+
%ŸVp
                MOVE.W  OFFSET2(A4,D5.L),(A6)+
                MOVE.W  ADDRESS1,(A6)+
%ŸVp
                MOVE.W  OFFSET1(PC),(A6)+
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),(A6)+
%ŸVp
                MOVE.W  OFFSET1(PC,D5.L),(A6)+
                MOVE.W  #10,(A6)+
%ŸVp

                MOVE.W  D4,-(A6)
%ŸVp
                MOVE.W  (A4)+,-(A6)
%ŸVp
                MOVE.W  ADDRESS2(A4),-(A6)
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),-(A6)
%ŸVp
                MOVE.W  OFFSET2(A4,D5.L),-(A6)
                MOVE.W  ADDRESS1,-(A6)
%ŸVp
                MOVE.W  OFFSET1(PC),-(A6)
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),-(A6)
%ŸVp
                MOVE.W  OFFSET1(PC,D5.L),-(A6)
                MOVE.W  #10,-(A6)
%ŸVp

                MOVE.W  D4,ADDRESS2(A6)
%ŸVp
                MOVE.W  (A4)+,ADDRESS2(A6)
%ŸVp
                MOVE.W  ADDRESS2(A4),ADDRESS2(A6)
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),ADDRESS2(A6)
%ŸVp
                MOVE.W  OFFSET2(A4,D5.L),ADDRESS2(A6)
%ŸVp
                MOVE.W  ADDRESS2,ADDRESS2(A6)
%ŸVp
                MOVE.W  OFFSET1(PC,A5),ADDRESS2(A6)
%ŸVp
                MOVE.W  OFFSET1(PC,D5),ADDRESS2(A6)
%ŸVp
                MOVE.W  #10,ADDRESS2(A6)
%ŸVp

                MOVE.W  D4,OFFSET1(A6,A7)
%ŸVp
                MOVE.W  (A4)+,OFFSET1(A6,A7)
%ŸVp
                MOVE.W  ADDRESS2(A4),OFFSET1(A6,A7)
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),OFFSET1(A6,A7)
                MOVE.W  OFFSET2(A4,D5),OFFSET1(A6,A7)
%ŸVp
                MOVE.W  ADDRESS1,OFFSET1(A6,A7)
                MOVE.W  ADDRESS2,OFFSET1(A6,A7)
%ŸVp
                MOVE.W  OFFSET1(PC,A5),OFFSET1(A6,A7)
%ŸVp
                MOVE.W  OFFSET1(PC,D5),OFFSET1(A6,A7)
                MOVE.W  OFFSET1(PC,D5.L),OFFSET1(A6,A7)
                MOVE.W  #10,OFFSET1(A6,A7)
%ŸVp

                MOVE.W  D4,OFFSET1(A6,A7.L)
%ŸVp
                MOVE.W  (A4)+,OFFSET1(A6,A7.L)
%ŸVp
                MOVE.W  ADDRESS2(A4),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),OFFSET1(A6,A7.L)
                MOVE.W  OFFSET2(A4,D5),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.W  ADDRESS1,OFFSET1(A6,A7.L)
                MOVE.W  ADDRESS2,OFFSET1(A6,A7.L)
                MOVE.W  OFFSET1(PC),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),OFFSET1(A6,A7.L)
                MOVE.W  OFFSET1(PC,D5),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.W  #10,OFFSET1(A6,A7.L)
                MOVE.W  #DATA16,OFFSET1(A6,A7.L)

                MOVE.W  D4,OFFSET1(A6,D7)
%ŸVp
                MOVE.W  (A4)+,OFFSET1(A6,D7)
%ŸVp
                MOVE.W  ADDRESS2(A4),OFFSET1(A6,D7)
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),OFFSET1(A6,D7)
                MOVE.W  OFFSET2(A4,D5),OFFSET1(A6,D7)
%ŸVp
                MOVE.W  ADDRESS1,OFFSET1(A6,D7)
                MOVE.W  ADDRESS2,OFFSET1(A6,D7)
%ŸVp
                MOVE.W  OFFSET1(PC,A5),OFFSET1(A6,D7)
%ŸVp
                MOVE.W  OFFSET1(PC,D5),OFFSET1(A6,D7)
                MOVE.W  OFFSET1(PC,D5.L),OFFSET1(A6,D7)
                MOVE.W  #10,OFFSET1(A6,D7)
%ŸVp

                MOVE.W  D4,OFFSET1(A6,D7.L)
%ŸVp
                MOVE.W  (A4)+,OFFSET1(A6,D7.L)
%ŸVp
                MOVE.W  ADDRESS2(A4),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),OFFSET1(A6,D7.L)
                MOVE.W  OFFSET2(A4,D5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.W  ADDRESS1,OFFSET1(A6,D7.L)
                MOVE.W  ADDRESS2,OFFSET1(A6,D7.L)
                MOVE.W  OFFSET1(PC),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),OFFSET1(A6,D7.L)
                MOVE.W  OFFSET1(PC,D5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.W  #10,OFFSET1(A6,D7.L)
                MOVE.W  #DATA16,OFFSET1(A6,D7.L)

                MOVE.W  D4,ADDRESS1
                MOVE.W  (A4),ADDRESS1
%ŸVp
                MOVE.W  -(A4),ADDRESS1
%ŸVp
                MOVE.W  OFFSET2(A4,A5),ADDRESS1
%ŸVp
                MOVE.W  OFFSET2(A4,D5),ADDRESS1
%ŸVp
                MOVE.W  ADDRESS1,ADDRESS1
%ŸVp
                MOVE.W  OFFSET1(PC),ADDRESS1
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),ADDRESS1
%ŸVp
                MOVE.W  OFFSET1(PC,D5.L),ADDRESS1
                MOVE.W  #10,ADDRESS1
%ŸVp

                MOVE.W  D4,ADDRESS2
%ŸVp
                MOVE.W  (A4)+,ADDRESS2
%ŸVp
                MOVE.W  ADDRESS2(A4),ADDRESS2
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),ADDRESS2
%ŸVp
                MOVE.W  OFFSET2(A4,D5.L),ADDRESS2
%ŸVp
                MOVE.W  ADDRESS2,ADDRESS2
                MOVE.W  OFFSET1(PC),ADDRESS2
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),ADDRESS2
%ŸVp
                MOVE.W  OFFSET1(PC,D5.L),ADDRESS2
                MOVE.W  #10,ADDRESS2
%ŸVp


MOVE.B:         MOVE.B  D0,D7
%ŸVp
                MOVE.B  (A4),D6
%ŸVp
                MOVE.B  -(A4),D6
%ŸVp
                MOVE.B  OFFSET2(A4,A5),D6
%ŸVp
                MOVE.B  OFFSET2(A4,D5),D6
%ŸVp
                MOVE.B  ADDRESS1,D6
                MOVE.B  ADDRESS2,D6
%ŸVp
                MOVE.B  OFFSET1(PC,A5),D6
%ŸVp
                MOVE.B  OFFSET1(PC,D5),D6
%ŸVp
                MOVE.B  #10,D6
                MOVE.B  #DATA8,D6

%ŸVp
                MOVE.B  (A4),(A6)
%ŸVp
                MOVE.B  -(A4),(A6)
%ŸVp
                MOVE.B  OFFSET2(A4,A5),(A6)
%ŸVp
                MOVE.B  OFFSET2(A4,D5),(A6)
%ŸVp
                MOVE.B  ADDRESS1,(A6)
                MOVE.B  ADDRESS2,(A6)
%ŸVp
                MOVE.B  OFFSET1(PC,A5),(A6)
%ŸVp
                MOVE.B  OFFSET1(PC,D5),(A6)
%ŸVp
                MOVE.B  #10,(A6)
                MOVE.B  #DATA8,(A6)

%ŸVp
                MOVE.B  (A4),(A6)+
%ŸVp
                MOVE.B  -(A4),(A6)+
%ŸVp
                MOVE.B  OFFSET2(A4,A5),(A6)+
%ŸVp
                MOVE.B  OFFSET2(A4,D5),(A6)+
%ŸVp
                MOVE.B  ADDRESS1,(A6)+
                MOVE.B  ADDRESS2,(A6)+
%ŸVp
                MOVE.B  OFFSET1(PC,A5),(A6)+
%ŸVp
                MOVE.B  OFFSET1(PC,D5),(A6)+
%ŸVp
                MOVE.B  #10,(A6)+
                MOVE.B  #DATA8,(A6)+

%ŸVp
                MOVE.B  (A4),-(A6)
%ŸVp
                MOVE.B  -(A4),-(A6)
%ŸVp
                MOVE.B  OFFSET2(A4,A5),-(A6)
%ŸVp
                MOVE.B  OFFSET2(A4,D5),-(A6)
%ŸVp
                MOVE.B  ADDRESS1,-(A6)
                MOVE.B  ADDRESS2,-(A6)
%ŸVp
                MOVE.B  OFFSET1(PC,A5),-(A6)
%ŸVp
                MOVE.B  OFFSET1(PC,D5),-(A6)
%ŸVp
                MOVE.B  #10,-(A6)
                MOVE.B  #DATA8,-(A6)

%ŸVp
                MOVE.B  (A4),ADDRESS2(A6)
%ŸVp
                MOVE.B  -(A4),ADDRESS2(A6)
%ŸVp
                MOVE.B  OFFSET2(A4,A5),ADDRESS2(A6)
%ŸVp
                MOVE.B  OFFSET2(A4,D5),ADDRESS2(A6)
%ŸVp
                MOVE.B  ADDRESS1,ADDRESS2(A6)
%ŸVp
                MOVE.B  OFFSET1(PC),ADDRESS2(A6)
%ŸVp
                MOVE.B  OFFSET1(PC,A5.L),ADDRESS2(A6)
%ŸVp
                MOVE.B  OFFSET1(PC,D5.L),ADDRESS2(A6)
%ŸVp
                MOVE.B  #DATA8,ADDRESS2(A6)

%ŸVp
                MOVE.B  (A4),OFFSET1(A6,A7)
%ŸVp
                MOVE.B  -(A4),OFFSET1(A6,A7)
%ŸVp
                MOVE.B  OFFSET2(A4,A5),OFFSET1(A6,A7)
%ŸVp
                MOVE.B  OFFSET2(A4,D5),OFFSET1(A6,A7)
                MOVE.B  OFFSET2(A4,D5.L),OFFSET1(A6,A7)
                MOVE.B  ADDRESS1,OFFSET1(A6,A7)
%ŸVp
                MOVE.B  OFFSET1(PC),OFFSET1(A6,A7)
%ŸVp
                MOVE.B  OFFSET1(PC,A5.L),OFFSET1(A6,A7)
                MOVE.B  OFFSET1(PC,D5),OFFSET1(A6,A7)
%ŸVp
                MOVE.B  #10,OFFSET1(A6,A7)
                MOVE.B  #DATA8,OFFSET1(A6,A7)

%ŸVp
                MOVE.B  (A4),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.B  -(A4),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.B  OFFSET2(A4,A5),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.B  OFFSET2(A4,D5),OFFSET1(A6,A7.L)
                MOVE.B  OFFSET2(A4,D5.L),OFFSET1(A6,A7.L)
                MOVE.B  ADDRESS1,OFFSET1(A6,A7.L)
%ŸVp
                MOVE.B  OFFSET1(PC),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.B  OFFSET1(PC,A5.L),OFFSET1(A6,A7.L)
                MOVE.B  OFFSET1(PC,D5),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.B  #10,OFFSET1(A6,A7.L)
                MOVE.B  #DATA8,OFFSET1(A6,A7.L)

                MOVE.B  D4,OFFSET1(A6,D7)
                MOVE.B  (A4),OFFSET1(A6,D7)
%ŸVp
                MOVE.B  -(A4),OFFSET1(A6,D7)
%ŸVp
                MOVE.B  OFFSET2(A4,A5),OFFSET1(A6,D7)
%ŸVp
                MOVE.B  OFFSET2(A4,D5),OFFSET1(A6,D7)
                MOVE.B  OFFSET2(A4,D5.L),OFFSET1(A6,D7)
                MOVE.B  ADDRESS1,OFFSET1(A6,D7)
%ŸVp
                MOVE.B  OFFSET1(PC),OFFSET1(A6,D7)
%ŸVp
                MOVE.B  OFFSET1(PC,A5.L),OFFSET1(A6,D7)
                MOVE.B  OFFSET1(PC,D5),OFFSET1(A6,D7)
%ŸVp
                MOVE.B  #10,OFFSET1(A6,D7)
                MOVE.B  #DATA8,OFFSET1(A6,D7)

%ŸVp
                MOVE.B  (A4),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.B  -(A4),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.B  OFFSET2(A4,A5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.B  OFFSET2(A4,D5),OFFSET1(A6,D7.L)
                MOVE.B  OFFSET2(A4,D5.L),OFFSET1(A6,D7.L)
                MOVE.B  ADDRESS1,OFFSET1(A6,D7.L)
                MOVE.B  ADDRESS2,OFFSET1(A6,D7.L)
%ŸVp
                MOVE.B  OFFSET1(PC,A5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.B  OFFSET1(PC,D5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.B  #10,OFFSET1(A6,D7.L)
                MOVE.B  #DATA8,OFFSET1(A6,D7.L)

                MOVE.B  D4,ADDRESS1
                MOVE.B  (A4),ADDRESS1
                MOVE.B  (A4)+,ADDRESS1
                MOVE.B  -(A4),ADDRESS1
                MOVE.B  ADDRESS2(A4),ADDRESS1
                MOVE.B  OFFSET2(A4,A5),ADDRESS1
%ŸVp
                MOVE.B  OFFSET2(A4,D5),ADDRESS1
%ŸVp
                MOVE.B  ADDRESS1,ADDRESS1
%ŸVp
                MOVE.B  OFFSET1(PC),ADDRESS1
%ŸVp
                MOVE.B  OFFSET1(PC,A5.L),ADDRESS1
%ŸVp
                MOVE.B  OFFSET1(PC,D5.L),ADDRESS1
                MOVE.B  #10,ADDRESS1
                MOVE.B  #DATA8,ADDRESS1

%ŸVp
                MOVE.B  (A4),ADDRESS2
%ŸVp
                MOVE.B  -(A4),ADDRESS2
%ŸVp
                MOVE.B  OFFSET2(A4,A5),ADDRESS2
%ŸVp
                MOVE.B  OFFSET2(A4,D5),ADDRESS2
%ŸVp
                MOVE.B  ADDRESS1,ADDRESS2
%ŸVp
                MOVE.B  OFFSET1(PC),ADDRESS2
%ŸVp
                MOVE.B  OFFSET1(PC,A5.L),ADDRESS2
%ŸVp
                MOVE.B  OFFSET1(PC,D5.L),ADDRESS2
                MOVE.B  #10,ADDRESS2
%ŸVp


MOVE.L:         MOVE.L  D0,D7
                MOVE.L  D7,D0
%ŸVp
                MOVE.L  A0,D7
%ŸVp
                MOVE.L  A7,D7
%ŸVp
                MOVE.L  (A4)+,D6
%ŸVp
                MOVE.L  ADDRESS2(A4),D6
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),D6
%ŸVp
                MOVE.L  OFFSET2(A4,D5.L),D6
                MOVE.L  ADDRESS1,D6
%ŸVp
                MOVE.L  OFFSET1(PC),D6
%ŸVp
                MOVE.L  OFFSET1(PC,A5.L),D6
%ŸVp
                MOVE.L  OFFSET1(PC,D5.L),D6
                MOVE.L  #10,D6
%ŸVp

                MOVE.L  D0,A0
                MOVE.L  D7,A7
%ŸVp
                MOVE.L  A0,A7
%ŸVp
                MOVE.L  A7,A7
%ŸVp
                MOVE.L  (A4)+,A6
%ŸVp
                MOVE.L  ADDRESS2(A4),A6
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),A6
%ŸVp
                MOVE.L  OFFSET2(A4,D5.L),A6
                MOVE.L  ADDRESS1,A6
%ŸVp
                MOVE.L  OFFSET1(PC),A6
%ŸVp
                MOVE.L  OFFSET1(PC,A5.L),A6
%ŸVp
                MOVE.L  OFFSET1(PC,D5.L),A6
                MOVE.L  #10,A6
%ŸVp

                MOVE.L  D4,(A6)
                MOVE.L  (A4),(A6)
%ŸVp
                MOVE.L  -(A4),(A6)
%ŸVp
                MOVE.L  OFFSET2(A4,A5),(A6)
%ŸVp
                MOVE.L  OFFSET2(A4,D5),(A6)
%ŸVp
                MOVE.L  ADDRESS1,(A6)
                MOVE.L  ADDRESS2,(A6)
                MOVE.L  OFFSET1(PC),(A6)
%ŸVp
                MOVE.L  OFFSET1(PC,A5.L),(A6)
%ŸVp
                MOVE.L  OFFSET1(PC,D5.L),(A6)
                MOVE.L  #10,(A6)
%ŸVp

                MOVE.L  D4,(A6)+
%ŸVp
                MOVE.L  (A4)+,(A6)+
%ŸVp
                MOVE.L  ADDRESS2(A4),(A6)+
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),(A6)+
%ŸVp
                MOVE.L  OFFSET2(A4,D5.L),(A6)+
                MOVE.L  ADDRESS1,(A6)+
%ŸVp
                MOVE.L  OFFSET1(PC),(A6)+
%ŸVp
                MOVE.L  OFFSET1(PC,A5.L),(A6)+
%ŸVp
                MOVE.L  OFFSET1(PC,D5.L),(A6)+
                MOVE.L  #10,(A6)+
%ŸVp

                MOVE.L  D4,-(A6)
%ŸVp
                MOVE.L  (A4)+,-(A6)
%ŸVp
                MOVE.L  ADDRESS2(A4),-(A6)
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),-(A6)
%ŸVp
                MOVE.L  OFFSET2(A4,D5.L),-(A6)
                MOVE.L  ADDRESS1,-(A6)
%ŸVp
                MOVE.L  OFFSET1(PC),-(A6)
%ŸVp
                MOVE.L  OFFSET1(PC,A5.L),-(A6)
%ŸVp
                MOVE.L  OFFSET1(PC,D5.L),-(A6)
                MOVE.L  #10,-(A6)
%ŸVp

                MOVE.L  D4,ADDRESS2(A6)
%ŸVp
                MOVE.L  (A4)+,ADDRESS2(A6)
%ŸVp
                MOVE.L  ADDRESS2(A4),ADDRESS2(A6)
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),ADDRESS2(A6)
%ŸVp
                MOVE.L  OFFSET2(A4,D5.L),ADDRESS2(A6)
%ŸVp
                MOVE.L  ADDRESS2,ADDRESS2(A6)
%ŸVp
                MOVE.L  OFFSET1(PC,A5),ADDRESS2(A6)
%ŸVp
                MOVE.L  OFFSET1(PC,D5),ADDRESS2(A6)
%ŸVp
                MOVE.L  #10,ADDRESS2(A6)
%ŸVp

                MOVE.L  D4,OFFSET1(A6,A7)
%ŸVp
                MOVE.L  (A4)+,OFFSET1(A6,A7)
%ŸVp
                MOVE.L  ADDRESS2(A4),OFFSET1(A6,A7)
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),OFFSET1(A6,A7)
                MOVE.L  OFFSET2(A4,D5),OFFSET1(A6,A7)
%ŸVp
                MOVE.L  ADDRESS1,OFFSET1(A6,A7)
                MOVE.L  ADDRESS2,OFFSET1(A6,A7)
%ŸVp
                MOVE.L  OFFSET1(PC,A5),OFFSET1(A6,A7)
%ŸVp
                MOVE.L  OFFSET1(PC,D5),OFFSET1(A6,A7)
                MOVE.L  OFFSET1(PC,D5.L),OFFSET1(A6,A7)
                MOVE.L  #10,OFFSET1(A6,A7)
%ŸVp

                MOVE.L  D4,OFFSET1(A6,A7.L)
%ŸVp
                MOVE.L  (A4)+,OFFSET1(A6,A7.L)
%ŸVp
                MOVE.L  ADDRESS2(A4),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),OFFSET1(A6,A7.L)
                MOVE.L  OFFSET2(A4,D5),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.L  ADDRESS1,OFFSET1(A6,A7.L)
                MOVE.L  ADDRESS2,OFFSET1(A6,A7.L)
                MOVE.L  OFFSET1(PC),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.L  OFFSET1(PC,A5.L),OFFSET1(A6,A7.L)
                MOVE.L  OFFSET1(PC,D5),OFFSET1(A6,A7.L)
%ŸVp
                MOVE.L  #10,OFFSET1(A6,A7.L)
                MOVE.L  #DATA32,OFFSET1(A6,A7.L)

                MOVE.L  D4,OFFSET1(A6,D7)
%ŸVp
                MOVE.L  (A4)+,OFFSET1(A6,D7)
%ŸVp
                MOVE.L  ADDRESS2(A4),OFFSET1(A6,D7)
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),OFFSET1(A6,D7)
                MOVE.L  OFFSET2(A4,D5),OFFSET1(A6,D7)
%ŸVp
                MOVE.L  ADDRESS1,OFFSET1(A6,D7)
                MOVE.L  ADDRESS2,OFFSET1(A6,D7)
%ŸVp
                MOVE.L  OFFSET1(PC,A5),OFFSET1(A6,D7)
%ŸVp
                MOVE.L  OFFSET1(PC,D5),OFFSET1(A6,D7)
                MOVE.L  OFFSET1(PC,D5.L),OFFSET1(A6,D7)
                MOVE.L  #10,OFFSET1(A6,D7)
%ŸVp

                MOVE.L  D4,OFFSET1(A6,D7.L)
%ŸVp
                MOVE.L  (A4)+,OFFSET1(A6,D7.L)
%ŸVp
                MOVE.L  ADDRESS2(A4),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),OFFSET1(A6,D7.L)
                MOVE.L  OFFSET2(A4,D5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.L  ADDRESS1,OFFSET1(A6,D7.L)
                MOVE.L  ADDRESS2,OFFSET1(A6,D7.L)
                MOVE.L  OFFSET1(PC),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.L  OFFSET1(PC,A5.L),OFFSET1(A6,D7.L)
                MOVE.L  OFFSET1(PC,D5),OFFSET1(A6,D7.L)
%ŸVp
                MOVE.L  #10,OFFSET1(A6,D7.L)
                MOVE.L  #DATA32,OFFSET1(A6,D7.L)

                MOVE.L  D4,ADDRESS1
                MOVE.L  (A4),ADDRESS1
%ŸVp
                MOVE.L  -(A4),ADDRESS1
%ŸVp
                MOVE.L  OFFSET2(A4,A5),ADDRESS1
%ŸVp
                MOVE.L  OFFSET2(A4,D5),ADDRESS1
%ŸVp
                MOVE.L  ADDRESS1,ADDRESS1
%ŸVp
                MOVE.L  OFFSET1(PC),ADDRESS1
%ŸVp
                MOVE.L  OFFSET1(PC,A5.L),ADDRESS1
%ŸVp
                MOVE.L  OFFSET1(PC,D5.L),ADDRESS1
                MOVE.L  #10,ADDRESS1
%ŸVp

                MOVE.L  D4,ADDRESS2
%ŸVp
                MOVE.L  (A4)+,ADDRESS2
%ŸVp
                MOVE.L  ADDRESS2(A4),ADDRESS2
%ŸVp
                MOVE.L  OFFSET2(A4,A5.L),ADDRESS2
%ŸVp
                MOVE.L  OFFSET2(A4,D5.L),ADDRESS2
%ŸVp
                MOVE.L  ADDRESS2,ADDRESS2
%ŸVp
                MOVE.L  OFFSET1(PC,A5),ADDRESS2
%ŸVp
                MOVE.L  OFFSET1(PC,D5),ADDRESS2
%ŸVp
                MOVE.L  #10,ADDRESS2
                MOVE.L  #DATA32,ADDRESS2


%ŸVp
                MOVE    (A4),CCR
%ŸVp
                MOVE    -(A4),CCR
%ŸVp
                MOVE    OFFSET2(A4,A5),CCR
%ŸVp
                MOVE    OFFSET2(A4,D5),CCR
%ŸVp
                MOVE    ADDRESS1,CCR
                MOVE    ADDRESS2,CCR

%ŸVp
                MOVE.B  (A4),CCR
%ŸVp
                MOVE.B  -(A4),CCR
%ŸVp
                MOVE.B  OFFSET2(A4,A5),CCR
%ŸVp
                MOVE.B  OFFSET2(A4,D5),CCR
%ŸVp
                MOVE.B  ADDRESS1,CCR
                MOVE.B  ADDRESS2,CCR

%ŸVp
                MOVE    CCR,(A4)
%ŸVp
                MOVE    CCR,-(A4)
%ŸVp
                MOVE    CCR,OFFSET2(A4,A5)
%ŸVp
                MOVE    CCR,OFFSET2(A4,D5)
%ŸVp
                MOVE    CCR,ADDRESS1
                MOVE    CCR,ADDRESS2

%ŸVp
                MOVE.B  CCR,(A4)
%ŸVp
                MOVE.B  CCR,-(A4)
%ŸVp
                MOVE.B  CCR,OFFSET2(A4,A5)
%ŸVp
                MOVE.B  CCR,OFFSET2(A4,D5)
%ŸVp
                MOVE.B  CCR,ADDRESS1
                MOVE.B  CCR,ADDRESS2


%ŸVp
                MOVEC   SFC,D7
%ŸVp
                MOVEC   DFC,D7
%ŸVp
                MOVEC   VBR,D7
%ŸVp
                MOVEC   SFC,A7
%ŸVp
                MOVEC   DFC,A7
%ŸVp
                MOVEC   VBR,A7

%ŸVp
                MOVEC   D7,SFC
%ŸVp
                MOVEC   D7,DFC
%ŸVp
                MOVEC   D7,VBR
%ŸVp
                MOVEC   A7,SFC
%ŸVp
                MOVEC   A7,DFC
%ŸVp
                MOVEC   A7,VBR


%ŸVp
                MOVES   A0,(A4)+
%ŸVp
                MOVES   D0,ADDRESS2(A4)
%ŸVp
                MOVES   A0,OFFSET2(A4,A5.L)
%ŸVp
                MOVES   D0,OFFSET2(A4,D5.L)
                MOVES   A7,ADDRESS1
%ŸVp

                MOVES.B D4,(A4)
%ŸVp
                MOVES.B A4,-(A4)
%ŸVp
                MOVES.B D4,OFFSET2(A4,A5)
%ŸVp
                MOVES.B A4,OFFSET2(A4,D5)
%ŸVp
                MOVES.B D4,ADDRESS1
                MOVES.B D6,ADDRESS2

%ŸVp
                MOVES.L D6,(A4)+
%ŸVp
                MOVES.L A6,ADDRESS2(A4)
%ŸVp
                MOVES.L D6,OFFSET2(A4,A5.L)
%ŸVp
                MOVES.L A6,OFFSET2(A4,D5.L)
                MOVES.L D4,ADDRESS1
%ŸVp

                MOVES   (A4),D7
%ŸVp
                MOVES   -(A4),A7
%ŸVp
                MOVES   OFFSET2(A4,A5),D7
%ŸVp
                MOVES   OFFSET2(A4,D5),A7
%ŸVp
                MOVES   ADDRESS1,A7
                MOVES   ADDRESS2,D0

%ŸVp
                MOVES.B (A4)+,D6
%ŸVp
                MOVES.B ADDRESS2(A4),A6
%ŸVp
                MOVES.B OFFSET2(A4,A5.L),D6
%ŸVp
                MOVES.B OFFSET2(A4,D5.L),A6
                MOVES.B ADDRESS1,D4
%ŸVp

                MOVES.L (A4),D4
%ŸVp
                MOVES.L -(A4),A4
%ŸVp
                MOVES.L OFFSET2(A4,A5),D4
%ŸVp
                MOVES.L OFFSET2(A4,D5),A4
%ŸVp
                MOVES.L ADDRESS1,D4
                MOVES.L ADDRESS2,D6

%ŸVp
                MOVEQ   #QUICK,D6

%ŸVp
                MOVEQ.L #QUICK,D6


%ŸVp
                MOVE    (A4),CCR
%ŸVp
                MOVE    -(A4),CCR
%ŸVp
                MOVE    OFFSET2(A4,A5),CCR
%ŸVp
                MOVE    OFFSET2(A4,D5),CCR
%ŸVp
                MOVE    ADDRESS1,CCR
                MOVE    ADDRESS2,CCR
%ŸVp
                MOVE    OFFSET1(PC,A5),CCR
%ŸVp
                MOVE    OFFSET1(PC,D5),CCR
%ŸVp
                MOVE    #10,CCR
                MOVE    #DATA8,CCR

%ŸVp
                MOVE.B  (A4),CCR
%ŸVp
                MOVE.B  -(A4),CCR
%ŸVp
                MOVE.B  OFFSET2(A4,A5),CCR
%ŸVp
                MOVE.B  OFFSET2(A4,D5),CCR
%ŸVp
                MOVE.B  ADDRESS1,CCR
                MOVE.B  ADDRESS2,CCR
%ŸVp
                MOVE.B  OFFSET1(PC,A5),CCR
%ŸVp
                MOVE.B  OFFSET1(PC,D5),CCR
%ŸVp
                MOVE.B  #10,CCR
                MOVE.B  #DATA8,CCR

%ŸVp
                MOVE    (A4),SR
%ŸVp
                MOVE    -(A4),SR
%ŸVp
                MOVE    OFFSET2(A4,A5),SR
%ŸVp
                MOVE    OFFSET2(A4,D5),SR
%ŸVp
                MOVE    ADDRESS1,SR
                MOVE    ADDRESS2,SR
%ŸVp
                MOVE    OFFSET1(PC,A5),SR
%ŸVp
                MOVE    OFFSET1(PC,D5),SR
%ŸVp
                MOVE    #10,SR
                MOVE    #DATA16,SR
%ŸVp

                MOVE.W  D4,SR
%ŸVp
                MOVE.W  (A4)+,SR
%ŸVp
                MOVE.W  ADDRESS2(A4),SR
%ŸVp
                MOVE.W  OFFSET2(A4,A5.L),SR
%ŸVp
                MOVE.W  OFFSET2(A4,D5.L),SR
                MOVE.W  ADDRESS1,SR
%ŸVp
                MOVE.W  OFFSET1(PC),SR
%ŸVp
                MOVE.W  OFFSET1(PC,A5.L),SR
%ŸVp
                MOVE.W  OFFSET1(PC,D5.L),SR
                MOVE.W  #10,SR
%ŸVp
                MOVE.W  #DATA16,SR

%ŸVp
                MOVE    SR,(A4)
%ŸVp
                MOVE    SR,-(A4)
%ŸVp
                MOVE    SR,OFFSET2(A4,A5)
%ŸVp
                MOVE    SR,OFFSET2(A4,D5)
%ŸVp
                MOVE    SR,ADDRESS1
                MOVE    SR,ADDRESS2

%ŸVp
                MOVE.W  SR,(A4)
%ŸVp
                MOVE.W  SR,-(A4)
%ŸVp
                MOVE.W  SR,OFFSET2(A4,A5)
%ŸVp
                MOVE.W  SR,OFFSET2(A4,D5)
%ŸVp
                MOVE.W  SR,ADDRESS1
                MOVE.W  SR,ADDRESS2

%ŸVp
                MOVE.L  A0,USP
%ŸVp
                MOVE.L  A7,USP
%ŸVp
                MOVE.L  USP,A0
%ŸVp
                MOVE.L  USP,A7

%ŸVp
                MOVEP.W ADDRESS1(A4),D6

%ŸVp
                MOVEP.W D6,ADDRESS1(A4)

%ŸVp
                MOVEP.L D6,ADDRESS1(A4)


%ŸVp
                MOVEM   (A4)+,D0-D7
%ŸVp
                MOVEM   OFFSET2(A4,A5),D0-D7
%ŸVp
                MOVEM   OFFSET2(A4,D5),D0-D7
%ŸVp
                MOVEM   ADDRESS1,D0-D7
                MOVEM   ADDRESS2,D0-D7
%ŸVp
                MOVEM   OFFSET1(PC,A5),D0-D7
%ŸVp
                MOVEM   OFFSET1(PC,D5),D0-D7
%ŸVp

                MOVEM   D0-D7,(A4)
                MOVEM   D0-D7,-(A4)
%ŸVp
                MOVEM   D0-D7,OFFSET2(A4,A5)
%ŸVp
                MOVEM   D0-D7,OFFSET2(A4,D5)
%ŸVp
                MOVEM   D0-D7,ADDRESS1
                MOVEM   D0-D7,ADDRESS2

%ŸVp
                MOVEM.W (A4)+,D0-D7
%ŸVp
                MOVEM.W OFFSET2(A4,A5),D0-D7
%ŸVp
                MOVEM.W OFFSET2(A4,D5),D0-D7
%ŸVp
                MOVEM.W ADDRESS1,D0-D7
                MOVEM.W ADDRESS2,D0-D7
                MOVEM.W OFFSET1(PC),D0-D7
%ŸVp
                MOVEM.W OFFSET1(PC,A5.L),D0-D7
%ŸVp
                MOVEM.W OFFSET1(PC,D5.L),D0-D7

%ŸVp
                MOVEM.W D0-D7,-(A4)
%ŸVp
                MOVEM.W D0-D7,OFFSET2(A4,A5)
%ŸVp
                MOVEM.W D0-D7,OFFSET2(A4,D5)
%ŸVp
                MOVEM.W D0-D7,ADDRESS1
                MOVEM.W D0-D7,ADDRESS2

                MOVEM.L (A4),D0-D7
                MOVEM.L (A4)+,D0-D7
%ŸVp
                MOVEM.L OFFSET2(A4,A5),D0-D7
%ŸVp
                MOVEM.L OFFSET2(A4,D5),D0-D7
%ŸVp
                MOVEM.L ADDRESS1,D0-D7
                MOVEM.L ADDRESS2,D0-D7
                MOVEM.L OFFSET1(PC),D0-D7
%ŸVp
                MOVEM.L OFFSET1(PC,A5.L),D0-D7
%ŸVp
                MOVEM.L OFFSET1(PC,D5.L),D0-D7

%ŸVp
                MOVEM.L D0-D7,-(A4)
%ŸVp
                MOVEM.L D0-D7,OFFSET2(A4,A5)
%ŸVp
                MOVEM.L D0-D7,OFFSET2(A4,D5)
%ŸVp
                MOVEM.L D0-D7,ADDRESS1
                MOVEM.L D0-D7,ADDRESS2

                MOVEM   (A4),D0-A7
                MOVEM   (A4)+,D0-A7
%ŸVp
                MOVEM   D0-A7,-(A4)
%ŸVp
                MOVEM   D0,(A4)
%ŸVp
                MOVEM   A4,(A4)
%ŸVp
                MOVEM   D0,-(A4)
%ŸVp
                MOVEM   A4,-(A4)
%ŸVp
                MOVEM   D0/D1/D2/A7,(A4)
%ŸVp
                MOVEM   D0/D1/A0-A7,-(A4)
%ŸVp
                MOVEM   D1-D3/D5/A5-A7,-(A4)


%ŸVp
                MULS    (A4),D6
%ŸVp
                MULS    -(A4),D6
%ŸVp
                MULS    OFFSET2(A4,A5),D6
%ŸVp
%ŸVp
                MULS    OFFSET2(A4,D5.L),D6
                MULS    ADDRESS1,D6
                MULS    ADDRESS2,D6
%ŸVp
                MULS    OFFSET1(PC,A5),D6
%ŸVp
                MULS    OFFSET1(PC,D5),D6
%ŸVp
                MULS    #DATA16,D6


MULU:           MULU    D4,D6
%ŸVp
                MULU    (A4)+,D6
%ŸVp
                MULU    ADDRESS2(A4),D6
%ŸVp
                MULU    OFFSET2(A4,A5.L),D6
%ŸVp
                MULU    OFFSET2(A4,D5.L),D6
                MULU    ADDRESS1,D6
%ŸVp
                MULU    OFFSET1(PC),D6
%ŸVp
                MULU    OFFSET1(PC,A5.L),D6
%ŸVp
                MULU    OFFSET1(PC,D5.L),D6
                MULU    #DATA16,D6


%ŸVp
                NBCD    D6
%ŸVp
                NBCD    (A4)+
%ŸVp
                NBCD    ADDRESS2(A4)
%ŸVp
                NBCD    OFFSET2(A4,A5.L)
%ŸVp
                NBCD    OFFSET2(A4,D5.L)
%ŸVp
                NBCD    ADDRESS2


%ŸVp
                NEG     D6
%ŸVp
                NEG     (A4)+
%ŸVp
                NEG     ADDRESS2(A4)
%ŸVp
                NEG     OFFSET2(A4,A5.L)
%ŸVp
                NEG     OFFSET2(A4,D5.L)
%ŸVp
                NEG     ADDRESS2


%ŸVp
                NEG.B   D4
%ŸVp
                NEG.B   (A4)+
%ŸVp
                NEG.B   ADDRESS2(A4)
%ŸVp
                NEG.B   OFFSET2(A4,A5.L)
                NEG.B   OFFSET2(A4,D5)
%ŸVp
                NEG.B   ADDRESS1
%ŸVp

NEG.W:          NEG.W   D4
%ŸVp
                NEG.W   (A4)
%ŸVp
                NEG.W   -(A4)
%ŸVp
                NEG.W   OFFSET2(A4,A5)
%ŸVp
                NEG.W   OFFSET2(A4,D5)
%ŸVp
                NEG.W   ADDRESS1
%ŸVp

NEG.L:          NEG.L   D4
%ŸVp
                NEG.L   (A4)
%ŸVp
                NEG.L   -(A4)
%ŸVp
                NEG.L   OFFSET2(A4,A5)
%ŸVp
                NEG.L   OFFSET2(A4,D5)
%ŸVp
                NEG.L   ADDRESS1
%ŸVp


NEGX:           NEGX    D4
%ŸVp
                NEGX    (A4)
%ŸVp
                NEGX    -(A4)
%ŸVp
                NEGX    OFFSET2(A4,A5)
%ŸVp
                NEGX    OFFSET2(A4,D5)
%ŸVp
                NEGX    ADDRESS1
%ŸVp

NEGX.B:         NEGX.B  D6
%ŸVp
                NEGX.B  (A4)
%ŸVp
                NEGX.B  -(A4)
%ŸVp
                NEGX.B  OFFSET2(A4,A5)
%ŸVp
                NEGX.B  OFFSET2(A4,D5)
%ŸVp
                NEGX.B  ADDRESS1
%ŸVp

NEGX.W:         NEGX.W  D4
%ŸVp
                NEGX.W  (A4)
%ŸVp
                NEGX.W  -(A4)
%ŸVp
                NEGX.W  OFFSET2(A4,A5)
%ŸVp
                NEGX.W  OFFSET2(A4,D5)
%ŸVp
                NEGX.W  ADDRESS1
%ŸVp

NEGX.L:         NEGX.L  D4
%ŸVp
                NEGX.L  (A4)
%ŸVp
                NEGX.L  -(A4)
%ŸVp
                NEGX.L  OFFSET2(A4,A5)
%ŸVp
                NEGX.L  OFFSET2(A4,D5)
%ŸVp
                NEGX.L  ADDRESS1
%ŸVp


NOP:            NOP


%ŸVp
                NOT     D6
%ŸVp
                NOT     (A4)+
%ŸVp
                NOT     ADDRESS2(A4)
%ŸVp
                NOT     OFFSET2(A4,A5.L)
%ŸVp
                NOT     OFFSET2(A4,D5.L)
%ŸVp
                NOT     ADDRESS2

%ŸVp
                NOT.B   D4
%ŸVp
                NOT.B   (A4)+
%ŸVp
                NOT.B   ADDRESS2(A4)
%ŸVp
                NOT.B   OFFSET2(A4,A5.L)
                NOT.B   OFFSET2(A4,D5)
%ŸVp
                NOT.B   ADDRESS1
%ŸVp

NOT.W:          NOT.W   D4
%ŸVp
                NOT.W   (A4)
%ŸVp
                NOT.W   -(A4)
%ŸVp
                NOT.W   OFFSET2(A4,A5)
%ŸVp
                NOT.W   OFFSET2(A4,D5)
%ŸVp
                NOT.W   ADDRESS1
%ŸVp

NOT.L:          NOT.L   D4
%ŸVp
                NOT.L   (A4)
%ŸVp
                NOT.L   -(A4)
%ŸVp
                NOT.L   OFFSET2(A4,A5)
%ŸVp
                NOT.L   OFFSET2(A4,D5)
%ŸVp
                NOT.L   ADDRESS1
%ŸVp


OR:             OR      D4,D6
%ŸVp
                OR      (A4)+,D6
%ŸVp
                OR      ADDRESS2(A4),D6
%ŸVp
                OR      OFFSET2(A4,A5.L),D6
%ŸVp
                OR      OFFSET2(A4,D5.L),D6
                OR      ADDRESS1,D6
%ŸVp
                OR      OFFSET1(PC),D6
%ŸVp
                OR      OFFSET1(PC,A5.L),D6
%ŸVp
                OR      OFFSET1(PC,D5.L),D6
                OR      #10,D6
%ŸVp

                OR      D4,D6
%ŸVp
                OR      D4,(A6)+
%ŸVp
                OR      D4,ADDRESS2(A6)
%ŸVp
                OR      D4,OFFSET1(A6,A4.L)
%ŸVp
                OR      D4,OFFSET1(A6,D4.L)
                OR      D4,ADDRESS1
%ŸVp

                OR      #10,D4
%ŸVp
                OR      #10,(A4)
%ŸVp
                OR      #ADDRESS1,(A4)+
%ŸVp
                OR      #ADDRESS1,ADDRESS2(A4)
%ŸVp
                OR      #ADDRESS1,OFFSET2(A4,A6.L)
%ŸVp
                OR      #ADDRESS1,OFFSET2(A4,D6.L)
%ŸVp
                OR      #ADDRESS1,ADDRESS2

%ŸVp
                ORI     #ADDRESS1,D4
%ŸVp
                ORI     #ADDRESS1,(A4)
%ŸVp
                ORI     #ADDRESS1,-(A4)
%ŸVp
                ORI     #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                ORI     #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                ORI     #ADDRESS1,ADDRESS1
%ŸVp

OR.W:           OR.W    D4,D6
                OR.W    (A4),D6
%ŸVp
                OR.W    -(A4),D6
%ŸVp
                OR.W    OFFSET2(A4,A5),D6
%ŸVp
                OR.W    OFFSET2(A4,D5),D6
%ŸVp
                OR.W    ADDRESS1,D6
                OR.W    ADDRESS2,D6
%ŸVp
                OR.W    OFFSET1(PC,A5),D6
%ŸVp
                OR.W    OFFSET1(PC,D5),D6
%ŸVp
                OR.W    #10,D6
                OR.W    #ADDRESS1,D6

%ŸVp
                OR.W    D4,(A6)
%ŸVp
                OR.W    D4,-(A6)
%ŸVp
                OR.W    D4,OFFSET1(A6,A4)
%ŸVp
                OR.W    D4,OFFSET1(A6,D4)
%ŸVp
                OR.W    D4,ADDRESS1
                OR.W    D4,ADDRESS2

%ŸVp
                OR.W    #ADDRESS1,D4
%ŸVp
                OR.W    #ADDRESS1,(A4)
%ŸVp
                OR.W    #ADDRESS1,-(A4)
%ŸVp
                OR.W    #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                OR.W    #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                OR.W    #ADDRESS1,ADDRESS1
                OR.W    #ADDRESS1,ADDRESS2

%ŸVp
                ORI.W   #ADDRESS1,D4
%ŸVp
                ORI.W   #ADDRESS1,(A4)
%ŸVp
                ORI.W   #ADDRESS1,-(A4)
%ŸVp
                ORI.W   #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                ORI.W   #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                ORI.W   #ADDRESS1,ADDRESS1
%ŸVp

OR.B:           OR.B    D4,D6
                OR.B    (A4),D6
%ŸVp
                OR.B    -(A4),D6
%ŸVp
                OR.B    OFFSET2(A4,A5),D6
%ŸVp
                OR.B    OFFSET2(A4,D5),D6
%ŸVp
                OR.B    ADDRESS1,D6
                OR.B    ADDRESS2,D6
%ŸVp
                OR.B    OFFSET1(PC,A5),D6
%ŸVp
                OR.B    OFFSET1(PC,D5),D6
%ŸVp
                OR.B    #10,D6
                OR.B    #DATA8,D6

%ŸVp
                OR.B    D4,(A6)
%ŸVp
                OR.B    D4,-(A6)
%ŸVp
                OR.B    D4,OFFSET1(A6,A4)
%ŸVp
                OR.B    D4,OFFSET1(A6,D4)
%ŸVp
                OR.B    D4,ADDRESS1
                OR.B    D4,ADDRESS2

%ŸVp
                OR.B    #DATA8,D4
%ŸVp
                OR.B    #DATA8,(A4)
%ŸVp
                OR.B    #DATA8,-(A4)
%ŸVp
                OR.B    #DATA8,OFFSET2(A4,A6)
%ŸVp
                OR.B    #DATA8,OFFSET2(A4,D6)
%ŸVp
                OR.B    #DATA8,ADDRESS1
                OR.B    #DATA8,ADDRESS2

%ŸVp
                ORI.B   #DATA8,D4
%ŸVp
                ORI.B   #DATA8,(A4)
%ŸVp
                ORI.B   #DATA8,-(A4)
%ŸVp
                ORI.B   #DATA8,OFFSET2(A4,A6)
%ŸVp
                ORI.B   #DATA8,OFFSET2(A4,D6)
%ŸVp
                ORI.B   #DATA8,ADDRESS1

OR.L:           OR.L    D4,D6
%ŸVp
                OR.L    (A4)+,D6
%ŸVp
                OR.L    ADDRESS2(A4),D6
%ŸVp
                OR.L    OFFSET2(A4,A5.L),D6
%ŸVp
                OR.L    OFFSET2(A4,D5.L),D6
                OR.L    ADDRESS1,D6
%ŸVp
                OR.L    OFFSET1(PC),D6
%ŸVp
                OR.L    OFFSET1(PC,A5.L),D6
%ŸVp
                OR.L    OFFSET1(PC,D5.L),D6
                OR.L    #10,D6
%ŸVp

                OR.L    D4,D6
%ŸVp
                OR.L    D4,(A6)+
%ŸVp
                OR.L    D4,ADDRESS2(A6)
%ŸVp
                OR.L    D4,OFFSET1(A6,A4.L)
%ŸVp
                OR.L    D4,OFFSET1(A6,D4.L)
                OR.L    D4,ADDRESS1
%ŸVp

                OR.L    #10,D4
%ŸVp
                OR.L    #10,(A4)
%ŸVp
                OR.L    #DATA32,(A4)+
%ŸVp
                OR.L    #DATA32,ADDRESS2(A4)
%ŸVp
                OR.L    #DATA32,OFFSET2(A4,A6.L)
%ŸVp
                OR.L    #DATA32,OFFSET2(A4,D6.L)
%ŸVp
                OR.L    #DATA32,ADDRESS2

%ŸVp
                ORI.L   #DATA32,D4
%ŸVp
                ORI.L   #DATA32,(A4)
%ŸVp
                ORI.L   #DATA32,-(A4)
%ŸVp
                ORI.L   #DATA32,OFFSET2(A4,A6)
%ŸVp
                ORI.L   #DATA32,OFFSET2(A4,D6)
%ŸVp
                ORI.L   #DATA32,ADDRESS1
%ŸVp


PEA:            PEA     (A4)
                PEA     ADDRESS2(A4)
%ŸVp
                PEA     OFFSET2(A4,A5.L)
%ŸVp
                PEA     OFFSET2(A4,D5.L)
%ŸVp
                PEA     ADDRESS2
%ŸVp
                PEA     OFFSET1(PC,A5)
%ŸVp
                PEA     OFFSET1(PC,D5)
%ŸVp


RESET:          RESET


%ŸVp
                ROL     D6,D4
%ŸVp
                ROL     #8,D4
%ŸVp
                ROL     (A4)+
%ŸVp
                ROL     ADDRESS2(A4)
%ŸVp
                ROL     OFFSET2(A4,A5.L)
%ŸVp
                ROL     OFFSET2(A4,D5.L)
%ŸVp
                ROL     ADDRESS2

%ŸVp
                ROL.B   D6,D4
%ŸVp
                ROL.B   #8,D4

%ŸVp
                ROL.W   D6,D4
%ŸVp
                ROL.W   #8,D4
%ŸVp
                ROL.W   (A4)+
%ŸVp
                ROL.W   ADDRESS2(A4)
%ŸVp
                ROL.W   OFFSET2(A4,A5.L)
                ROL.W   OFFSET2(A4,D5)
%ŸVp
                ROL.W   ADDRESS1
%ŸVp

ROL.L:          ROL.L   D4,D6
%ŸVp
                ROL.L   #1,D6
%ŸVp


ROR:            ROR     D4,D6
%ŸVp
                ROR     #1,D6
%ŸVp
                ROR     (A4)
%ŸVp
                ROR     -(A4)
%ŸVp
                ROR     OFFSET2(A4,A5)
%ŸVp
                ROR     OFFSET2(A4,D5)
%ŸVp
                ROR     ADDRESS1
%ŸVp

ROR.B:          ROR.B   D6,D4
%ŸVp
                ROR.B   #1,D6
%ŸVp

ROR.W:          ROR.W   D4,D6
%ŸVp
                ROR.W   #1,D6
%ŸVp
                ROR.W   (A4)
%ŸVp
                ROR.W   -(A4)
%ŸVp
                ROR.W   OFFSET2(A4,A5)
%ŸVp
                ROR.W   OFFSET2(A4,D5)
%ŸVp
                ROR.W   ADDRESS1
%ŸVp

ROR.L:          ROR.L   D4,D6
%ŸVp
                ROR.L   #1,D6
%ŸVp


ROXL:           ROXL    D4,D6
%ŸVp
                ROXL    #1,D6
%ŸVp
                ROXL    (A4)
%ŸVp
                ROXL    -(A4)
%ŸVp
                ROXL    OFFSET2(A4,A5)
%ŸVp
                ROXL    OFFSET2(A4,D5)
%ŸVp
                ROXL    ADDRESS1
%ŸVp

ROXL.B:         ROXL.B  D6,D4
%ŸVp
                ROXL.B  #1,D6
%ŸVp

ROXL.W:         ROXL.W  D4,D6
%ŸVp
                ROXL.W  #1,D6
%ŸVp
                ROXL.W  (A4)
%ŸVp
                ROXL.W  -(A4)
%ŸVp
                ROXL.W  OFFSET2(A4,A5)
%ŸVp
                ROXL.W  OFFSET2(A4,D5)
%ŸVp
                ROXL.W  ADDRESS1
%ŸVp

ROXL.L:         ROXL.L  D4,D6
%ŸVp
                ROXL.L  #1,D6
%ŸVp


ROXR:           ROXR    D4,D6
%ŸVp
                ROXR    #1,D6
%ŸVp
                ROXR    (A4)
%ŸVp
                ROXR    -(A4)
%ŸVp
                ROXR    OFFSET2(A4,A5)
%ŸVp
                ROXR    OFFSET2(A4,D5)
%ŸVp
                ROXR    ADDRESS1
%ŸVp

ROXR.B:         ROXR.B  D6,D4
%ŸVp
                ROXR.B  #1,D6
%ŸVp

ROXR.W:         ROXR.W  D4,D6
%ŸVp
                ROXR.W  #1,D6
%ŸVp
                ROXR.W  (A4)
%ŸVp
                ROXR.W  -(A4)
%ŸVp
                ROXR.W  OFFSET2(A4,A5)
%ŸVp
                ROXR.W  OFFSET2(A4,D5)
%ŸVp
                ROXR.W  ADDRESS1
%ŸVp

ROXR.L:         ROXR.L  D4,D6
%ŸVp
                ROXR.L  #1,D6
%ŸVp

RTD:            RTD     DATA8
%ŸVp
                RTD     DATA16
%ŸVp

RTE:            RTE


%ŸVp


%ŸVp


%ŸVp
                SBCD    D7,D0
%ŸVp
                SBCD    (A0),(A7)
%ŸVp
                SBCD    (A4),(A6)
%ŸVp


ST:             ST      D4
%ŸVp
                ST      (A4)+
%ŸVp
                ST      ADDRESS2(A4)
%ŸVp
                ST      OFFSET2(A4,A5.L)
%ŸVp
                ST      OFFSET2(A4,D5.L)
%ŸVp
                ST      ADDRESS2

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp

%ŸVp


%ŸVp
                STOP    #DATA16


%ŸVp
                SUB     A4,D6
%ŸVp
                SUB     (A4)+,D6
%ŸVp
                SUB     ADDRESS2(A4),D6
%ŸVp
                SUB     OFFSET2(A4,A5.L),D6
%ŸVp
                SUB     OFFSET2(A4,D5.L),D6
                SUB     ADDRESS1,D6
%ŸVp
                SUB     OFFSET1(PC),D6
%ŸVp
                SUB     OFFSET1(PC,A5.L),D6
%ŸVp
                SUB     OFFSET1(PC,D5.L),D6
                SUB     #10,D6
%ŸVp

                SUB     D4,D6
%ŸVp
                SUB     D4,(A6)
%ŸVp
                SUB     D4,-(A6)
%ŸVp
                SUB     D4,OFFSET1(A6,A4)
%ŸVp
                SUB     D4,OFFSET1(A6,D4)
%ŸVp
                SUB     D4,ADDRESS1
%ŸVp

                SUB     D4,A6
%ŸVp
                SUB     (A4),A6
%ŸVp
                SUB     -(A4),A6
                SUB     ADDRESS2(A4),A6
%ŸVp
                SUB     OFFSET2(A4,A6.L),A6
%ŸVp
                SUB     OFFSET2(A4,D6.L),A6
                SUB     ADDRESS1,A6
%ŸVp
                SUB     OFFSET1(PC),A6
%ŸVp
                SUB     OFFSET1(PC,A6.L),A6
%ŸVp
                SUB     OFFSET1(PC,D6.L),A6
                SUB     #10,A6
%ŸVp

                SUB     #10,D4
%ŸVp
                SUB     #10,(A4)
%ŸVp
                SUB     #DATA8,(A4)+
%ŸVp
                SUB     #DATA8,ADDRESS2(A4)
%ŸVp
                SUB     #DATA8,OFFSET2(A4,A6.L)
%ŸVp
                SUB     #DATA8,OFFSET2(A4,D6.L)
                SUB     #DATA8,ADDRESS1
%ŸVp

                SUB     #1,D4
%ŸVp
                SUB     #8,D4
%ŸVp
                SUB     #QUICK,D4
%ŸVp
                SUB     #QUICK,A4
%ŸVp
                SUB     #QUICK,(A4)
%ŸVp
                SUB     #QUICK,(A4)+
%ŸVp
                SUB     #QUICK,-(A4)
%ŸVp
                SUB     #QUICK,ADDRESS2(A4)
%ŸVp
                SUB     #QUICK,OFFSET2(A4,A6)
%ŸVp
                SUB     #QUICK,OFFSET2(A4,A6.L)
%ŸVp
                SUB     #QUICK,OFFSET2(A4,D6)
%ŸVp
                SUB     #QUICK,OFFSET2(A4,D6.L)
%ŸVp


SUBA:           SUBA    D4,A6
                SUBA    A4,A6
%ŸVp
                SUBA    (A4)+,A6
%ŸVp
                SUBA    ADDRESS2(A4),A6
%ŸVp
                SUBA    OFFSET2(A4,A6.L),A6
%ŸVp
                SUBA    OFFSET2(A4,D6.L),A6
                SUBA    ADDRESS1,A6
%ŸVp
                SUBA    OFFSET1(PC),A6
%ŸVp
                SUBA    OFFSET1(PC,A6.L),A6
%ŸVp
                SUBA    OFFSET1(PC,D6.L),A6
                SUBA    #10,A6
%ŸVp


SUBI:           SUBI    #10,D4
%ŸVp
                SUBI    #10,(A4)
%ŸVp
                SUBI    #ADDRESS1,(A4)+
%ŸVp
                SUBI    #ADDRESS1,ADDRESS2(A4)
%ŸVp
                SUBI    #ADDRESS1,OFFSET2(A4,A6.L)
%ŸVp
                SUBI    #ADDRESS1,OFFSET2(A4,D6.L)
%ŸVp
                SUBI    #ADDRESS1,ADDRESS2


%ŸVp
                SUB.W   A4,D6
%ŸVp
                SUB.W   (A4)+,D6
%ŸVp
                SUB.W   ADDRESS2(A4),D6
%ŸVp
                SUB.W   OFFSET2(A4,A5.L),D6
%ŸVp
                SUB.W   OFFSET2(A4,D5.L),D6
                SUB.W   ADDRESS1,D6
%ŸVp
                SUB.W   OFFSET1(PC),D6
%ŸVp
                SUB.W   OFFSET1(PC,A5.L),D6
%ŸVp
                SUB.W   OFFSET1(PC,D5.L),D6
                SUB.W   #10,D6
%ŸVp

                SUB.W   D4,D6
%ŸVp
                SUB.W   D4,(A6)
%ŸVp
                SUB.W   D4,-(A6)
%ŸVp
                SUB.W   D4,OFFSET1(A6,A4)
%ŸVp
                SUB.W   D4,OFFSET1(A6,D4)
%ŸVp
                SUB.W   D4,ADDRESS1
                SUB.W   D4,ADDRESS2

%ŸVp
                SUB.W   A4,A6
%ŸVp
                SUB.W   (A4)+,A6
%ŸVp
                SUB.W   ADDRESS2(A4),A6
%ŸVp
                SUB.W   OFFSET2(A4,A6.L),A6
%ŸVp
                SUB.W   OFFSET2(A4,D6.L),A6
                SUB.W   ADDRESS1,A6
%ŸVp
                SUB.W   OFFSET1(PC),A6
%ŸVp
                SUB.W   OFFSET1(PC,A6.L),A6
%ŸVp
                SUB.W   OFFSET1(PC,D6.L),A6
                SUB.W   #10,A6
%ŸVp

                SUB.W   #10,D4
%ŸVp
                SUB.W   #10,(A4)
%ŸVp
                SUB.W   #ADDRESS1,(A4)+
%ŸVp
                SUB.W   #ADDRESS1,ADDRESS2(A4)
%ŸVp
                SUB.W   #ADDRESS1,OFFSET2(A4,A6.L)
%ŸVp
                SUB.W   #ADDRESS1,OFFSET2(A4,D6.L)
%ŸVp
                SUB.W   #ADDRESS1,ADDRESS2

%ŸVp
                SUBQ.W  #1,D4
%ŸVp
                SUBQ.W  #8,D4
%ŸVp
                SUBQ.W  #QUICK,D4
%ŸVp
                SUBQ.W  #QUICK,A4
%ŸVp
                SUBQ.W  #QUICK,(A4)
%ŸVp
                SUBQ.W  #QUICK,(A4)+
%ŸVp
                SUBQ.W  #QUICK,-(A4)
%ŸVp
                SUBQ.W  #QUICK,ADDRESS2(A4)
%ŸVp
                SUBQ.W  #QUICK,OFFSET2(A4,A6)
%ŸVp
                SUBQ.W  #QUICK,OFFSET2(A4,A6.L)
%ŸVp
                SUBQ.W  #QUICK,OFFSET2(A4,D6)
%ŸVp
                SUBQ.W  #QUICK,OFFSET2(A4,D6.L)


%ŸVp
                SUBA.W  A4,A6
%ŸVp
                SUBA.W  (A4)+,A6
%ŸVp
                SUBA.W  ADDRESS2(A4),A6
%ŸVp
                SUBA.W  OFFSET2(A4,A6.L),A6
%ŸVp
                SUBA.W  OFFSET2(A4,D6.L),A6
                SUBA.W  ADDRESS1,A6
%ŸVp
                SUBA.W  OFFSET1(PC),A6
%ŸVp
                SUBA.W  OFFSET1(PC,A6.L),A6
%ŸVp
                SUBA.W  OFFSET1(PC,D6.L),A6
                SUBA.W  #10,A6
%ŸVp

SUBI.W:         SUBI.W  #10,D4
%ŸVp
                SUBI.W  #10,(A4)
%ŸVp
                SUBI.W  #ADDRESS1,(A4)+
                SUBI.W  #ADDRESS1,-(A4)
%ŸVp
                SUBI.W  #ADDRESS1,OFFSET2(A4,A6)
%ŸVp
                SUBI.W  #ADDRESS1,OFFSET2(A4,D6)
%ŸVp
                SUBI.W  #ADDRESS1,ADDRESS1
%ŸVp


SUB.B:          SUB.B   D4,D6
                SUB.B   (A4),D6
%ŸVp
                SUB.B   -(A4),D6
%ŸVp
                SUB.B   OFFSET2(A4,A5),D6
%ŸVp
                SUB.B   OFFSET2(A4,D5),D6
%ŸVp
                SUB.B   ADDRESS1,D6
                SUB.B   ADDRESS2,D6
%ŸVp
                SUB.B   OFFSET1(PC,A5),D6
%ŸVp
                SUB.B   OFFSET1(PC,D5),D6
%ŸVp
                SUB.B   #10,D6
                SUB.B   #DATA8,D6

%ŸVp
                SUB.B   D4,(A6)
%ŸVp
                SUB.B   D4,-(A6)
%ŸVp
                SUB.B   D4,OFFSET1(A6,A4)
%ŸVp
                SUB.B   D4,OFFSET1(A6,D4)
%ŸVp
                SUB.B   D4,ADDRESS1
                SUB.B   D4,ADDRESS2

%ŸVp
                SUB.B   #DATA8,D4
%ŸVp
                SUB.B   #DATA8,(A4)
%ŸVp
                SUB.B   #DATA8,-(A4)
%ŸVp
                SUB.B   #DATA8,OFFSET2(A4,A6)
%ŸVp
                SUB.B   #DATA8,OFFSET2(A4,D6)
%ŸVp
                SUB.B   #DATA8,ADDRESS1
                SUB.B   #DATA8,ADDRESS2

%ŸVp
                SUBQ.B  #1,D4
%ŸVp
                SUBQ.B  #8,D4
%ŸVp
                SUBQ.B  #QUICK,D4
%ŸVp
                SUBQ.B  #QUICK,(A4)
%ŸVp
                SUBQ.B  #QUICK,(A4)+
%ŸVp
                SUBQ.B  #QUICK,-(A4)
%ŸVp
                SUBQ.B  #QUICK,ADDRESS2(A4)
%ŸVp
                SUBQ.B  #QUICK,OFFSET2(A4,A6)
%ŸVp
                SUBQ.B  #QUICK,OFFSET2(A4,A6.L)
%ŸVp
                SUBQ.B  #QUICK,OFFSET2(A4,D6)
%ŸVp
                SUBQ.B  #QUICK,OFFSET2(A4,D6.L)

%ŸVp
                SUBI.B  #DATA8,D4
%ŸVp
                SUBI.B  #DATA8,(A4)
%ŸVp
                SUBI.B  #DATA8,-(A4)
%ŸVp
                SUBI.B  #DATA8,OFFSET2(A4,A6)
%ŸVp
                SUBI.B  #DATA8,OFFSET2(A4,D6)
%ŸVp
                SUBI.B  #DATA8,ADDRESS1
                SUBI.B  #DATA8,ADDRESS2

SUB.L:          SUB.L   D4,D6
                SUB.L   A4,D6
%ŸVp
                SUB.L   (A4)+,D6
%ŸVp
                SUB.L   ADDRESS2(A4),D6
%ŸVp
                SUB.L   OFFSET2(A4,A5.L),D6
%ŸVp
                SUB.L   OFFSET2(A4,D5.L),D6
                SUB.L   ADDRESS1,D6
%ŸVp
                SUB.L   OFFSET1(PC),D6
%ŸVp
                SUB.L   OFFSET1(PC,A5.L),D6
%ŸVp
                SUB.L   OFFSET1(PC,D5.L),D6
                SUB.L   #10,D6
%ŸVp

                SUB.L   D4,D6
%ŸVp
                SUB.L   D4,(A6)
%ŸVp
                SUB.L   D4,-(A6)
%ŸVp
                SUB.L   D4,OFFSET1(A6,A4)
%ŸVp
                SUB.L   D4,OFFSET1(A6,D4)
%ŸVp
                SUB.L   D4,ADDRESS1
                SUB.L   D4,ADDRESS2

%ŸVp
                SUB.L   A4,A6
%ŸVp
                SUB.L   (A4)+,A6
%ŸVp
                SUB.L   ADDRESS2(A4),A6
%ŸVp
                SUB.L   OFFSET2(A4,A6.L),A6
%ŸVp
                SUB.L   OFFSET2(A4,D6.L),A6
                SUB.L   ADDRESS1,A6
%ŸVp
                SUB.L   OFFSET1(PC),A6
%ŸVp
                SUB.L   OFFSET1(PC,A6.L),A6
%ŸVp
                SUB.L   OFFSET1(PC,D6.L),A6
                SUB.L   #10,A6
%ŸVp

                SUB.L   #10,D4
%ŸVp
                SUB.L   #10,(A4)
%ŸVp
                SUB.L   #DATA32,(A4)+
%ŸVp
                SUB.L   #DATA32,ADDRESS2(A4)
%ŸVp
                SUB.L   #DATA32,OFFSET2(A4,A6.L)
%ŸVp
                SUB.L   #DATA32,OFFSET2(A4,D6.L)
%ŸVp
                SUB.L   #DATA32,ADDRESS2

%ŸVp
                SUBQ.L  #1,D4
%ŸVp
                SUBQ.L  #8,D4
%ŸVp
                SUBQ.L  #QUICK,D4
%ŸVp
                SUBQ.L  #QUICK,A4
%ŸVp
                SUBQ.L  #QUICK,(A4)
%ŸVp
                SUBQ.L  #QUICK,(A4)+
%ŸVp
                SUBQ.L  #QUICK,-(A4)
%ŸVp
                SUBQ.L  #QUICK,ADDRESS2(A4)
%ŸVp
                SUBQ.L  #QUICK,OFFSET2(A4,A6)
%ŸVp
                SUBQ.L  #QUICK,OFFSET2(A4,A6.L)
%ŸVp
                SUBQ.L  #QUICK,OFFSET2(A4,D6)
%ŸVp
                SUBQ.L  #QUICK,OFFSET2(A4,D6.L)

%ŸVp
                SUBA.L  A4,A6
%ŸVp
                SUBA.L  (A4)+,A6
%ŸVp
                SUBA.L  ADDRESS2(A4),A6
%ŸVp
                SUBA.L  OFFSET2(A4,A6.L),A6
%ŸVp
                SUBA.L  OFFSET2(A4,D6.L),A6
                SUBA.L  ADDRESS1,A6
%ŸVp
                SUBA.L  OFFSET1(PC),A6
%ŸVp
                SUBA.L  OFFSET1(PC,A6.L),A6
%ŸVp
                SUBA.L  OFFSET1(PC,D6.L),A6
                SUBA.L  #10,A6
%ŸVp

SUBI.L:         SUBI.L  #10,D4
%ŸVp
                SUBI.L  #10,(A4)
%ŸVp
                SUBI.L  #DATA32,(A4)+
%ŸVp
                SUBI.L  #DATA32,ADDRESS2(A4)
%ŸVp
                SUBI.L  #DATA32,OFFSET2(A4,A6.L)
%ŸVp
                SUBI.L  #DATA32,OFFSET2(A4,D6.L)
%ŸVp
                SUBI.L  #DATA32,ADDRESS2


%ŸVp
                SUBX    D7,D0
%ŸVp
                SUBX.W  D0,D7
%ŸVp
                SUBX.W  D4,D6
%ŸVp
                SUBX    -(A7),-(A0)
%ŸVp
                SUBX.W  -(A0),-(A7)
%ŸVp
                SUBX.W  -(A4),-(A6)

%ŸVp
                SUBX.B  D7,D0
%ŸVp
                SUBX.B  -(A0),-(A7)
%ŸVp
                SUBX.B  -(A4),-(A6)

%ŸVp
                SUBX.L  D7,D0
%ŸVp
                SUBX.L  -(A0),-(A7)
%ŸVp
                SUBX.L  -(A4),-(A6)


%ŸVp
                SWAP    D7


%ŸVp
                TAS     D6
%ŸVp
                TAS     (A4)+
%ŸVp
                TAS     ADDRESS2(A4)
%ŸVp
                TAS     OFFSET2(A4,A5.L)
%ŸVp
                TAS     OFFSET2(A4,D5.L)
%ŸVp
                TAS     ADDRESS2

%ŸVp
                TAS.B   D4
%ŸVp
                TAS.B   (A4)+
%ŸVp
                TAS.B   ADDRESS2(A4)
%ŸVp
                TAS.B   OFFSET2(A4,A5.L)
                TAS.B   OFFSET2(A4,D5)
%ŸVp
                TAS.B   ADDRESS1
%ŸVp


TRAP:           TRAP    #0
%ŸVp


TRAPV:          TRAPV


%ŸVp
                TST     D6
%ŸVp
                TST     (A4)+
%ŸVp
                TST     ADDRESS2(A4)
%ŸVp
                TST     OFFSET2(A4,A5.L)
%ŸVp
                TST     OFFSET2(A4,D5.L)
%ŸVp
                TST     ADDRESS2

%ŸVp
                TST.B   D4
%ŸVp
                TST.B   (A4)+
%ŸVp
                TST.B   ADDRESS2(A4)
%ŸVp
                TST.B   OFFSET2(A4,A5.L)
                TST.B   OFFSET2(A4,D5)
%ŸVp
                TST.B   ADDRESS1
%ŸVp

TST.W:          TST.W   D4
%ŸVp
                TST.W   (A4)
%ŸVp
                TST.W   -(A4)
%ŸVp
                TST.W   OFFSET2(A4,A5)
%ŸVp
                TST.W   OFFSET2(A4,D5)
%ŸVp
                TST.W   ADDRESS1
%ŸVp

TST.L:          TST.L   D4
%ŸVp
                TST.L   (A4)
%ŸVp
                TST.L   -(A4)
%ŸVp
                TST.L   OFFSET2(A4,A5)
%ŸVp
                TST.L   OFFSET2(A4,D5)
%ŸVp
                TST.L   ADDRESS1
%ŸVp


UNLK:           UNLK    A0
%ŸVp

ADDRESS2:       .EQUAL  $


%ŸVp
