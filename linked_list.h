#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdbool.h>

typedef struct node {
    void* data;
    struct node* next;
    struct node* prev;
} Node;

typedef struct list
{
    Node* head;
    Node* tail;
    int size;
} List;

List* createNewList();
void destroyList(List* list);

bool push(List* list, void* new_data);
void* pop(List* list);
bool pushBack(List* list, void* new_data);
void* popBack(List* list);

#endif /* LINKED_LIST_H */
