#ifndef UNDO_REDO_LIST_H
#define UNDO_REDO_LIST_H
#include "linked_list.h"

typedef struct {
    List* list; /* CR: do you need this to be a pointer? Maybe not. Consider and decide for yourself */
    Node* current; /* CR: perhaps some numerical field indicating where 'current' is situated will be helpful? i.e.: if no undo's had taken place - 0, if k undo's had taken place - k */
} UndoRedoList;

UndoRedoList* createNewUndoRedo();
bool makeMove(UndoRedoList* move_list, void* new_data);
bool undo(UndoRedoList* move_list);
bool redo(UndoRedoList* move_list);
bool reset(UndoRedoList* move_list);
void* getCurrent(UndoRedoList* move_list);
void destroyUndoRedo(UndoRedoList* move_list);

#endif /* UNDO_REDO_LIST_H */
