CC=gcc
CFLAGS=-g -Wall
OBJFLAGS=-g -Wall -c
FLEX=flex
BISON=bison
HFLAG=-d
EXEC=k0

k0: main.o lex.yy.o lex.o k0gram.tab.o tree.o dot.o symTab.o typeDeclaration.o type.o typeHelpers.o typeCheck.o symTabHelper.o errorHandling.o tac.o icode.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c lex.h k0gram.tab.h dot.h tree.h symTab.h
	$(CC) $(OBJFLAGS) $<

lex.o: lex.c lex.h
	$(CC) $(OBJFLAGS) $<

lex.yy.o: lex.yy.c lex.h k0gram.tab.h tree.h
	$(CC) $(OBJFLAGS) $<

lex.yy.c: kotlex.l
	$(FLEX) $^

k0gram.tab.o: k0gram.tab.c k0gram.tab.h
	$(CC) $(OBJFLAGS) $<

k0gram.tab.c: k0gram.y
	$(BISON) $^

k0gram.tab.h: k0gram.y
	$(BISON) $(HFLAG) $^

tree.o: tree.c tree.h lex.h k0gram.tab.h symTab.h symNonTerminals.h type.h typeHelpers.h
	$(CC) $(OBJFLAGS) $<

dot.o: dot.c dot.h
	$(CC) $(OBJFLAGS) $<

symTab.o: symTab.c symTab.h symTabHelper.h
	$(CC) $(OBJFLAGS) $<

type.o: type.c type.h typeHelpers.h
	$(CC) $(OBJFLAGS) $<

typeDeclaration.o: typeDeclaration.c typeDeclaration.h type.h symTab.h tree.h symNonTerminals.h typeHelpers.h typeCheck.h
	$(CC) $(OBJFLAGS) $<

typeHelpers.o: typeHelpers.c typeHelpers.h type.h
	$(CC) $(OBJFLAGS) $<

typeCheck.o: typeCheck.c typeCheck.h typeHelpers.h type.h
	$(CC) $(OBJFLAGS) $<

symTabHelper.o: symTabHelper.c symTabHelper.h symTab.h tree.h type.h typeHelpers.h
	$(CC) $(OBJFLAGS) $<

errorHandling.o: errorHandling.c errorHandling.h
	$(CC) $(OBJFLAGS) $<

tac.o: tac.c symTab.h tac.h 
	$(CC) $(OBJFLAGS) $<

icode.o: icode.c icode.h 
	$(CC) $(OBJFLAGS) $<

dotify:
	dot -Tpng dotfile.dot > dotfile.png

clean:
	rm lex.yy.c *.o k0 *.tab.* *.h.gch *.dot *.png lab9 *.ic

valgrind: k0
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./k0 TestCasesOld/ktstuff.kt

zip: *.c *.h k0gram.y kotlex.l Makefile README TestCasesOld/ tests/ gdb.sh
	zip -r hw6.zip $^