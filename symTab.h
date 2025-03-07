#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct symTab{
    char *s; // uhhh nice?
    struct symTab *parent;
    struct symEntry **buckets;
};

struct symEntry{
    char *name;
    struct tree *type;
    struct symTab *scope;  
    struct symEntry *next;
};

enum types{
    FUNCTION = 1,
    VARIABLE
};

#define SYMBUCKETS 251

extern struct symTab *rootScope;

struct symTab *addSymTab(struct symTab *table, char *elem, struct tree *type, int func);
struct symEntry *createEntry(struct symTab *table, char *elem, struct tree *type, int func);
struct symTab *createTable(struct symTab *parent);
int hash(char *elem);
struct symEntry *contains(struct symTab *table, char *elem);