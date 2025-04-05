#ifndef symTab_H
#define symTAB_H


struct symTab{
    char *name;
    struct symTab *parent;
    struct symEntry **buckets;
    int tableType;
};

struct symEntry{
    char *name;
    struct symTab *scope;
    struct symEntry *next;
    int func;
    struct typeInfo *type;

    int mutable;
    int nullable;
};

enum types{
    FUNCTION = 1,
    PACKAGE,
    VARIABLE = 0
};

#define SYMBUCKETS 251

extern struct symTab *rootScope;

struct symTab *addSymTab(struct symTab *table, char *elem, int func);
struct symEntry *createEntry(struct symTab *table, char *elem, int func);
struct symTab *createTable(struct symTab *parent, char *name, int type);
int hash(char *elem);
struct symEntry *contains(struct symTab *table, char *elem);
int freeTable(struct symTab *table);
int freeEntry(struct symEntry *e);
int printTable(struct symTab *table);
char *getTableType(int type);
int grabTypes(struct symTab *table);
int addMathModule();
int assignEntrytype(struct symTab *table, char *string, struct typeInfo *type);
int makeEntryNullable(struct symTab *table, char *string);
int makeEntryNonMutable(struct symTab *table, char *string);
int checkNullable(struct symTab *table, char *string);
int checkMutable(struct symTab *table, char *string);
#endif