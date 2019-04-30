/**
 * LINKED_LIST Summary:
 *
 * As part of the requirements of the project, this module contains an implementation of
 * a generic doubly linked list. 
 */

#include <stdlib.h>

#include "linked_list.h"

/**
 * the Node struct represents a list element. It has to pointers, to the next node
 * and the previous node, and a void pointer to the data the node contains.
 * 
 */
struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
};

void initList(List* list) { 
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
}

bool isListEmpty(const List* list) {
    return (list->size == 0);
}

/**
 * Creates a new Node struct containing the provided data.
 * 
 * @param new_data      [in] the new node's data
 * @return Node*        A pointer to the created node, or NULL on memory error
 */
Node* createNewNode(void* new_data) {
    Node* new_node;
    new_node = (Node*) calloc(1, sizeof(Node));
    if (new_node != NULL) {
        new_node->data = new_data;
        new_node->prev = NULL;
        new_node->next = NULL;
    }
    return new_node;
}

bool pushList(List* list, void* new_data) {
    Node* new_node = createNewNode(new_data);
    if (new_node == NULL) { return false; }

    if (!isListEmpty(list)) {
        list->head->prev = new_node;
        new_node->next = list->head;
        list->head = new_node;
    }
    else {
        list->head = new_node;
        list->tail = list->head;
    }
    list->size++;
    return true;
}

void* popList(List* list) {
    void* data;
    Node* old_head = NULL;

    if (isListEmpty(list)) { 
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

bool pushListBack(List* list, void* new_data) { 
    Node* new_node = createNewNode(new_data);
    if (new_node == NULL) { return false; }

    if (!isListEmpty(list)) {
        list->tail->next = new_node;
        new_node->prev = list->tail;
        list->tail = new_node;
    }
    else {
        list->head = new_node;
        list->tail = list->head;
    }
    list->size++;
    return true;
}

void* popListBack(List* list) { 
    void* data;
    Node* old_tail = NULL;

    if (isListEmpty(list)) {
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

int getListSize(const List* list) {
    return list->size;
}

const Node* getListHead(const List* list) {
    if (list == NULL || isListEmpty(list)) {
        return NULL;
    }

    return list->head;
}

const Node* getListTail(const List* list) {
    if (list == NULL || isListEmpty(list)) {
        return NULL;
    }

    return list->tail;
}

const Node* getNodeNext(const Node* node) {
    if (node == NULL) {
        return NULL;
    }

    return node->next;
}

const Node* getNodePrev(const Node* node) {
    if (node == NULL) {
        return NULL;
    }

    return node->prev;
}

void* getNodeData(const Node* node) {
    if (node == NULL) {
        return NULL;
    }

    return node->data;
}
