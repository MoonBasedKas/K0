#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct symTab{
    char *name; 
    struct symTab *parent;
    struct symEntry **buckets;
};

struct symEntry{
    char *name;
    struct tree *type;
    struct symTab *scope;  
    struct symEntry *next;
    int func;
};

enum types{
    FUNCTION = 1,
    VARIABLE = 0
};

#define SYMBUCKETS 251

extern struct symTab *rootScope;

struct symTab *addSymTab(struct symTab *table, char *elem, struct tree *type, int func);
struct symEntry *createEntry(struct symTab *table, char *elem, struct tree *type, int func);
struct symTab *createTable(struct symTab *parent, char *name);
int hash(char *elem);
struct symEntry *contains(struct symTab *table, char *elem);
int freeTable(struct symTab *table);
int freeEntry(struct symEntry *e);
int printTable(struct symTab *table);