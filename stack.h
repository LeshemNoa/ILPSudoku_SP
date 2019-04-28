#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct StackNode StackNode;

typedef struct Stack
{
    StackNode* head;
    int size;
} Stack;

void initStack(Stack* stack);
bool isStackEmpty(const Stack* stack);
bool pushStack(Stack* stack, void* new_data);
void* popStack(Stack* stack);
int getStackSize(const Stack* stack);
const void* peekStack(const Stack* stack);

#endif /* STACK_H */
