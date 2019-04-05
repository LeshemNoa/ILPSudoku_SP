#include <stdlib.h>
#include "linked_list.h"

List* createNewList() {
    List* list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
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

void push(void* new_data, List* list) {
    if (list->head != NULL) {
        Node* new_node = createNewNode(new_data);
        list->head->prev = new_node;
        new_node->next = list->head;
        list->head = new_node;
    }
    else {
        list->head = createNewNode(new_data);
    }
}

void* pop(List* list) {
    void* data;
    Node* old_head = NULL;
    if (list->head != NULL) {
        Node* new_head = list->head->next;
        if(new_head != NULL) {
            new_head->prev = NULL;
        }
        list->head->next = NULL;
        old_head = list->head;
        list->head = new_head;
    }
    data = old_head->data;
    free(old_head);
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
