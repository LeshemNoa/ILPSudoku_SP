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
bool isListEmpty(const List* list);
bool pushList(List* list, void* new_data);
void* popList(List* list);
bool pushListBack(List* list, void* new_data);
void* popListBack(List* list);
int getListSize(const List* list);
Node* getListHead(const List* list);
Node* getListTail(const List* list);
Node* getNodeNext(Node* node);
Node* getNodePrev(Node* node);
void* getNodeData(Node* node);

#endif /* LINKED_LIST_H */
