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
    UNIT_TYPE, //does anything else need to happen to add this? -Lily
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

extern typePtr integerType_ptr;
extern typePtr byteType_ptr;
extern typePtr doubleType_ptr;
extern typePtr booleanType_ptr;
extern typePtr charType_ptr;
extern typePtr stringType_ptr;
extern typePtr nullType_ptr;
extern typePtr unitType_ptr; //also this one plz erik

//ERIK make this point at shit
//plz
extern typePtr arrayIntegerType_ptr;
extern typePtr arrayByteType_ptr;
extern typePtr arrayDoubleType_ptr;
extern typePtr arrayBooleanType_ptr;
extern typePtr arrayCharType_ptr;
extern typePtr arrayStringType_ptr;
extern typePtr arrayAnyType_ptr;

extern typePtr returnIntegerType_ptr;
extern typePtr reuturnByteType_ptr;
extern typePtr returnDoubleType_ptr;
extern typePtr returnBooleanType_ptr;
extern typePtr returnCharType_ptr;
extern typePtr returnStringType_ptr;
extern typePtr returnUnitType_ptr;

#endif
