#include "BT_solver.h"

#include "linked_list.h"
#include <stdlib.h>

typedef struct {
    int curRow;
    int curCol;
} CallFrame;

bool pushStack(List* stack, int curRow, int curCol) {
    CallFrame* frame = (CallFrame*)calloc(1, sizeof(CallFrame));
    if (frame == NULL) {
        return false;
    }
    frame->curRow = curRow;
    frame->curCol = curCol;
    if (!pushList(stack, frame)) {
        free(frame);
        return false;
    }

    return true;
}

bool peekStack(const List* stack, int* curRow, int* curCol) {
    CallFrame* frame;
    if (isListEmpty(stack)) {
        return false;
    }
    
    frame = (CallFrame*)getNodeData(getListHead(stack));
    *curRow = frame->curRow;
    *curCol = frame->curCol;
    return true;
}

bool popStack(List* stack) {
    CallFrame* frame = (CallFrame*) popList(stack);
    if (frame == NULL) {
        return false;
    }

    free(frame);
    return true;
}

void clearStack(List* stack) {
    while(!isListEmpty(stack)) {
        popStack(stack);
    }
}

bool calculateNumSolutions(const Board* boardIn, int* numSolutions) {
	List stack = {0};
	int curCol, curRow;
	int sum = 0;
	int MN = getBoardBlockSize_MN(boardIn);
	bool erroneous = false;
	Board boardCopy;
	
	if (!checkErroneousCells(boardIn, &erroneous)) {
		/* memory error */
		return false;
	}

	if (erroneous) {
		/* board has errors, no possible solutions */
		*numSolutions = 0;
		return true;
	}

	if (!getNextEmptyBoardCell(boardIn, 0, 0, &curRow, &curCol)) {
		*numSolutions = 1; /* board is full and has no errors */
		return true;
	}

	if (!copyBoard(boardIn, &boardCopy)) {
		/* memory error */
		return false;
	}

	initList(&stack);

	/* push initial cell */
	if (!pushStack(&stack, curRow, curCol)) {
		/* memory error */
		cleanupBoard(&boardCopy);
		return false;
	}

	while (peekStack(&stack, &curRow, &curCol)) {
		Cell* cell;
		int nextRow, nextCol, newValue;
		bool isLegalValue;

		cell = getBoardCellByRow(&boardCopy, curRow, curCol);
		if (getBoardCellValue(cell) == MN) { /* max value */
			/* back track */
			emptyBoardCell(cell);
			popStack(&stack);
			continue;
		}

		/* increment value, not assuming EMPTY_CELL_VALUE == 0 */
		newValue = isBoardCellEmpty(cell) ? 1 : cell->value + 1;
		/* check if board is valid after incrementing value*/
		isLegalValue = isValueLegalForBoardCell(&boardCopy, curRow, curCol, newValue);
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
		if (!pushStack(&stack, nextRow, nextCol)) {
			/* memory error */
			clearStack(&stack);
			cleanupBoard(&boardCopy);
			return false;
		}
	}

	cleanupBoard(&boardCopy);
	*numSolutions = sum;
	return true;
}
