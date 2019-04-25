#ifndef UNDO_REDO_LIST_H
#define UNDO_REDO_LIST_H

#include <stdbool.h>

#include "linked_list.h"
#include "move.h"

typedef struct {
    List list; 
    Node* current;
    int numUndos;
} UndoRedoList;

singleCellMove* createSingleCellMove(int prevVal, int newVal, int row, int col);
bool addSingleCellMoveToMove(Move* move, int prevVal, int newVal, int row, int col);
void initUndoRedo(UndoRedoList* move_list);
bool addNewMoveToList(UndoRedoList* move_list, Move* new_move);
bool canUndo(UndoRedoList* moveList);
bool canRedo(UndoRedoList* moveList);
Move* undoInList(UndoRedoList* moveList);
Move* redoInList(UndoRedoList* moveList);
Move* getCurrent(UndoRedoList* moveList);
void cleanupUndoRedoList(UndoRedoList* moveList);

#endif /* UNDO_REDO_LIST_H */
