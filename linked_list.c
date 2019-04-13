#include <stdlib.h>
#include "linked_list.h"

List* createNewList() {
    List* list = malloc(sizeof(List));
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
    }
    return list;
}

Node* createNewNode(void* new_data) {
    Node* new_node;
    new_node = (Node*) malloc(sizeof(Node));
    if (new_node != NULL) {
        new_node->data = new_data;
        new_node->next = NULL;
        new_node->prev = NULL;
    }
    return new_node;
}

/* returns true on success, i.e. no malloc issue */
bool push(List* list, void* new_data) {
    Node* new_node = createNewNode(new_data);
    if (new_node == NULL) { return false; }

    if (list->head != NULL) {
        list->head->next = new_node;
        new_node->prev = list->head;
        list->head = new_node;
    }
    else { /* pushing into an empty list */
        list->head = new_node;
        list->tail = list->head;
    }
    return true;
}

void* pop(List* list) {
    void* data;
    Node* old_head = NULL;

    if (list->head == NULL) { /* list is empty */
        return NULL;
    }

    old_head = list->head;
    data = old_head->data;
    if (old_head->prev == NULL) { /* list has 1 element */
        list->head = NULL;
        list->tail = NULL;
    } 
    else { 
        list->head = old_head->prev;
        list->head->next = NULL;
        old_head->prev = NULL;
    }
    free(old_head);
    return data;   
}

bool pushBack(List* list, void* new_data) {
    Node* new_node = createNewNode(new_data);
    if (new_node == NULL) { return false; }

    if (list->tail != NULL) {
        list->tail->prev = new_node;
        new_node->next = list->tail;
        list->tail = new_node;
    }
    else { /* pushing into an empty list */
        list->head = new_node;
        list->tail = list->head;
    }
    return true;
}

void* popBack(List* list) {
    void* data;
    Node* old_tail = NULL;

    if (list->head == NULL) { /* list is empty */
        return NULL;
    }

    old_tail = list->tail;
    data = old_tail->data;
    if (old_tail->next == NULL) { /* list has 1 element */
        list->head = NULL;
        list->tail = NULL;
    } 
    else { 
        list->tail = old_tail->next;
        list->tail->prev = NULL;
        old_tail->next = NULL;
    }
    free(old_tail);
    return data;   
}

void destroyList(List* list) {
    void* data;
    while (list->head != NULL) {
        data = pop(list);
        free(data);
    }
    free(list);
}


