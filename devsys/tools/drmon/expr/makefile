
LEX = flex #-F
YACC = bison -y -v

CC = bcc #-DYYDEBUG=1

expr: ytab.o lexyy.o
	$(CC) -o expr ytab.o lexyy.o

lexyy.o: 	lexyy.c y.tab.h
	
lexyy.o ytab.o:	expr.h

ytab.c ytab.h: expr.y
	$(YACC) -d expr.y

lexyy.c: expr.l
	$(LEX) expr.l



clean:
	-rm *.o
	-rm expr
