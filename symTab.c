#include "symTab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct symTab *rootScope;


/**
 * @brief Inserts an element into a symbol table.
 * @param table 
 * @param elem 
 * @param type 
 * @param func
 * @return int 
 */
struct symTab *addSymTab(struct symTab *table, char *elem, struct tree *type, int func){
    int bucket = hash(elem);
    struct symEntry *temp = table->buckets[bucket];

    // No entry insert it!
    if(temp == NULL){
        
        table->buckets[bucket] = createEntry(table, elem, type, func);
        temp = table->buckets[bucket];

        if(func == FUNCTION){
            return temp->scope;
        }
        // Perhaps this is the danger?
        return NULL;
    }


    // Find the first invalid next entry and insert it there.
    for(;temp->next != NULL; temp = temp->next);
    temp->next = createEntry(table, elem, type, func);
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
struct symEntry *createEntry(struct symTab *table, char *elem, struct tree *type, int func){
    struct symEntry *temp = malloc(sizeof(struct symEntry));
    temp->type = type;
    temp->scope = NULL;
    temp->name = elem;
    temp->func = func;
    if(func == FUNCTION){
        temp->scope = createTable(table, temp->name);
    }
    temp->next = NULL;

    return temp;
}


/**
 * @brief Checks if an element exists within the symbol table.
 * 
 * @param elem 
 * @param table 
 * @return struct symEntry* 
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
struct symTab *createTable(struct symTab *parent, char *name){
    struct symTab *table = malloc(sizeof(struct symTab));

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
        freeEntry(table->buckets[i]);
        free(table->buckets[i]);
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
    struct symEntry *temp = e;
    while(e != NULL){
        temp = e;
        if(temp->func == FUNCTION){
            freeTable(e->scope);
        }
        e = e->next;
        // TODO: Figure out what in here is causing the problem.
        // free(temp);
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
    // TODO: fix name bug
    printf("-- symbol table for %s --\n", table->name);
    for(int i = 0; i < SYMBUCKETS; i++){
        temp = table->buckets[i];
        if (temp != NULL){
            for(; temp != NULL;){
                printf("%s\n", temp->name);
                temp = temp->next;
            }
        }
    }
    
    for(int i = 0; i < SYMBUCKETS; i++){
        temp = table->buckets[i];
        // printf("%p %d\n", temp); 
        if (temp != NULL && temp->func == FUNCTION){
            
            printTable(temp->scope);
        }
    }

    return 0;
}

