/**
 * BT_SOLVER Summary:
 *
 * This module is designed to calculate the number of solution to a provided sudoku board
 * using the exhaustive backtracking algorithm, using a stack to simulate the recursion instead
 * of making explicit recursive calls
 *
 * calculateNumSolutions - calculates the number of solution to the given board
 */

#ifndef BT_SOLVER_H
#define BT_SOLVER_H

#include <stdbool.h>
#include "board.h"

/**
 * Calculates the number of possible solutions to the provided suduko board. 
 * It employs the exhaustive backtracking algorithm, trying every legal value
 * for each cell, and backtrack when the board is solved to find all other solutions.
 * pre-condition: the input board is not erroneous.
 * 
 * @param board             [in] Board to calculate the number of solutions for
 * @param numSolutions      [in, out] Pointer to an integer to be assigned with the result
 * @return true             iff the procedure was successful
 * @return false            iff a memory error occurred
 */

bool calculateNumSolutions(const Board* board, int* numSolutions);

#endif /* BT_SOLVER_H */
