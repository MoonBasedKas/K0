#include "symTab.h"
#include "type.h"
#include "typeHelpers.h"
#include "symTabHelper.h"
#include "errorHandling.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct symTab *rootScope;
extern int symError;
/**
 * @brief Inserts an element into a symbol table.
 * @param table
 * @param elem
 * @param type
 * @param func
 * @return int
 */
struct symTab *addSymTab(struct symTab *table, char *elem, int func){
    int bucket = hash(elem);
    struct symEntry *temp = table->buckets[bucket];

    // Check for redeclaration in the same scope
    struct symEntry *checker = temp;
    for (; checker != NULL; checker = checker->next) {
        if (strcmp(checker->name, elem) == 0) {
            fprintf(stderr, "ERROR: redeclaration of '%s' in the same scope.\n", elem);
            symError = 3;
        }
    }

    // No entry insert it!
    if(temp == NULL){

        table->buckets[bucket] = createEntry(table, elem, func);
        temp = table->buckets[bucket];

        if(func == FUNCTION || func == PACKAGE){
            return temp->scope;
        }
        // Perhaps this is the danger?
        return NULL;
    }

    // Find the first invalid next entry and insert it there.
    for(;temp->next != NULL; temp = temp->next);
    temp->next = createEntry(table, elem, func);
    if (func == FUNCTION){
        return temp->next->scope;
    }

    return NULL;
}

/**
 * @brief Create a Entry object
 *
 * @param table
 * @param elem
 * @param type
 * @return struct symEntry*
 */
struct symEntry *createEntry(struct symTab *table, char *elem, int func){
    struct symEntry *temp = malloc(sizeof(struct symEntry));
    temp->type = NULL; // To be assigned later.
    temp->scope = NULL;
    temp->name = elem;
    temp->func = func;
    temp->mutable = 1; // mutable
    temp->nullable = notNullable; // Not nullable
    if(func == FUNCTION || func == PACKAGE){
        temp->scope = createTable(table, temp->name, func);
    }
    temp->next = NULL;

    return temp;
}

/**
 * @brief Checks if an element exists within the symbol table.
 *
 * @param elem
 * @param table
 * @return struct symEntry* NULL if not there.
 */
struct symEntry *contains(struct symTab *table, char *elem){

    int bucket = hash(elem);
    struct symEntry *temp = table->buckets[bucket];

    if (!temp) return NULL; // Bucket does not exist
    for(; temp != NULL; temp = temp->next){

        if(strcmp(elem, temp->name) == 0) {
            return temp; // HIT
        }
    }

    return NULL; //No...
}


/**
 * @brief Computes a quick hash for our function.
 *
 * @param e
 * @return int
 */
int hash(char *e){
    int val = 0;
    for(int i = 0; i < strlen(e); i++){
        val += *(e + i);
    }
    val *= 37; // Straight outta crypto.
    if(val < 0) val = -(val);
    return val % SYMBUCKETS;
}


/**
 * @brief Create a Table object
 *
 * @return struct symTab*
 */
struct symTab *createTable(struct symTab *parent, char *name, int type){
    struct symTab *table = malloc(sizeof(struct symTab));
    table->tableType = type;
    table->buckets = calloc(SYMBUCKETS, sizeof(struct symEntry*));
    table->name = name;
    table->parent = parent;
    return table;
}

/**
 * @brief Frees the table
 *
 * @param table
 * @return int
 */
int freeTable(struct symTab *table){
    for(int i = 0; i < SYMBUCKETS; i++){
        if(table->buckets[i] != NULL){
            freeEntry(table->buckets[i]);
        }
    }
    free(table->buckets);
    free(table);
    return 0;
}


/**
 * @brief Frees each table entry.
 *
 * @param e
 * @return int
 */
int freeEntry(struct symEntry *e){
    struct symEntry *temp;
    while(e != NULL){
        temp = e;
        if(temp->func == FUNCTION || temp->func == PACKAGE){
            freeTable(temp->scope);
        }
        e = e->next;
        // TODO: Figure out what in here is causing the problem.
        free(temp);
    }
    return 0;
}

/**
 * @brief Prints out the entire symbol table reachable from a singular table.
 *
 * @param table
 * @return int
 */
int printTable(struct symTab *table){
    struct symEntry *temp;
    printf("--- symbol table for %s: %s ---\n", getTableType(table->tableType), table->name);
    for(int i = 0; i < SYMBUCKETS; i++){
        temp = table->buckets[i];
        if (temp != NULL){
            for(; temp != NULL;){
                printf("%s %s\n", temp->name, typeName(temp->type));
                temp = temp->next;
            }
        }
    }
    printf("---\n");

    for(int i = 0; i < SYMBUCKETS; i++){
        temp = table->buckets[i];
        // printf("%p %d\n", temp);
        if (temp != NULL && (temp->func == FUNCTION || temp->func == PACKAGE)){

            printTable(temp->scope);
        }
    }

    return 0;
}

/**
 * @brief Assigns an entry in a given symbol table its type.
 * 
 * @param table 
 * @param string 
 * @return int 
 */
int assignEntrytype(struct symTab *table, char *string, struct typeInfo *type){
    struct symEntry *entry = contains(table, string);
    if (!entry) return 1; // No entry
    entry->type = type;
    return 0; // Yay!
}


/**
 * @brief Makes a symtab entry nullable.
 * 
 * @param table 
 * @param string 
 * @param type 
 * @return int 
 */
int changeNullable(struct symTab *table, char *string, int mode){
    struct symEntry *entry = contains(table, string);
    if (!entry) return 1;
    entry->nullable = mode; 
    return 0;
}

/**
 * @brief Tells us if something is nullable.
 * 
 * @param table 
 * @param string 
 * @return int 
 */
int checkNullable(struct symTab *table, char *string){
    struct symEntry *entry = contains(table, string);
    if (!entry) return -1; // Bad
    return entry->nullable;
}


/**
 * @brief Tells us if something is mutable.
 * 
 * @param table 
 * @param string 
 * @return int 
 */
int checkMutable(struct symTab *table, char *string){
    struct symEntry *entry = contains(table, string);
    if (!entry) return -1; // Bad
    return entry->mutable;
}

/**
 * @brief Sets an entry to be immutable when something is done with val or const val.
 * 
 * @param table 
 * @param string 
 * @return int 
 */
int makeEntryNonMutable(struct symTab *table, char *string){
    struct symEntry *entry = contains(table, string);
    if (!entry) return 1;
    entry->mutable = 0; // Not mutable
    return 0;
}


int findNullTables(struct tree *root){
    if (root->table == NULL) printf("%s\n", root->symbolname);
    for(int i = 0; i < root->nkids; i++){
        findNullTables(root->kids[i]);
    }
    return 0;
}