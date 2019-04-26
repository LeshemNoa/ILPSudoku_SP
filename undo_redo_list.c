#include <stdlib.h>

#include "undo_redo_list.h"

/* Lists are not dynamically allocated anymore. Nodes and their data still are */

void initUndoRedo(UndoRedoList* moveList) { 
    if (moveList != NULL) {
        initList(&(moveList->list));
        moveList->current = moveList->list.head; /* CR: access head through a wrapper function */
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
    return push(&(move->singleCellMoves), scMove); /* CR: if push has failed, you have to free scMove */
}

/* returns false upon memory allocation error in push */
bool addNewMoveToList(UndoRedoList* moveList, Move* newMove) {
    while (moveList->list.head != moveList->current) { /* CR: should call getHead */ /* CR: also, perhaps make this a function of undo_redo_list? (it could also naturally change numUndos to 0 */ /* CR: found bug: this check is not good enough, because current can point to head, but numUndos may still be greater than zero. I thin that all you really need to check is if numUndos is greater than zero (talk to me if don't understand what I mean; I can give you a case to run for youself) */
        void* data = pop(&(moveList->list));
        free(data);
    }
    if (!push(&(moveList->list), newMove)) { return false; }
    moveList->current = moveList->list.head;
    moveList->numUndos = 0;
    return true;
}

bool canUndo(UndoRedoList* moveList) {
    return (moveList->numUndos != moveList->list.size); /*CR: use a wrapper function to get list's size */ /* CR: better to check if numUndos is smaller that list's size (so if the worst happens and numUndos is negative....) */
}

bool canRedo(UndoRedoList* moveList) {
    return (moveList->numUndos != 0); /* CR: again, better to check if numUndos is larger than zero */
}

Move* undoInList(UndoRedoList* moveList) {
    Move* moveToUndo;
    if (!canUndo(moveList)) {
        return NULL; /* no more moves to undo */
    }
    moveToUndo = getCurrent(moveList);
    if (moveList->current->next != NULL) { /* CR: use a wrapper function to get next */ /* CR: it's better to minimise number of derefercing when possible - aka save the 'next' node to a variable */
        moveList->current = moveList->current->next;
    }
    moveList->numUndos++;
    return moveToUndo;
}

Move* redoInList(UndoRedoList* moveList) { /* CR: similar to undoInList */
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
    return (Move*) moveList->current->data; /* CR: access data through a wrapper function */
}

void cleanupUndoRedoList(UndoRedoList* moveList) {
	while (!isEmpty(&(moveList->list))) { /* CR: this looks like it needs to be a function of List */
		Move* move = (Move*)pop(&(moveList->list));
        while(!isEmpty(&(move->singleCellMoves))) { /* CR: ... which, it seems, would be called here again */
            singleCellMove* scMove = pop(&(move->singleCellMoves));
            free(scMove);
        }
		free(move);
	}
}
