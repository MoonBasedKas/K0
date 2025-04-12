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
    UNIT_TYPE, //Void
    RANGE_TYPE,
    LAST_TYPE
} basicType;

typedef enum {
    IMPORT_RETURN_INT,
    IMPORT_RETURN_DOUBLE,
    IMPORT_RETURN_BOOLEAN,
    IMPORT_RETURN_CHAR,
    IMPORT_RETURN_STRING,
    IMPORT_RETURN_UNIT // ??
} importValueType;

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
        struct rangeInfo {
            struct typeInfo *elemType;
            int until;
        } range;
        struct importInfo {
            importValueType returnType;
            int paramCount;
            importValueType paramTypes[2];
        } import;
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
extern typePtr unitType_ptr;
extern typePtr rangeType_ptr;

/* Global pointers for premade array types */
extern typePtr arrayIntegerType_ptr;
extern typePtr arrayByteType_ptr;
extern typePtr arrayDoubleType_ptr;
extern typePtr arrayBooleanType_ptr;
extern typePtr arrayCharType_ptr;
extern typePtr arrayStringType_ptr;
extern typePtr arrayAnyType_ptr;

/* Global pointers for premade range types */
extern typePtr rangeIntegerType_ptr;

/* Global pointers for return types */
extern typePtr returnIntegerType_ptr;
extern typePtr returnByteType_ptr;
extern typePtr returnDoubleType_ptr;
extern typePtr returnBooleanType_ptr;
extern typePtr returnCharType_ptr;
extern typePtr returnStringType_ptr;
extern typePtr returnUnitType_ptr;

#endif
