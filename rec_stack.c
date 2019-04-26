#include <stdlib.h>

#include "rec_stack.h"

/* CR: if you feel like on top of a generic stack module we need also a rec_stacl module, sure (and it will be used by the BT_solver module). If not, all this could be in the BT_solver module */

typedef struct {
    int curRow;
    int curCol;
} CallFrame;

void initStack(Stack* stack) {
    initList(&(stack->list)); /* CR: you see, accessing the list part of the stack violates what this module needs to know! it need only call a pushList function of a stack module */
}

bool pushStack(Stack* stack, int curRow, int curCol) {
    CallFrame* frame = (CallFrame*)calloc(1, sizeof(CallFrame));
    if (frame == NULL) {
        return false;
    }
    frame->curRow = curRow;
    frame->curCol = curCol;
    if (!pushList(&(stack->list), frame)) {
        free(frame);
        return false;
    }

    return true;
}

bool peekStack(const Stack* stack, int* curRow, int* curCol) {
    CallFrame* frame;
    if (isEmptyStack(stack)) {
        return false;
    }
    
    frame = (CallFrame*)getNodeData(getListHead(&(stack->list)));
    *curRow = frame->curRow;
    *curCol = frame->curCol;
    return true;
}

bool popStack(Stack* stack) {
    CallFrame* frame = (CallFrame*) popList(&stack->list);
    if (frame == NULL) {
        return false;
    }

    free(frame);
    return true;
}

bool isEmptyStack(const Stack* stack) {
    return isListEmpty(&stack->list);
}


