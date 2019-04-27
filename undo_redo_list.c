#include <stdlib.h>

#include "undo_redo_list.h"

/* Note: Lists are not dynamically allocated anymore. Nodes and their data still are */

void initUndoRedo(UndoRedoList* moveList) {
    if (moveList != NULL) {
        initList(&(moveList->list));
        moveList->current = getListHead(&(moveList->list));
        moveList->numUndos = 0;
    }
}

/* returns false upon memory allocation error in pushList */
bool addNewMoveToList(UndoRedoList* moveList, Move* newMove) {
    while (getListHead(&(moveList->list)) != moveList->current) { /* CR: this part (the loop) is intended to clear the redo part of the list, right? If so, I think there's a bug here. Copying the previous comment...: */ /* CR: found bug: this check is not good enough, because current can point to head, but numUndos may still be greater than zero. I thin that all you really need to check is if numUndos is greater than zero (talk to me if don't understand what I mean; I can give you a case to run for yourself) */
        Move* move = (Move*)popList(&(moveList->list));
        destroyMove(move);
    }
    if (!pushList(&(moveList->list), newMove)) { return false; }
    moveList->current = getListHead(&(moveList->list));
    moveList->numUndos = 0;
    return true;
}

bool canUndo(const UndoRedoList* moveList) {
    return moveList->numUndos < getListSize(&(moveList->list));
}

bool canRedo(const UndoRedoList* moveList) {
    return moveList->numUndos > 0;
}

const Move* undoInList(UndoRedoList* moveList) {
    const Move* moveToUndo;
    const Node* next;
    if (!canUndo(moveList)) {
        return NULL; /* no more moves to undo */
    }
    moveToUndo = getCurrentMove(moveList);
    next = getNodeNext(moveList->current);
    if (next != NULL) {
        moveList->current = next;
    }
    moveList->numUndos++;
    return moveToUndo;
}

const Move* redoInList(UndoRedoList* moveList) {
    const Move* moveToRedo;
    if (!canRedo(moveList)) {
        return NULL; /* cannot advance current further */
    }
    if (moveList->numUndos == getListSize(&(moveList->list))) {
        moveToRedo = getCurrentMove(moveList);
    } else {
        moveList->current = getNodePrev(moveList->current);
        moveToRedo = getCurrentMove(moveList);
    }
    moveList->numUndos--;
    return moveToRedo;
}

const Move* getCurrentMove(UndoRedoList* moveList) {
    return (Move*) getNodeData(moveList->current);
}

void cleanupUndoRedoList(UndoRedoList* moveList) {
	while (!isListEmpty(&(moveList->list))) {
        Move* move = (Move*)popList(&(moveList->list));
        destroyMove(move);
	}
}
