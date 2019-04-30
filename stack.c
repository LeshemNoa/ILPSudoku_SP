/**
 * STACK Summary:
 *
 * An implementation of a stack based on a doubly linked list.
 * The API is almost exacly identical to that of the linked list.
 */

#include <stdlib.h>

#include "stack.h"

void initStack(Stack* stack) { 
    initList(stack);
}

bool isStackEmpty(const Stack* stack) {
    return isListEmpty(stack);
}

bool pushStack(Stack* stack, void* new_data) {
    return pushList(stack, new_data);
}

void* popStack(Stack* stack) {
    return popList(stack); 
}

int getStackSize(const Stack* stack) {
    return getListSize(stack);
}

const void* peekStack(const Stack* stack) {
    if (isStackEmpty(stack)) {
        return NULL;
    }

    return getNodeData(getListHead(stack));
}
