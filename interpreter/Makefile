CC = gcc
LIB = 
CFLAGS = -I ../include

roboticFinger: lex.yy.c rf.tab.c
	$(CC) $(CFLAGS) -g -w rflib.h lex.yy.c rf.tab.c -o roboticFinger 

rf.tab.c: robotic_finger.y
	yacc -b rf -d robotic_finger.y

lex.yy.c: rf.tab.c robotic_finger.l
	lex robotic_finger.l

clean: 
	rm -f lex.yy.c rf.tab.c rf.tab.h roboticFinger
