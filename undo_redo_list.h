#ifndef UNDO_REDO_LIST_H
#define UNDO_REDO_LIST_H
#include "linked_list.h"

typedef struct Move Move; 

typedef struct {
    List* list; /* CR: do you need this to be a pointer? Maybe not. Consider and decide for yourself */
    Node* current;
    int numUndos;
} UndoRedoList;

UndoRedoList* createNewUndoRedo();
bool makeMove(UndoRedoList* move_list, Move* new_move);
bool undo(UndoRedoList* move_list); /* should this pass out move to undo? */
bool redo(UndoRedoList* move_list);
bool reset(UndoRedoList* move_list);
void* getCurrent(UndoRedoList* move_list);
void destroyUndoRedo(UndoRedoList* move_list);

#endif /* UNDO_REDO_LIST_H */
