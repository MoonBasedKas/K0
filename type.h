#ifndef TYPE_H
#define TYPE_H

#include "tree.h"

typedef struct param {
    char *name;
    struct typeInfo *type;
    struct param *next;
} *paramList;

struct field {
    char *name;
    struct type *elemType;
};

/* Basic types
Note that short, long, and float are aliases in Kotlin so.... ?

*/
typedef enum {
    FIRST_TYPE = 1000000,
    NULL_TYPE,
    BYTE_TYPE,
    INT_TYPE,
    SHORT_TYPE,
    LONG_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    BOOL_TYPE,
    CHAR_TYPE,
    STRING_TYPE,
    ARRAY_TYPE,
    FUNCTION_TYPE,
    ANY_TYPE, //Not sure if we need this catchall type
    LAST_TYPE
} basicType;

typedef struct typeInfo {
    int basicType;
    union {
        struct funcInfo {
            char *name;
            int defined;
            struct symTab *st;
            struct typeInfo *returnType;
            int numParams;
            struct param *parameters;
        } func;
        struct arrayInfo {
            int size; // -1 unkown size
            struct typeInfo *elemType;
        } array;
    } u;
} *typePtr;

typePtr alcType(int);
typePtr alcFuncType(struct tree *returnType, struct tree *params, struct symTab *st);
typePtr alcArrayType(struct tree *size, struct typeInfo *elemType);
char *typeName(typePtr t);
//int findType(struct tree *node);

extern struct symTab *globalSymTab;

extern typePtr int_typePtr;
extern typePtr byte_typePtr;
extern typePtr short_typePtr;
extern typePtr long_typePtr;
extern typePtr float_typePtr;
extern typePtr double_typePtr;
extern typePtr boolean_typePtr;
extern typePtr char_typePtr;
extern typePtr string_typePtr;
extern typePtr null_typePtr;

#endif
