#ifndef REC_STACK_H
#define REC_STACK_H

#include <stdbool.h>

#include "linked_list.h" /* CR: again, we are graded on modularity. We should have a generic stack module, included by this module (this module certainly does not need the whole power of a list */

typedef struct {
    List list;
} Stack;

void initStack(Stack* stack);

bool pushStack(Stack* stack, int curRow, int curCol);

bool popStack(Stack* stack);

bool peekStack(const Stack* stack, int* curRow, int* curCol);

bool isEmptyStack(const Stack* stack);

#endif /* REC_STACK_H */
