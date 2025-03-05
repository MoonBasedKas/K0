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
    int type;
    struct symTab *scope;  
    struct symEntry *next;
};

enum types{
    FUNCTION = 1,
    VARIABLE
};

#define SYMBUCKETS 251

extern struct symTab *rootScope;

struct symTab *addSymTab(struct symTab *table, char *elem, int type);
struct symEntry *createEntry(struct symTab *table, char *elem, int type);
struct symTab *createTable(struct symTab *parent);
int hash(char *elem);
struct symEntry *contains(struct symTab *table, char *elem);