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
    struct tree *tableType; // Kass I changed this from "type" to "tableType" - Erik
    struct symTab *scope;
    struct symEntry *next;
    int func;
    struct typeInfo *type;
};

enum types{
    FUNCTION = 1,
    PACKAGE,
    VARIABLE = 0
};

#define SYMBUCKETS 251

extern struct symTab *rootScope;

struct symTab *addSymTab(struct symTab *table, char *elem, struct tree *type, int func);
struct symEntry *createEntry(struct symTab *table, char *elem, struct tree *type, int func);
struct symTab *createTable(struct symTab *parent, char *name, int type);
int hash(char *elem);
struct symEntry *contains(struct symTab *table, char *elem);
int freeTable(struct symTab *table);
int freeEntry(struct symEntry *e);
int printTable(struct symTab *table);
char *getTableType(int tableType);

#endif