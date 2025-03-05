#include "symTab.h"


struct symTab *rootScope;

/**
 * @brief Inserts an element into a symbol table.
 * 
 * @param table 
 * @param elem 
 * @param type 
 * @return int 
 */
int addSymTab(struct symTab *table, char *elem, int type){
    int bucket = hash(elem);
    struct symEntry *temp = *(table->buckets + sizeof(struct symEntry *) * bucket);

    
    if(!temp){
        
        *(table->buckets + sizeof(struct symEntry *) * bucket) = createEntry(table, elem, type);
        return 0;
    }


    for(;temp->next != NULL; temp = temp->next);
    temp->next = createEntry(table, elem, type);
    

    return 0;
}


struct symEntry *createEntry(struct symTab *table, char *elem, int type){
    struct symEntry *temp = malloc(sizeof(struct symEntry));
    temp->type = type;
    temp->scope = NULL;
    temp->name = elem;
    if(type == FUNCTION){
        temp->scope = createTable(table);
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
struct symEntry *contains(char *elem, struct symTab *table){

    int bucket = hash(elem);
    
    struct symEntry *temp = *(table->buckets + sizeof(struct symEntry *) * bucket);
    
    if (!temp) return NULL; // Bucket does not exist
    for(; temp != NULL; temp = temp->next){

        if(!strcmp(elem, temp->name)) return temp; // HIT
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
struct symTab *createTable(struct symTab *parent){
    struct symTab *table = malloc(sizeof(struct symTab));
    table->buckets = calloc(SYMBUCKETS, sizeof(struct symEntry));

    return table;
}



