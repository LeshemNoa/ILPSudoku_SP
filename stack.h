/**
 * An implementation of a stack based on a doubly linked list.
 * The API is almost exacly identical to that of the linked list.
 * 
 * initStack(Stack* stack) - Initialize the provided stack.
 * isStackEmpty - Checks if the stack is empty.
 * pushStack - Push a new item to the stack.
 * popStack(Stack* stack) - Pop the top element of the stack.
 * getStackSize - Get the number of elements in the stack.
 * peekStack - Get the top element in the stack without popping it.
 */

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "linked_list.h"


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
