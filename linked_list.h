#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node {
    void* data;
    struct node* next;
    struct node* prev;
} Node;

typedef struct list
{
    Node* head;
    Node* tail;
} List;

List* createNewList();
void destroyList(List* list);
void push(void* new_data, List* list);
void* pop(List* list);

#endif /* LINKED_LIST_H */
