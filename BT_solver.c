#include "BT_solver.h"

#include "stack.h"
#include <stdlib.h>

typedef struct {
    int curRow;
    int curCol;
} CallFrame;

bool pushCallFrame(Stack* stack, int curRow, int curCol) {
    CallFrame* frame = (CallFrame*)calloc(1, sizeof(CallFrame));
    if (frame == NULL) {
        return false;
    }
    frame->curRow = curRow;
    frame->curCol = curCol;
    if (!pushStack(stack, frame)) {
        free(frame);
        return false;
    }

    return true;
}

bool peekCallFrame(const Stack* stack, int* curRow, int* curCol) {
    const CallFrame* frame;
    if (isStackEmpty(stack)) {
        return false;
    }
    
    frame = (const CallFrame*)peekStack(stack);
    *curRow = frame->curRow;
    *curCol = frame->curCol;
    return true;
}

bool popCallFrame(Stack* stack) {
    CallFrame* frame = (CallFrame*) popStack(stack);
    if (frame == NULL) {
        return false;
    }

    free(frame);
    return true;
}

void clearCallFrames(Stack* stack) {
    while(!isStackEmpty(stack)) {
        popCallFrame(stack);
    }
}
/* pre-condition: the input board is not erroneous. */
bool calculateNumSolutions(const Board* boardIn, int* numSolutions) {
	Stack stack = {0};
	int curCol, curRow;
	int sum = 0;
	int MN = getBoardBlockSize_MN(boardIn);
	Board boardCopy = {0};
	
	if (!getNextEmptyBoardCell(boardIn, 0, 0, &curRow, &curCol)) {
		*numSolutions = 1; /* board is full and has no errors (pre-condition) */
		return true;
	}

	if (!copyBoard(boardIn, &boardCopy)) {
		/* memory error */
		return false;
	}

	initStack(&stack);

	/* push initial empty cell */
	if (!pushCallFrame(&stack, curRow, curCol)) {
		/* memory error */
		cleanupBoard(&boardCopy);
		return false;
	}

	while (peekCallFrame(&stack, &curRow, &curCol)) {
		Cell* cell;
		int nextRow, nextCol, newValue;
		bool isLegalValue;

		cell = getBoardCellByRow(&boardCopy, curRow, curCol);
		if (getBoardCellValue(cell) == MN) { /* max value */
			/* back track */
			emptyBoardCell(cell);
			popCallFrame(&stack);
			continue;
		}

		/* increment value, not assuming EMPTY_CELL_VALUE == 0 */
		newValue = isBoardCellEmpty(cell) ? 1 : cell->value + 1;
		/* check if board is valid after incrementing value*/
		isLegalValue = isValueLegalForBoardCell(&boardCopy, curRow, curCol, newValue); /* CR: note that calling this function will result in the same running time as the previous code did (which updated erroneousness). Surely you must have noticed that if you ran num_solutions on a large-enough board. You could save running time by creating counters. I leave it to you, to consider doing this or not (i.e. mimicking what isValueLegalForCell does in game module */
		/* set value anyway (after checking legality) */
		setBoardCellValue(&boardCopy, curRow, curCol, newValue);
		
		if (!isLegalValue) {
			/* illegal, try next value */
			continue;
		}

		if (!getNextEmptyBoardCell(&boardCopy, curRow, curCol, &nextRow, &nextCol)) {
			/* no more empty cells, count as solved */
			sum++;
			continue;
		}

		/* count solutions for next empty cell given current board */
		if (!pushCallFrame(&stack, nextRow, nextCol)) {
			/* memory error */
			clearCallFrames(&stack);
			cleanupBoard(&boardCopy);
			return false;
		}
	}

	cleanupBoard(&boardCopy);
	*numSolutions = sum;
	return true;
}
