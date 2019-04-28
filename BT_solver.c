#include "BT_solver.h"

#include "stack.h"
#include <stdlib.h>

typedef struct {
    int curRow;
    int curCol;
} CallFrame;

typedef struct BacktrackSolverState {
	int MN;	
	Board puzzle;
	Stack callStack;
	int** rowsCellsValuesCounters;
	int** columnsCellsValuesCounters;
	int** blocksCellsValuesCounters;
} BacktrackSolverState;

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

bool popCallFrame(BacktrackSolverState* state) {
    CallFrame* frame = (CallFrame*) popStack(&(state->callStack));
    if (frame == NULL) {
        return false;
    }

    free(frame);
    return true;
}

void clearCallFrames(BacktrackSolverState* state) {
    while(!isStackEmpty(&(state->callStack))) {
        popCallFrame(state);
    }
}

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

void cleanupSolverState(BacktrackSolverState* state) {
	clearCallFrames(state);
	cleanupBoard(&(state->puzzle));
	freeSolverCellsValuesCounters(state);
}

bool isValueLegalForSolverCell(BacktrackSolverState* state, int row, int col, int value) {
	int block = whichBlock(&(state->puzzle), row, col);
	return (state->rowsCellsValuesCounters[row][value] == 0) &&
		   (state->columnsCellsValuesCounters[col][value] == 0) &&
		   (state->blocksCellsValuesCounters[block][value] == 0);
}

/* Maintaining the invariant: at all times, all cells' value counters are updated
correctly after each change in the board. Returning the previous value
of that cell */
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

/* pre-condition: the input board is not erroneous. */
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
