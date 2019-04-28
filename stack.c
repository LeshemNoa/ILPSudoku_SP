#include <stdlib.h>

#include "stack.h"

void initStack(Stack* stack) { 
    if (stack != NULL) {
        initList(&(stack->list));
    }
}

bool isStackEmpty(const Stack* stack) {
    return isListEmpty(&(stack->list));
}

bool pushStack(Stack* stack, void* new_data) {
    return pushList(&(stack->list), new_data);
}

void* popStack(Stack* stack) {
    return popList(&(stack->list)); 
}

int getStackSize(const Stack* stack) {
    return getListSize(&(stack->list));
}

const void* peekStack(const Stack* stack) {
    if (isStackEmpty(stack)) {
        return NULL;
    }
    
    return getNodeData(getListHead(&(stack->list)));
}
