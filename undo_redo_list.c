#include "undo_redo_list.h"
#include <stdlib.h>

/* can be empty - right after load or new game setup*/
UndoRedoList* createNewUndoRedo() {
    UndoRedoList* move_list;
    List* list = createNewList();
    if (list == NULL) { return NULL; }
    
    move_list = (UndoRedoList*) malloc(sizeof(UndoRedoList));
    if (move_list != NULL) {
        move_list->list = list;
        move_list->current = list->head;
    }
    else {
        destroyList(list); /* allocating List succeeded, allocating UndoRedo failed*/
    }
    return move_list;
}

/* returns true on success, i.e. push was successful */
bool makeMove(UndoRedoList* move_list, void* new_data) {
    while (move_list->list->head != move_list->current) {
        void* data = pop(move_list->list);
        free(data);
    }
    if (!push(move_list->list, new_data)) { return false; }
    move_list->current = move_list->list->head;
    return true;
}

/* return true iff something's actually changed */
bool undo(UndoRedoList* move_list) {
    if (move_list->current == move_list->list->tail) {
        return false; /* cannot move current back anymore */
    }
    move_list->current = move_list->current->prev;
    return true;
}

/* return true iff something's actually changed */
bool redo(UndoRedoList* move_list) {
    if (move_list->current == move_list->list->head) {
        return false; /* cannot advance current further */
    }
    move_list->current = move_list->current->next;
    return true;
}

/* return true iff something's actually changed */
bool reset(UndoRedoList* move_list) {
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
    destroyList(move_list->list);
    free(move_list);
}
