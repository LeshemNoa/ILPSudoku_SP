#include <stdlib.h>
#include "linked_list.h"

/* Nodes and data are dynamically allocated while lists aren't */

void initList(List* list) { 
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
}

bool isEmpty(List* list) {
    return (list->size == 0);
}

Node* createNewNode(void* new_data) {
    Node* new_node;
    new_node = (Node*) malloc(sizeof(Node));
    if (new_node != NULL) {
        new_node->data = new_data;
        new_node->prev = NULL;
        new_node->next = NULL;
    }
    return new_node;
}

bool push(List* list, void* new_data) {
    Node* new_node = createNewNode(new_data);
    if (new_node == NULL) { return false; }

    if (!isEmpty(list)) {
        list->head->prev = new_node;
        new_node->next = list->head;
        list->head = new_node;
    }
    else { /* pushing into an empty list */
        list->head = new_node;
        list->tail = list->head;
    }
    list->size++;
    return true;
}

void* pop(List* list) {
    void* data;
    Node* old_head = NULL;

    if (isEmpty(list)) { 
        return NULL;
    }

    old_head = list->head;
    data = old_head->data;
    if (list->size == 1) {
        list->head = NULL;
        list->tail = NULL;
    } 
    else { 
        list->head = old_head->next;
        list->head->prev = NULL;
        old_head->next = NULL;
    }
    free(old_head);
    list->size--;
    return data;   
}

bool pushBack(List* list, void* new_data) { 
    Node* new_node = createNewNode(new_data);
    if (new_node == NULL) { return false; }

    if (!isEmpty(list)) { /* tail != NULL */
        list->tail->next = new_node;
        new_node->prev = list->tail;
        list->tail = new_node;
    }
    else { /* pushing into an empty list */
        list->head = new_node;
        list->tail = list->head;
    }
    list->size++;
    return true;
}

void* popBack(List* list) { 
    void* data;
    Node* old_tail = NULL;

    if (isEmpty(list)) { /* list is empty */
        return NULL;
    }

    old_tail = list->tail;
    data = old_tail->data;
    if (list->size == 1) { 
        list->head = NULL;
        list->tail = NULL;
    } 
    else { 
        list->tail = old_tail->prev;
        list->tail->next = NULL;
        old_tail->prev = NULL;
    }
    list->size--;
    free(old_tail);
    return data;   
}

/* Data is dynamically allocated always */ 
void destroyList(List* list) {
    void* data;
    while (!isEmpty(list)) {
        data = pop(list);
        free(data); 
    }
}


