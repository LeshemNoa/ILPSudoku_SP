#include <stdlib.h>

#include "rec_stack.h"


typedef struct {
    int curRow;
    int curCol;
} CallFrame;

void initStack(Stack* stack) {
    initList(&(stack->list));
}

bool pushStack(Stack* stack, int curRow, int curCol) {
    CallFrame* frame = (CallFrame*)calloc(1, sizeof(CallFrame));
    if (frame == NULL) {
        return false;
    }

    frame->curRow = curRow;
    frame->curCol = curCol;
    return push(&(stack->list), frame);
}

bool peekStack(Stack* stack, int* curRow, int* curCol) {
    CallFrame* frame;
    if (isEmptyStack(stack)) {
        return false;
    }
    
    frame = (CallFrame*)stack->list.head->data;
    *curRow = frame->curRow;
    *curCol = frame->curCol;
    return true;
}

bool popStack(Stack* stack) {
    CallFrame* frame = (CallFrame*) pop(&stack->list);
    if (frame == NULL) {
        return false;
    }

    free(frame);
    return true;
}

bool isEmptyStack(Stack* stack) {
    return isEmpty(&stack->list);
}


