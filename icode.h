#ifndef ICCODE_H
#define ICCODE_H
#include "tree.h"

void buildICode(struct tree *node);
void localAddr(struct tree *node);
void basicBlocks(struct tree *node);
void assignFirst(struct tree *node);
void assignFollow(struct tree *node);
void assignOnTrueFalse(struct tree *node);
void control(struct tree *node);
void debugICode(char *string, struct tree *node);
#endif
