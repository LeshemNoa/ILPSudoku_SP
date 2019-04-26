#include "BT_Solver.h"
#include "rec_stack.h"

bool calculateNumSolutions(GameState* gameState, int* numSolutions) {
	Stack stack = {{0}};
	int curCol, curRow;
	int sum = 0;
	int MN = getBlockSize_MN(gameState);
    Board* puzzle = getPuzzle(gameState);

	if (isBoardErroneous(gameState) || getNumEmptyCells(gameState) == 0) {
		/* board has errors or no moves to make, no possible solutions */
		*numSolutions = 0;
		return true;
	}

	initStack(&stack);

	if (!getNextEmptyBoardCell(puzzle, 0, 0, &curRow, &curCol)) {
		*numSolutions = 1; /* board is full and has no errors */
		return true;
	}

	/* pushList initial cell */
	if (!pushStack(&stack, curRow, curCol)) {
		/* memory error */
		return false;
	}

	while (peekStack(&stack, &curRow, &curCol)) {
		Cell* cell;
		int nextRow, nextCol, newValue;
		bool isLegalValue;

		cell = getBoardCellByRow(puzzle, curRow, curCol);
		if (getBoardCellValue(cell) == MN) { /* max value */
			/* back track */
			setPuzzleCell(gameState, curRow, curCol, EMPTY_CELL_VALUE);
			popStack(&stack);
			continue;
		}

		/* increment value, not assuming EMPTY_CELL_VALUE == 0 */
		newValue = isBoardCellEmpty(cell) ? 1 : cell->value + 1;
		/* check if board is valid after incrementing value*/
		isLegalValue = isValueLegalForCell(gameState, curRow, curCol, newValue);
		/* set value anyway (after checking legality) */
		setPuzzleCell(gameState, curRow, curCol, newValue);
		
		if (!isLegalValue) {
			/* illegal, try next value */
			continue;
		}

		if (!getNextEmptyBoardCell(puzzle, curRow, curCol, &nextRow, &nextCol)) {
			/* no more empty cells, count as solved */
			sum++;
			continue;
		}

		/* count solutions for next empty cell given current board */
		if (!pushStack(&stack, nextRow, nextCol)) {
			/* memory error, undo every change */
			while(peekStack(&stack, &curRow, &curCol)) { /* CR+: this loop looks like it should be a function of the stack */ /* CR Response: no because we want to backtrack all changes */
				setPuzzleCell(gameState, curRow, curCol, EMPTY_CELL_VALUE); /* CR+: you'll probably be working on a copy of the original board, hence there won't be any read need to restore it to its original state */ /* CR Response: we're working with the original state, thus we need to revert all changes */
				popStack(&stack);
			}
			return false;
		}
	}

	*numSolutions = sum;
	return true;
}
