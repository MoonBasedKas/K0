#ifndef IMPORT_HANDLER_H
#define IMPORT_HANDLER_H

#include "symTab.h"
#include "tree.h"
#include "type.h"

void processImport(struct tree *importIdNode, struct symTab *rootScope);
void parseImportList(struct tree* importListNode, struct symTab* rootScope);

#endif