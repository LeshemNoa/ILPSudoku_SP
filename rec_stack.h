#ifndef REC_STACK_H
#define REC_STACK_H

#include <stdbool.h>

#include "linked_list.h"

typedef struct {
    List list;
} Stack;

void initStack(Stack* stack);

bool pushStack(Stack* stack, int curRow, int curCol);

bool popStack(Stack* stack);

bool peekStack(Stack* stack, int* curRow, int* curCol);

bool isEmptyStack(Stack* stack);

#endif /* REC_STACK_H */
