#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "linked_list.h"

typedef struct Stack
{
    List list;
} Stack;

void initStack(Stack* stack);
bool isStackEmpty(const Stack* stack);
bool pushStack(Stack* stack, void* new_data);
void* popStack(Stack* stack);
int getStackSize(const Stack* stack);
const void* peekStack(const Stack* stack);

#endif /* STACK_H */
