#ifndef ILP_SOLVER_H_
#define ILP_SOLVER_H_

#include <stdbool.h>
#include <stdio.h>

#include "game.h"

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

solveBoardUsingLinearProgrammingErrorCode solveBoardUsingLinearProgramming(solveBoardUsingLinearProgrammingSolvingMode solvingMode, Board* board, Board* boardSolution, double*** allCellsValuesScores);

void freeValuesScoresArr(double*** valuesScores, Board* board);

bool allocateValuesScoresArr(double**** valuesScoresOut, Board* board);

#endif /* ILP_SOLVER_H_ */
