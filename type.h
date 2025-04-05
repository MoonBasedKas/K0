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

/* Basic types */
typedef enum {
    FIRST_TYPE = 1000000,
    NULL_TYPE,
    BYTE_TYPE,
    INT_TYPE,
    DOUBLE_TYPE,
    BOOL_TYPE,
    CHAR_TYPE,
    STRING_TYPE,
    ARRAY_TYPE,
    FUNCTION_TYPE,
    ANY_TYPE, //Not sure if we need this catchall type
    LAST_TYPE
} basicType;

enum {
    notNullable = 0,
    nullable,
    indexNullable,
    squareNullable,
};

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

extern typePtr integerType_ptr;
extern typePtr byteType_ptr;
extern typePtr doubleType_ptr;
extern typePtr booleanType_ptr;
extern typePtr charType_ptr;
extern typePtr stringType_ptr;
extern typePtr nullType_ptr;

#endif
