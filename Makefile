CC=gcc
CFLAGS=-g -Wall
OBJFLAGS=-g -Wall -c
FLEX=flex
BISON=bison
HFLAG=-d

k0: main.o lex.yy.o lex.o k0gram.tab.o tree.o dot.o symTab.o semanticBuild.o type.o typeHelpers.o typeCheck.o symTabHelper.o importHandler.o
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

semanticBuild.o: semanticBuild.c type.h symTab.h tree.h symNonTerminals.h typeHelpers.h typeCheck.h
	$(CC) $(OBJFLAGS) $<

typeHelpers.o: typeHelpers.c typeHelpers.h type.h
	$(CC) $(OBJFLAGS) $<

typeCheck.o: typeCheck.c typeCheck.h typeHelpers.h type.h
	$(CC) $(OBJFLAGS) $<

symTabHelper.o: symTabHelper.c symTabHelper.h symTab.h tree.h type.h typeHelpers.h
	$(CC) $(OBJFLAGS) $<

importHandler.o: importHandler.c importHandler.h symTab.h tree.h type.h typeHelpers.h symNonTerminals.h lex.h k0gram.tab.h
	$(CC) $(OBJFLAGS) $<

dotify:
	dot -Tpng dotfile.dot > dotfile.png

clean:
	rm lex.yy.c *.o k0 *.tab.* *.h.gch *.dot *.png

zip: *.c *.h k0gram.y kotlex.l Makefile README TestCasesOld/ tests/
	zip -r hw5.zip $^