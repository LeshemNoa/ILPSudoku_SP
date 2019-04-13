#include <stdlib.h>
#include "linked_list.h"

List* createNewList() { /* CR: it needs to be considered: we don't necessarily need this function. Any one who wishes to use a list could create a local List struct of their own, no? I'm just presenting this option - you decide */
    List* list = malloc(sizeof(List));
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
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

    if (list->head != NULL) { /* CR: having an isEmpty function would make the code more readable (no need for the comment later). Also this function is considered to be "canonical" */
        list->head->next = new_node; /* CR: typically head is considered that starting point of the list and hence that its 'prev' is NULL and its 'next' points to the second element and onwards... Recall the doubly-linked list is an extension of the original singly-linked list where you only head 'next' pointers and the head... This is really confusing. */
        new_node->prev = list->head;
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

    if (list->head == NULL) { /* list is empty */ /* CR: isEmpty... */
        return NULL;
    }

    old_head = list->head;
    data = old_head->data;
    if (old_head->prev == NULL) { /* list has 1 element */ /* CR: you have a way to check for that easily using the 'size' field */
        list->head = NULL;
        list->tail = NULL;
    } 
    else { 
        list->head = old_head->prev;
        list->head->next = NULL;
        old_head->prev = NULL;
    }
    free(old_head);
    list->size--;
    return data;   
}

bool pushBack(List* list, void* new_data) { /* CR: symmetrical comments to 'push''s */
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
    list->size++;
    return true;
}

void* popBack(List* list) { /* CR: symmetrical comments to 'pop''s */
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
    list->size--;
    free(old_tail);
    return data;   
}

void destroyList(List* list) {
    void* data;
    while (list->head != NULL) {
        data = pop(list);
        free(data); /* CR: This is different behaviour when compared to pop's. It assumes that data inserted to the list will have been dynamically allocated. This either needs to be documented (in which case a TODO has to proclaim this here) or else taken care of in some other fashion (for example, this function could return a list of pointers to the user, or the user can indicate when inserting some data whether it was or wasn't dynamically allocated) */
    }
    free(list); /* CR: if we don't use createNewList, this would become unnecessary */
}


