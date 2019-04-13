#include "undo_redo_list.h"
#include <stdlib.h>

/* can be empty - right after load or new game setup*/
UndoRedoList* createNewUndoRedo() { /* CR: depending on what you decide, this function could in principle be removed */
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
bool makeMove(UndoRedoList* move_list, void* new_data) { /* CR: why void* you know what you expect here: a so-called 'Move' */
    while (move_list->list->head != move_list->current) { /* CR: this loop could be replaced by some 'split'+destroy functions (in linked_list). Also try as much as possible to use functions from linked_list when changing a linked_list 'object' */
        void* data = pop(move_list->list);
        free(data);
    }
    if (!push(move_list->list, new_data)) { return false; }
    move_list->current = move_list->list->head; /* CR: getting the head via a function would be my advice here (as much as possible - to use an object through an interface */
    return true;
}

/* return true iff something's actually changed */
bool undo(UndoRedoList* move_list) { /* CR: this name suggests that the undoing will happen here, which is wrong. This function will probably be returning the 'Move' to be undone.. */
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
    destroyList(move_list->list);
    free(move_list);
}
