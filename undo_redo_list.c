#include <stdlib.h>

#include "undo_redo_list.h"

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

singleCellMove* createSingleCellMove(int prevVal, int newVal, int row, int col){
    singleCellMove* scMove = (singleCellMove*) calloc(1, sizeof(singleCellMove));
    if (scMove == NULL) { return NULL; }
    scMove->col = col;
    scMove->row = row;
    scMove->prevVal = prevVal;
    scMove->newVal = newVal;
    return scMove;
}

/* false is returned on memory allocation error */
bool addSingleCellMoveToMove(Move* move, int prevVal, int newVal, int row, int col) {
    singleCellMove* scMove = createSingleCellMove(prevVal, newVal, row, col);
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

bool canUndo(UndoRedoList* moveList) {
    return (moveList->numUndos != moveList->list.size);
}

bool canRedo(UndoRedoList* moveList) {
    return (moveList->numUndos != 0);
}

Move* undoInList(UndoRedoList* moveList) {
    Move* moveToUndo;
    if (!canUndo(moveList)) {
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
    if (!canRedo(moveList)) {
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

Move* getCurrent(UndoRedoList* moveList) {
    return (Move*) moveList->current->data;
}

void cleanupUndoRedoList(UndoRedoList* moveList) {
	while (!isEmpty(&(moveList->list))) {
		Move* move = (Move*)pop(&(moveList->list));
        while(!isEmpty(&(move->singleCellMoves))) {
            singleCellMove* scMove = pop(&(move->singleCellMoves));
            free(scMove);
        }
		free(move);
	}
}
