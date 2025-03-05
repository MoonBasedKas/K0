#include <stdlib.h>
#include <stdio.h>

struct symTab{
    char *s; // uhhh nice?
    struct symTab *parent;
    struct SymEntry **buckets;
};

struct symEntry{
    char *name;
    void *type;
    struct symTab *parent;  
    struct symEntry *next;
};

#define SYMBUCKETS 251
#define SYMTSIZE sizeof(sturct SymEntry *) * SYMBUCKETS

struct symTab *rootScope;

int addSymTab(struct symTable *table, char *elem, void *type);

struct symTab *createTable(struct symTab *parent);
int hash(char *elem);
struct symEntry *contains(char *elem, struct symTab *table);