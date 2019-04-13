#ifndef UNDO_REDO_LIST_H
#define UNDO_REDO_LIST_H
#include "linked_list.h"

typedef struct {
    List* list;
    Node* current;
} UndoRedoList;

UndoRedoList* createNewUndoRedo(void* start);
bool makeMove(UndoRedoList* move_list, void* new_data);
bool undo(UndoRedoList* move_list);
bool redo(UndoRedoList* move_list);
bool reset(UndoRedoList* move_list);
void* getCurrent(UndoRedoList* move_list);
void destroyUndoRedo(UndoRedoList* move_list);

#endif /* UNDO_REDO_LIST_H */
