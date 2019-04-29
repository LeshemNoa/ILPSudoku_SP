#ifndef UNDO_REDO_LIST_H
#define UNDO_REDO_LIST_H

#include <stdbool.h>

#include "linked_list.h"
#include "move.h"

/**
 * The undo-redo list module, as part of the requirements, implements the list of moves
 * in a suduko game session. It can be traveresed using the Undo and Redo commands.
 * 
 */

/**
 * The UndoRedoList consists of a doubly linked list whose nodes contain Move structs,
 * a pointer to the current Node, represented the current latest move made during the game,
 * and a counter for the number of consecutive Undo commands the user made.
 * 
 */
typedef struct {
    List list; 
    const Node* current;
    int numUndos;
} UndoRedoList;

/**
 * Initializes an UndoRedoList struct.
 * 
 * @param move_list     [in, out] Pointer to the UndoRedoList to be initialized.
 */
void initUndoRedo(UndoRedoList* move_list);
/**
 * Adds a new Move struct to the list right after the Node pointed to by the current
 * pointer. That is, if exist any Moves to redo after the current Node, they are discraded
 * and repaced with the new node added to the list.
 * 
 * @param move_list     [in, out] A pointer to the list to be added 
 * @param new_move      [in] The move to add at the head of the list
 * @return true         iff the procedure was susccessful
 * @return false        iff a memory error has occurred
 */
bool addNewMoveToList(UndoRedoList* move_list, Move* new_move);

/**
 * Checks whether exists any move to undo in the list.
 * 
 * @param moveList      [in] The move list to be examined
 * @return true         iff there exists a move to undo
 * @return false        otherwise
 */
bool canUndo(const UndoRedoList* moveList);

/**
 * Checks whether exists any move to redo in the list.
 * 
 * @param moveList      [in] The move list to be examined
 * @return true         iff there exists a move to redo
 * @return false        otherwise
 */
bool canRedo(const UndoRedoList* moveList);
/**
 * Reflects the consequences of a Undo command on the UndoRedoList struct by 
 * moving the current pointer to the next node and incrementing numUndos. 
 * 
 * @param moveList          [in, out] Move list to be updated
 * @return const Move*       A read-only const pointer to a Move struct corresponding with
 *                          the move that needs to be undone
 */
const Move* undoInList(UndoRedoList* moveList);

/**
 * Reflects the consequences of a Redo command on the UndoRedoList struct by 
 * moving the current pointer to the previous node and decrementing numUndos. 
 * 
 * @param moveList          [in, out] Move list to be updated
 * @return const Move*      A read-only const pointer to a Move struct corresponding with
 *                          the move that needs to be redone
 */
const Move* redoInList(UndoRedoList* moveList);

/**
 * Get the Move contained in the the current Node.
 * 
 * @param moveList          [in] Move list to be examined
 * @return const Move*      A read-only const pointer to a Move struct corresponding with
 *                          the current move
 */
const Move* getCurrentMove(UndoRedoList* moveList);

/**
 * Destroys a UndoRedoList struct by freeing all the memory allocated to it and its sub-structs.
 * 
 * @param move  [in] Pointer to the UndoRedoList struct to be destryoed.
 */
void cleanupUndoRedoList(UndoRedoList* moveList);

#endif /* UNDO_REDO_LIST_H */
