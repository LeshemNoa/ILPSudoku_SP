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

void initUndoRedo(UndoRedoList* move_list);
bool addNewMoveToList(UndoRedoList* move_list, Move* new_move);
bool canUndo(const UndoRedoList* moveList);
bool canRedo(const UndoRedoList* moveList);
Move* undoInList(UndoRedoList* moveList);
Move* redoInList(UndoRedoList* moveList);
Move* getCurrentMove(UndoRedoList* moveList);
void cleanupUndoRedoList(UndoRedoList* moveList);

#endif /* UNDO_REDO_LIST_H */
