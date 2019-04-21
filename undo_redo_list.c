#include "undo_redo_list.h"
#include <stdlib.h>

/* Lists are not dynamically allocated anymore. Nodes and their data still are */

void initUndoRedo(UndoRedoList* moveList) { 
    if (moveList != NULL) {
        initList(&(moveList->list));
        moveList->current = moveList->list.head;
        moveList->numUndos = 0;
    }
}

void initMove(Move* move) {
    initList(&(move->singleCellMoves));
}

singleCellMove* createSingleCellMove(int prevVal, int newVal, int col, int row){
    singleCellMove* scMove = (singleCellMove*) malloc(sizeof(singleCellMove));
    if (scMove == NULL) { return NULL; }
    scMove->col = col;
    scMove->row = row;
    scMove->prevVal = prevVal;
    scMove->newVal = newVal;
    return scMove;
}

/* false is returned on memory allocation error */
bool addSingleCellMoveToMove(Move* move, int prevVal, int newVal, int col, int row) {
    singleCellMove* scMove = createSingleCellMove(prevVal, newVal, col, row);
    if (scMove == NULL) { return false; }
    return push(&(move->singleCellMoves), scMove);
}

/* returns false upon memory allocation error in push */
bool addNewMoveToList(UndoRedoList* moveList, Move* newMove) {
    while (moveList->list.head != moveList->current) {
        void* data = pop(&(moveList->list));
        free(data);
    }
    if (!push(&(moveList->list), newMove)) { return false; }
    moveList->current = moveList->list.head;
    moveList->numUndos = 0;
    return true;
}

Move* undoInList(UndoRedoList* moveList) {
    Move* moveToUndo;
    if (moveList->numUndos == moveList->list.size) {
        return NULL; /* no more moves to undo */
    }
    moveToUndo = getCurrent(moveList);
    if (moveList->current->next != NULL) {
        moveList->current = moveList->current->next;
    }
    moveList->numUndos++;
    return moveToUndo;
}

Move* redoInList(UndoRedoList* moveList) {
    Move* moveToRedo;
    if (moveList->numUndos == 0) {
        return NULL; /* cannot advance current further */
    }
    if (moveList->numUndos == moveList->list.size) {
        moveToRedo = getCurrent(moveList);
    } else {
        moveList->current = moveList->current->prev;
        moveToRedo = getCurrent(moveList);
    }
    moveList->numUndos--;
    return moveToRedo;
}

/* return true iff something's actually changed */
bool reset(UndoRedoList* moveList) { /* CR response: I'm leaving this here for now, I'll use it later */
    bool changed = false;
    while (undoInList(moveList)) {
        changed = true;
    }
    return changed;
}

Move* getCurrent(UndoRedoList* moveList) {
    return (Move*) moveList->current->data;
}
