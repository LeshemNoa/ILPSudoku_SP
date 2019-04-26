#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>

typedef struct Node Node;

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
const Node* getListHead(const List* list);
const Node* getListTail(const List* list);
const Node* getNodeNext(const Node* node);
const Node* getNodePrev(const Node* node);
void* getNodeData(const Node* node);

#endif /* LINKED_LIST_H */
