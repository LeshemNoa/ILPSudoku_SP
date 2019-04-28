#include <stdlib.h>

#include "stack.h"

/* Note: Nodes and data are dynamically allocated while stacks aren't */
struct StackNode {
    void* data;
    struct StackNode* next;
};

void initStack(Stack* stack) { 
    if (stack != NULL) {
        stack->head = NULL;
        stack->size = 0;
    }
}

bool isStackEmpty(const Stack* stack) {
    return (stack->size == 0);
}

StackNode* createNewStackNode(void* new_data) {
    StackNode* new_node;
    new_node = (StackNode*) calloc(1, sizeof(StackNode));
    if (new_node != NULL) {
        new_node->data = new_data;
        new_node->next = NULL;
    }
    return new_node;
}

bool pushStack(Stack* stack, void* new_data) {
    StackNode* new_node = createNewStackNode(new_data);
    if (new_node == NULL) { return false; }

    new_node->next = stack->head;
    stack->head = new_node;
    stack->size++;
    return true;
}

void* popStack(Stack* stack) {
    void* data;
    StackNode* old_head = NULL;

    if (isStackEmpty(stack)) { 
        return NULL;
    }

    old_head = stack->head;
    data = old_head->data;
    stack->head = old_head->next;
    free(old_head);
    stack->size--;
    return data;   
}

int getStackSize(const Stack* stack) {
    return stack->size;
}

const void* peekStack(const Stack* stack) {
    if (isStackEmpty(stack)) {
        return NULL;
    }

    return stack->head->data;
}
