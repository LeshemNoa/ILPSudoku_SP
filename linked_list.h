#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>

/**
 * As part of the requirements of the project, this module contains an implementation of
 * a generic doubly linked list. 
 * 
 * initList - Initialize the provided list. 
 * isListEmpty - Checks if the provided list is empty.
 * pushList - Add a new item at the head of the list. 
 * popList - Removes the node at the head of the list.
 * pushListBack - Add a new item at the tail of the list. 
 * popListBack - Removes the node at the tail of the list.
 * getListSize - Get the size of the provided list.
 * getListHead - Get the Node at the head of the list.
 * getListTail -  Get the Node at the tail of the list.
 * getNodeNext - Get the provided node's next node.
 * getNodePrev -  Get the provided node's previous node.
 * getNodeData -  Get the data contained in the provided node.
 */

typedef struct Node Node;

/**
 * The List struct represents the doubly linked list. It contains a pointer to the node
 * at the head of the list and to the node at the tail, as well as a size field for the
 * number of elements in the list. 
 */
typedef struct list
{
    Node* head;
    Node* tail;
    int size;
} List;

/**
 * Initialize the provided list. 
 * 
 * @param list      [in, out] Pointer to a list to be initialized
 */
void initList(List* list);

/**
 * Checks if the provided list is empty. 
 * 
 * @param list      [in] The list to be examined
 * @return true     iff the list is empty
 * @return false    otherwise
 */
bool isListEmpty(const List* list);

/**
 * Add a new item at the head of the list. 
 * 
 * @param list      [in, out] List to which the new element is added
 * @param new_data  [in] Data to be assigned as the content of the new node
 * @return true     iff the procedure was successful
 * @return false    iff a memory error has occurred
 */
bool pushList(List* list, void* new_data);

/**
 * Removes the node at the head of the list.
 * 
 * @param list      [in, out] List whose head is removed
 * @return void*    The data that was contained in the popped node
 */
void* popList(List* list);

/**
 * Add a new item at the tail of the list. 
 * 
 * @param list      [in, out] List to which the new element is added
 * @param new_data  [in] Data to be assigned as the content of the new node
 * @return true     iff the procedure was successful
 * @return false    iff a memory error has occurred
 */
bool pushListBack(List* list, void* new_data);

/**
 * Removes the node at the tail of the list.
 * 
 * @param list      [in, out] List whose tail is removed
 * @return void*    The data that was contained in the popped node
 */
void* popListBack(List* list);

/**
 * Get the size of the provided list.
 * 
 * @param list  [in] The list whose size is requested
 * @return int   The list's size
 */
int getListSize(const List* list);

/**
 * Get the Node at the head of the list.
 * 
 * @param list          [in] List whose head is requested
 * @return const Node*  A read-only pointer to the node at the head of the list
 */
const Node* getListHead(const List* list);

/**
 * Get the Node at the tail of the list.
 * 
 * @param list 
 * @return const Node* 
 */
const Node* getListTail(const List* list);

/**
 * Get the provided node's next node.
 * 
 * @param node              [in] The node whose next node is requested
 * @return const Node*      A read-only pointer to the next node
 */
const Node* getNodeNext(const Node* node);

/**
 * Get the provided node's previous node.
 * 
 * @param node              [in] The node whose previous node is requested
 * @return const Node*      A read-only pointer to the previous node
 */
const Node* getNodePrev(const Node* node);

/**
 * Get the data contained in the provided node.
 * 
 * @param node      [in] Node whose data is requested
 * @return void*    The data
 */
void* getNodeData(const Node* node);

#endif /* LINKED_LIST_H */
