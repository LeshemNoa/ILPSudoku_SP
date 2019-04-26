/**
 * LP_SOLVER Summary:
 *
 * A module designed to solve sudoku boards, employing Linear Programming (LP) for the task.
 * The Linear Programming is done via an external library called Gurobi.
 *
 * A sudoku board may be passed to this module in order for it to be solved in one of two modes:
 * 		1. ILP (Integer Linear Programming) - in which the output would be a solved board
 * 		2. LP (Linear Programming) - in which the output would be an array of possible values and
 * 									 their scores for each and every empty cell in the input board.
 *
 *
 * solveBoardUsingLinearProgramming - solves a sudoku puzzle in any of the above-mentioned two modes
 * allocateValuesScoresArr - allocates an array for the values scores (relevant for LP mode)
 * freeValuesScoresArr - frees an array created by allocateValuesScoresArr (relevant for LP mode)
 */

#ifndef ILP_SOLVER_H_
#define ILP_SOLVER_H_

#include <stdbool.h>

#include "board.h"

#define ERROR_SUCCESS (0)

typedef enum {
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SUCCESS,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_BOARD_ISNT_SOLVABLE,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_CREATING_NEW_GRB_ENVIRONMENT,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_CREATING_NEW_GRB_MODEL,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_ADDING_VARS_AND_OBJECTIVE_FUNC,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_ADDING_CONSTRAINTS,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_ACQUIRING_MODEL_SOLUTION,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_OTHER_FAILURE
} solveBoardUsingLinearProgrammingErrorCode;

typedef enum {
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_ILP,
	SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_LP
} solveBoardUsingLinearProgrammingSolvingMode;


/**
 * solveBoardUsingLinearProgramming solves a given board in one of two modes: ILP (where a certain
 * solution of the board is found), or LP (where scores are given for all values in each of the empty
 * cells in the board).
 *
 * @param solvingMode			[in] the solving mode (ILP or LP)
 * @param board					[in] the board to be solved
 * @param boardSolution			[in, out] a pointer to a board when in ILP mode (can be NULL in LP mode)
 * @param allCellsValuesScores	[in, out] a pointer to an array gotten from allocateValuesScoresArr in LP
 * 									  mode (can be NULL in ILP mode). The structure of the array after
 * 									  the function has succeeded is: allCellsValuesScores[row][col][value] = score
 * 									  (note: cell (row, col) in board must have been empty)
 *
 * @return solveBoardUsingLinearProgrammingErrorCode	an error code is returned, specifying whether
 * 														the board was solved, or else that an error has
 * 														occurred
 */
solveBoardUsingLinearProgrammingErrorCode solveBoardUsingLinearProgramming(solveBoardUsingLinearProgrammingSolvingMode solvingMode, const Board* board, Board* boardSolution, double*** allCellsValuesScores);

/**
 * freeValuesScoresArr frees a scores array gotten from allocateValuesScoresArr
 *
 * @param valuesScores		[in] the values scores array to free
 * @param board				[in] the board which was passed to allocateValuesScoresArr upon creating the array
 *
 * @return	void
 */
void freeValuesScoresArr(double*** valuesScores, const Board* board);


/**
 * allocateValuesScoresArr allocates a scores array for the given board (later to be passed to solveBoardUsingLinearProgramming)
 *
 * @param valuesScoresOut	[out] a pointer in to assign the values scores array to
 * @param board				[in] the board for which to create the values scores array
 *
 * @return	bool			true when succeeds, false otherwise (due to memory allocation failure)
 */
bool allocateValuesScoresArr(double**** valuesScoresOut, const Board* board);

#endif /* ILP_SOLVER_H_ */
