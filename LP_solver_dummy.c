#include "LP_solver.h"

#define UNUSED(x) (void)(x)

solveBoardUsingLinearProgrammingErrorCode solveBoardUsingLinearProgramming(solveBoardUsingLinearProgrammingSolvingMode solvingMode, const Board* board, Board* boardSolution, double*** allCellsValuesScores) {
	UNUSED(solvingMode);
	UNUSED(board);
	UNUSED(allCellsValuesScores);
	UNUSED(boardSolution);
	return SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE;
}

void freeValuesScoresArr(double*** valuesScores, const Board* board) {
	UNUSED(valuesScores);
	UNUSED(board);
	return;
}

bool allocateValuesScoresArr(double**** valuesScoresOut, const Board* board) {
	UNUSED(valuesScoresOut);
	UNUSED(board);
	return false;
}
