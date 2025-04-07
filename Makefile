CC=gcc
CFLAGS=-g -Wall
OBJFLAGS=-g -Wall -c
FLEX=flex
BISON=bison
HFLAG=-d

k0: main.o lex.yy.o lex.o k0gram.tab.o tree.o dot.o symTab.o semanticBuild.o type.o typeHelpers.o symTabHelper.o
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

semanticBuild.o: semanticBuild.c type.h symTab.h tree.h symNonTerminals.h typeHelpers.h
	$(CC) $(OBJFLAGS) $<

typeHelpers.o: typeHelpers.c typeHelpers.h type.h
	$(CC) $(OBJFLAGS) $<

symTabHelper.o: symTabHelper.c symTabHelper.h symTab.h tree.h type.h typeHelpers.h
	$(CC) $(OBJFLAGS) $<

dotify:
	dot -Tpng dotfile.dot > dotfile.png

clean:
	rm lex.yy.c *.o k0 *.tab.* *.h.gch *.dot *.png

zip: main.c lex.c lex.h tree.c tree.h k0gram.y kotlex.l Makefile README dot.c dot.h TestCasesOld/ dotOutputs/ symTab.c symTab.h symNonTerminals.h type.c type.h semanticBuild.c semanticBuild.h typeHelpers.c typeHelpers.h tests/
	zip -r hw5.zip $^