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

void initList(List* list);
bool isEmpty(List* list);
bool push(List* list, void* new_data); /* CR: again, please use cannonical names... */ /* CR: it would also be better to have the name of the data structure within the name of each function (ESPECIALLY when you use a name like push, which could collide with a stack module's push function) */
void* pop(List* list);
bool pushBack(List* list, void* new_data);
void* popBack(List* list);
Node* getHead(List* list);
Node* getNext(Node* node);

#endif /* LINKED_LIST_H */
