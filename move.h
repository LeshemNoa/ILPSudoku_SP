#ifndef MOVE_H
#define MOVE_H

#include "linked_list.h"

/**
 * The Move module is responsible for representing and updating moves in the undo-redo list.
 */

/**
 * Represents an atomic modification in a single cell that followed some move made during
 * the game. It holds the indices of the cell changed, its previous value and its new value.
 */
typedef struct {
	int row;
	int col;
	int prevVal;
	int newVal;
} CellChange;

/**
 * Some commands cause extensive changes in the board at once. Thus each move is represented
 * by a list of CellChanges.  
 */
typedef struct {
	List cellChanges;
} Move;

/**
 * Creates a new, initialized Move struct.
 * 
 * @return Move*  A pointer to the allocated Move, or NULL if there'd been a memory error.
 */
Move* createMove();

/**
 * Destroys a Move struct by freeing all the memory allocated to it and its sub-structs.
 * 
 * @param move  [in] Pointer to the Move struct to be destryoed.
 */
void destroyMove(Move* move);

/**
 * Creates a new CellChange struct with provided parameters.
 * 
 * @param prevVal 		[in] The cell's value prior to the change
 * @param newVal 		[in] The cell's value after the change
 * @param row 			[in] The changed cell's row number
 * @param col 			[in] The changed cell's column number
 * @return CellChange* 	A pointer to the CellChange struct created, or NULL if there'd been
 * 						a memory error
 */
CellChange* createCellChange(int prevVal, int newVal, int row, int col);

/**
 * Adds a new CellChange to an existing Move struct, which consists of a list of CellChanges.
 * 
 * @param move 		[in, out] A pointer to the move struct to which the new CellChange
 * 					will be added
 * @param prevVal 	[in] The cell's value prior to the change
 * @param newVal 	[in] The cell's value after the change
 * @param row 		[in] The changed cell's row number
 * @param col 		[in] The changed cell's column number
 * @return true 	iff the procedure was successful
 * @return false 	iff there'd been a memory error
 */
bool addCellChangeToMove(Move* move, int prevVal, int newVal, int row, int col);

/**
 * Get the number of CellChanges a particular Move struct contains.
 * 
 * @param move  [in] The Move struct to be examined.
 * @return int 	The number of elements in the CellChange list
 */
int getMoveSize(const Move* move);

/**
 * Get the head of the list in a particular Move struct.
 * 
 * @param move 				[in] The move struct to be examined
 * @return const Node* 		A read-only pointer to the first Node in the list
 */
const Node* getFirstCellChange(const Move* move);
#endif /* MOVE_H */
