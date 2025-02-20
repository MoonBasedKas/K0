CC=gcc
CFLAGS=-g -Wall
OBJFLAGS=-g -Wall -c
FLEX=flex
BISON=bison
HFLAG=-d

k0: main.o lex.yy.o lex.o k0gram.tab.o tree.o

	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c lex.h k0gram.tab.h
	$(CC) $(OBJFLAGS) $^

lex.o: lex.c lex.h
	$(CC) $(OBJFLAGS) $^

lex.yy.o: lex.yy.c lex.h k0gram.tab.h tree.h
	$(CC) $(OBJFLAGS) $^

lex.yy.c: kotlex.l
	$(FLEX) $^

k0gram.tab.o: k0gram.tab.c k0gram.tab.h
	$(CC) $(OBJFLAGS) $^

k0gram.tab.c: k0gram.y
	$(BISON) $^

k0gram.tab.h: k0gram.y
	$(BISON) $(HFLAG) $^

tree.o: tree.c tree.h
	$(CC) $(OBJFLAGS) $^

clean:
	rm lex.yy.c *.o k0 *.tab.* *.h.gch