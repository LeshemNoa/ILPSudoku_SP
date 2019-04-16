#include "undo_redo_list.h"
#include <stdlib.h>

/* Lists are not dynamically allocated anymore. Nodes and their data still are */

void initUndoRedo(UndoRedoList* move_list) { 
    if (move_list != NULL) {
        initList(&(move_list->list));
        move_list->current = move_list->list.head;
        move_list->numUndos = 0;
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
}

bool addSingleCellMoveToMove(Move* move, int prevVal, int newVal, int col, int row) {
    singleCellMove* scMove = createSingleCellMove(prevVal, newVal, col, row);
    return push(&(move->singleCellMoves), scMove);
}

/* returns true on success, i.e. push was successful */
bool makeMove(UndoRedoList* move_list, Move* new_move) {
    while (move_list->list.head != move_list->current) {
        void* data = pop(&(move_list->list));
        free(data);
    }
    if (!push(&(move_list->list), new_move)) { return false; }
    move_list->current = move_list->list.head;
    return true;
}

/* return true iff something's actually changed */
bool undo(UndoRedoList* move_list) { /* CR: this name suggests that the undoing will happen here, which is wrong. This function will probably be returning the 'Move' to be undone.. */
    if (move_list->current == move_list->list.tail) {
        return false; /* cannot move current back anymore */
    }
    move_list->current = move_list->current->next;
    move_list->numUndos = 0;
    return true;
}

/* return true iff something's actually changed */
bool redo(UndoRedoList* move_list) {
    if (move_list->current == move_list->list.head) {
        return false; /* cannot advance current further */
    }
    move_list->current = move_list->current->prev;
    return true;
}

/* return true iff something's actually changed */
bool reset(UndoRedoList* move_list) { /* CR: there's no need for such a function here. Reset is a command of the game, and in no way a part of the undo_redo_list module itself */
    bool changed = false;
    while (undo(move_list)) {
        changed = true;
    }
    return changed;
}

void* getCurrent(UndoRedoList* move_list) {
    return move_list->current->data;
}

void destroyUndoRedo(UndoRedoList* move_list) {
    destroyList(&(move_list->list));
    free(move_list);
}
