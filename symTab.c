#include "symTab.h"

int add(struct symTable *table, char *elem, void *type){
    int bucket = hash(elem);
    struct symEntry *temp = *(table->buckets + sizeof(struct symEntry *) * bucket);
    




    

    return 0;
}


/**
 * @brief Checks if an element exists within the symbol table.
 * 
 * @param elem 
 * @param table 
 * @return struct symEntry* 
 */
struct symEntry *contains(char *elem, struct symTab *table){

    int bucket = hash(elem);
    struct symEntry *temp = *(table->buckets + sizeof(struct symEntry *) * bucket);

    if (!temp) return NULL; // Bucket does not exist

    for(; temp != NULL; temp = temp->next){
        if(!strcmp(elem, temp->name)) return 0; // HIT
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
    int val;
    for(int i = 0; i < strlen(e); i++){
        val += *(e + i);
    }
    val *= 37; // Straight outta crypto.
    return val % SYMBUCKETS;
}

/**
 * @brief Create a Table object
 * 
 * @return struct symTab* 
 */
struct symTab *createTable(){
    struct symTab *table = malloc(sizeof(struct symTab));
    table->buckets = allocateBuckets(table);

    return table;
}


/**
 * @brief Allocates buckets due to lack of callocing trust.
 * 
 * @param table 
 * @return int; is it successful
 */
int allocateBuckets(struct symTab *table){

    for(int i = 0; i < SYMBUCKETS; i++){

        *(table->buckets + i) = malloc(sizeof(struct symEntry *));



        if(!*(table->buckets + i)) return 1; // bucket failed.

        *(table->buckets + i) = NULL;
    }

    return 0;   
}