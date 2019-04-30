/**
 * BT_SOLVER Summary:
 *
 * This module is designed to calculate the number of solution to a provided sudoku board
 * using the exhaustive backtracking algorithm, using a stack to simulate the recursion instead
 * of making explicit recursive calls
 */

#include "BT_solver.h"

#include "stack.h"
#include <stdlib.h>

/**
 * The CallFrame struct contains the indices of the cell to be set in the current
 * simulated recursive call. 
 */
typedef struct {
    int curRow;
    int curCol;
} CallFrame;

/**
 * The BacktrackSolverState struct represents the current intermediate
 * state of the algorithm: the call stack, the board on which the algorithm runs,
 * and the counter matrices for values in the board, by row, column and block. 
 */
typedef struct BacktrackSolverState {
	int MN;	
	Board puzzle;
	Stack callStack;
	int** rowsCellsValuesCounters;
	int** columnsCellsValuesCounters;
	int** blocksCellsValuesCounters;
} BacktrackSolverState;

/**
 * Push a new call frame to the call stack.
 * 
 * @param state 	[in, out] Current state of the solver
 * @param curRow 	[in] The row number of the cell being filled
 * @param curCol 	[in] The column number of the cell being filled
 * @return true 	iff the procedure was successful
 * @return false 	iff a memory error occurred
 */
bool pushCallFrame(BacktrackSolverState* state, int curRow, int curCol) {
    CallFrame* frame = (CallFrame*)calloc(1, sizeof(CallFrame));
    if (frame == NULL) {
        return false;
    }
    frame->curRow = curRow;
    frame->curCol = curCol;
    if (!pushStack(&(state->callStack), frame)) {
        free(frame);
        return false;
    }

    return true;
}

/**
 * Peeks the top call frame in the stack.
 * 
 * @param state 	[in, out] Current state of the solver
 * @param curRow 	[in, out] A pointer to an integer to be assigned with the row number
 * 					of the top call frame
 * @param curCol 	[in, out] A pointer to an integer to be assigned with the column number
 * 					of the top call frame
 * @return true 	iff the stack is not empty and value have been assigned to the pointers
 * @return false 	otherwise
 */
bool peekCallFrame(BacktrackSolverState* state, int* curRow, int* curCol) {
    const CallFrame* frame;
    if (isStackEmpty(&(state->callStack))) {
        return false;
    }
    
    frame = (const CallFrame*)peekStack(&(state->callStack));
    *curRow = frame->curRow;
    *curCol = frame->curCol;
    return true;
}

/**
 * Pop the top call frame.
 * 
 * @param state 	[in, out] Current state of the solver
 * @return true 	iff the procedure was successful
 * @return false 	iff the stack is empty
 */
bool popCallFrame(BacktrackSolverState* state) {
    CallFrame* frame = (CallFrame*) popStack(&(state->callStack));
    if (frame == NULL) {
        return false;
    }

    free(frame);
    return true;
}

/**
 * Empties the call stack.
 * 
 * @param state 	[in, out] Current state of the solver
 */
void clearCallFrames(BacktrackSolverState* state) {
    while(!isStackEmpty(&(state->callStack))) {
        popCallFrame(state);
    }
}

/**
 * Frees all memory allocated to the value counter matrices.
 * 
 * @param state 	[in, out] Current state of the solver
 */
void freeSolverCellsValuesCounters(BacktrackSolverState* state) {
	if (state->rowsCellsValuesCounters != NULL) {
		freeSpecificCellsValuesCounters(state->rowsCellsValuesCounters, &(state->puzzle));
		state->rowsCellsValuesCounters = NULL;
	}
	if (state->columnsCellsValuesCounters != NULL) {
		freeSpecificCellsValuesCounters(state->columnsCellsValuesCounters, &(state->puzzle));
		state->columnsCellsValuesCounters = NULL;
	}
	if (state->blocksCellsValuesCounters != NULL) {
		freeSpecificCellsValuesCounters(state->blocksCellsValuesCounters, &(state->puzzle));
		state->blocksCellsValuesCounters = NULL;
	}
}

/**
 * Creates the value counter matrices for the board contained in the provided state. 
 * 
 * @param state 	[in, out] Current state of the solver
 * @return true 	iff the procedure was successful
 * @return false 	iff a memory error occured
 */
bool createSolverCellsValuesCounters(BacktrackSolverState* state) {
	state->rowsCellsValuesCounters = createCellsValuesCountersByCategory(&(state->puzzle), viewBoardCellByRow);
	state->columnsCellsValuesCounters = createCellsValuesCountersByCategory(&(state->puzzle), viewBoardCellByColumn);
	state->blocksCellsValuesCounters = createCellsValuesCountersByCategory(&(state->puzzle), viewBoardCellByBlock);

	if ((state->rowsCellsValuesCounters != NULL) &&
		(state->columnsCellsValuesCounters != NULL) &&
		(state->blocksCellsValuesCounters != NULL)) {
		return true;
	}

	freeSolverCellsValuesCounters(state);
	return false;
}

/**
 * Initializes the solver's state, given an inital read-only pointer to a board.
 * 
 * @param state 	[in, out] Current state of the solver
 * @param boardIn 	[in] Board to use for initializtion
 * @return true 	iff the procedure was successful
 * @return false 	iff a memory error occurred
 */
bool initSolverState(BacktrackSolverState* state, const Board* boardIn) {
	state->rowsCellsValuesCounters = NULL;
	state->columnsCellsValuesCounters = NULL;
	state->blocksCellsValuesCounters = NULL;
	state->MN = getBoardBlockSize_MN(boardIn);

	initStack(&state->callStack);
	if (!copyBoard(boardIn, &(state->puzzle))) {
		/* memory error */
		return false;
	}
	
	if (!createSolverCellsValuesCounters(state)) {
		/* memory error */
		cleanupBoard(&(state->puzzle));
		return false;
	}

	return true;
}

/**
 * Frees all memory allocated to the provided BacktrackSolverState sturct.
 * 
 * @param state 	[in, out] Current state of the solver
 */
void cleanupSolverState(BacktrackSolverState* state) {
	clearCallFrames(state);
	cleanupBoard(&(state->puzzle));
	freeSolverCellsValuesCounters(state);
}

/**
 * Checks if a particular value is a valid selection for a particular cell in the 
 * provided BacktrackSolverState's board.
 * 
 * @param state 		[in] BacktrackSolverState whose board's cell is examined
 * @param row 			[in] The row number of the cell being checked
 * @param col 			[in] The column number of the cell being checked
 * @param value 		[in] The value whose validity for the cell is checked	
 * @return true 		iff the value is indeed a valid selection for the cell
 * @return false 		otherwise
 */
bool isValueLegalForSolverCell(BacktrackSolverState* state, int row, int col, int value) {
	int block = whichBlock(&(state->puzzle), row, col);
	return (state->rowsCellsValuesCounters[row][value] == 0) &&
		   (state->columnsCellsValuesCounters[col][value] == 0) &&
		   (state->blocksCellsValuesCounters[block][value] == 0);
}

/**
 * Sets the value of the cell with provided indices, and reflecting the consequences of
 * that across the BacktrackSolverState's Board by updating the value counters.
 * 
 * @param state		 		[in, out] BacktrackSolverState whose board is changed
 * @param row 				[in] The number of row in which this cell is located
 * @param indexInRow 		[in] The number of column in which this cell is located
 * @param value 			[in] value to be set in this cell
 * @return int 				the previous value of that cell, for further processing
 */
int setSolverPuzzleCell(BacktrackSolverState* state, int row, int col, int value) {
	Cell* cell = getBoardCellByRow(&(state->puzzle), row, col);
	int block, prevValue = getBoardCellValue(cell);
	if (prevValue == value) {
		/* Nothing to be done */
		return value;
	}

	block = whichBlock(&(state->puzzle), row, col);

	if (prevValue != EMPTY_CELL_VALUE) {
		state->rowsCellsValuesCounters[row][prevValue]--;
		state->columnsCellsValuesCounters[col][prevValue]--;
		state->blocksCellsValuesCounters[block][prevValue]--;
	}

	if (value != EMPTY_CELL_VALUE) {
		state->rowsCellsValuesCounters[row][value]++;
		state->columnsCellsValuesCounters[col][value]++;
		state->blocksCellsValuesCounters[block][value]++;
	}

	setBoardCellValue(&(state->puzzle), row, col, value);
	return prevValue;
}

bool calculateNumSolutions(const Board* boardIn, int* numSolutions) {
	BacktrackSolverState state = {0};

	int curCol, curRow;
	int sum = 0;
	
	if (!getNextEmptyBoardCell(boardIn, 0, 0, &curRow, &curCol)) {
		*numSolutions = 1; /* board is full and has no errors (pre-condition) */
		return true;
	}

	if (!initSolverState(&state, boardIn)) {
		return false;
	}

	/* push initial empty cell */
	if (!pushCallFrame(&state, curRow, curCol)) {
		/* memory error */
		cleanupSolverState(&state);
		return false;
	}

	while (peekCallFrame(&state, &curRow, &curCol)) {
		int nextRow, nextCol, newValue;
		bool isLegalValue;
		const Cell* cell = viewBoardCellByRow(&(state.puzzle), curRow, curCol);

		/* increment value, not assuming EMPTY_CELL_VALUE == 0 */
		newValue = isBoardCellEmpty(cell) ? 1 : getBoardCellValue(cell) + 1;

		if (newValue == state.MN + 1) { /* max value */
			/* back track */
			setSolverPuzzleCell(&state, curRow, curCol, EMPTY_CELL_VALUE);
			popCallFrame(&state);
			continue;
		}

		/* check if board is valid after incrementing value*/
		isLegalValue = isValueLegalForSolverCell(&state, curRow, curCol, newValue);
		/* set value anyway (after checking legality) */
		setSolverPuzzleCell(&state, curRow, curCol, newValue);

		if (!isLegalValue) {
			/* illegal, try next value */
			continue;
		}

		if (!getNextEmptyBoardCell(&(state.puzzle), curRow, curCol, &nextRow, &nextCol)) {
			/* no more empty cells, count as solved */
			sum++;
			continue;
		}

		/* count solutions for next empty cell given current board */
		if (!pushCallFrame(&state, nextRow, nextCol)) {
			/* memory error */
			cleanupSolverState(&state);
			return false;
		}
	}

	cleanupSolverState(&state);
	*numSolutions = sum;
	return true;
}
