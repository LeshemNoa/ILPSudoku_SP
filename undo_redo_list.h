#ifndef UNDO_REDO_LIST_H
#define UNDO_REDO_LIST_H

#include <stdbool.h>

#include "linked_list.h"
#include "move.h"

typedef struct {
    List list; 
    Node* current; /* CR: could be a const pointer, couldn't it? */
    int numUndos;
} UndoRedoList;

void initUndoRedo(UndoRedoList* move_list);
bool addNewMoveToList(UndoRedoList* move_list, Move* new_move);
bool canUndo(const UndoRedoList* moveList);
bool canRedo(const UndoRedoList* moveList);
Move* undoInList(UndoRedoList* moveList); /* CR: returned value should probably be const pointer, no? */
Move* redoInList(UndoRedoList* moveList); /* CR: returned value should probably be const pointer, no? */
Move* getCurrentMove(UndoRedoList* moveList); /* CR: returned value should probably be const pointer, no? */
void cleanupUndoRedoList(UndoRedoList* moveList);

#endif /* UNDO_REDO_LIST_H */
