#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "linked_list.h"
/**
 * An implementation of a stack based on a doubly linked list.
 * The API is almost exacly identical to that of the linked list.
 */

typedef List Stack;

void initStack(Stack* stack);
bool isStackEmpty(const Stack* stack);
bool pushStack(Stack* stack, void* new_data);
void* popStack(Stack* stack);
int getStackSize(const Stack* stack);
/**
 * Get the top element in the stack without popping it.
 * 
 * @param stack         [in] The stack being examined
 * @return const void*  A read-only pointer to the data contained in the top stack element.
 */
const void* peekStack(const Stack* stack);

#endif /* STACK_H */
