#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"
#include "undo_redo_list.h"

void test_list() {
    List* list = createNewList();
    int i;
    for (i = 0; i < 5; i++) {
        int* ip = malloc(sizeof(int));
        *ip = i;
        push(list, (void*) ip);
    }
    while (list->head != NULL) {
        int* ip = (int*) pop(list);
        printf("%d\n", *ip);
        free(ip);
    }

    for (i = 0; i < 5; i++) {
        int* ip = malloc(sizeof(int));
        *ip = i;
        push(list, (void*) ip);
    }
    destroyList(list);
}

void test_undoRedo() {
    int i;
    UndoRedoList* move_list;
    int* start = malloc(sizeof(int));
    *start = 0;

    move_list = createNewUndoRedo(start);
    for (i = 1; i < 6; i++) {
        int* ip = malloc(sizeof(int));
        *ip = i;
        makeMove(move_list, (void*) ip);
    }
    printf("Current = %d\n", *((int*)getCurrent(move_list)));
    undo(move_list);
    printf("After 1 undo, Current = %d\n", *((int*)getCurrent(move_list)));
    redo(move_list);
    printf("After redo, current = %d\n", *((int*)getCurrent(move_list)));
    reset(move_list);
    printf("After reset, current = %d\n", *((int*)getCurrent(move_list)));

    destroyUndoRedo(move_list);
}

int main() {
    test_undoRedo();
}
