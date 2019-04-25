#include <stdlib.h>

#include "game.h"

#include "undo_redo_list.h"
#include "rec_stack.h"

#define UNUSED(x) (void)(x)

#define GAME_MODE_INIT_STRING ("INIT")
#define GAME_MODE_EDIT_STRING ("EDIT")
#define GAME_MODE_SOLVE_STRING ("SOLVE")

/**
 * GameState struct represents a sudoku game in its current GameState. It contains the board itself, a 
 * possible solution for it, and the number of cells left to fill in the board in its current
 * configuration.
 * Note: the implementation of this struct is meant to be hidden from the user.
 */
struct GameState {
	Board puzzle;
	int numEmpty;
	int numErroneous;
	int** rowsCellsValuesCounters;
	int** columnsCellsValuesCounters;
	int** blocksCellsValuesCounters;
	UndoRedoList moveList;
};

int getNumEmptyCells(GameState* gameState) {
	return gameState->numEmpty;
}

int getNumErroneousCells(GameState* gameState) {
	return gameState->numErroneous;
}

int getNumColumnsInBlock_N(GameState* gameState) {
	return getNumColumnsInBoardBlock_N(&(gameState->puzzle));
}

int getNumRowsInBlock_M(GameState* gameState) {
	return getNumRowsInBoardBlock_M(&(gameState->puzzle));
}

int getBlockSize_MN(GameState* gameState) {
	return getBoardBlockSize_MN(&(gameState->puzzle));
}

int getPuzzleBoardSize_MN2(GameState* gameState) {
	return getBoardSize_MN2(&(gameState->puzzle));
}

bool isIndexInRange(GameState* gameState, int index) {
	return index >= 0 && index < getBlockSize_MN(gameState);
}

bool isCellValueInRange(GameState* gameState, int value) {
	return (value >= 1 && value <= getBlockSize_MN(gameState)) || (value == EMPTY_CELL_VALUE);
}

char* getCurModeString(State* state) {
	switch (state->gameMode) {
	case GAME_MODE_INIT:
		return GAME_MODE_INIT_STRING;
	case GAME_MODE_EDIT:
		return GAME_MODE_EDIT_STRING;
	case GAME_MODE_SOLVE:
		return GAME_MODE_SOLVE_STRING;
	}
	return NULL;
}

bool isBoardFilled(GameState* gameState) {
	return (gameState->numEmpty == 0);
}

bool isBoardErroneous(GameState* gameState) {
	return (gameState->numErroneous > 0);
}

getPuzzleSolutionErrorCode getPuzzleSolution(GameState* gameState, Board* solutionOut) {
	getPuzzleSolutionErrorCode retVal = GET_PUZZLE_SOLUTION_SUCCESS;

	Board board = {0};

	if (!exportBoard(gameState, &board)) {
		retVal = GET_PUZZLE_SOLUTION_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	switch (getBoardSolution(&board, solutionOut)) {
		case GET_BOARD_SOLUTION_SUCCESS:
			retVal = GET_PUZZLE_SOLUTION_SUCCESS;
			break;
		case GET_BOARD_SOLUTION_MEMORY_ALLOCATION_FAILURE:
			retVal =  GET_PUZZLE_SOLUTION_MEMORY_ALLOCATION_FAILURE;
			break;
		case GET_BOARD_SOLUTION_BOARD_UNSOLVABLE:
			retVal =  GET_PUZZLE_SOLUTION_BOARD_UNSOLVABLE;
			break;
		case GET_BOARD_SOLUTION_COULD_NOT_SOLVE_BOARD:
			retVal = GET_PUZZLE_SOLUTION_COULD_NOT_SOLVE_BOARD;
			break;
	}

	if (retVal != GET_PUZZLE_SOLUTION_SUCCESS) {
		cleanupBoard(solutionOut);
	}

	cleanupBoard(&board);

	return retVal;
}

isPuzzleSolvableErrorCode isPuzzleSolvable(GameState* gameState) {
	isPuzzleSolvableErrorCode retVal = IS_PUZZLE_SOLVABLE_BOARD_SOLVABLE;

	Board boardSolution = {0};

	switch (getPuzzleSolution(gameState, &boardSolution)) {
	case GET_PUZZLE_SOLUTION_SUCCESS:
		retVal = IS_PUZZLE_SOLVABLE_BOARD_SOLVABLE;
		break;
	case GET_PUZZLE_SOLUTION_MEMORY_ALLOCATION_FAILURE:
		retVal = IS_PUZZLE_SOLVABLE_MEMORY_ALLOCATION_FAILURE;
		break;
	case GET_PUZZLE_SOLUTION_BOARD_UNSOLVABLE:
		retVal = IS_PUZZLE_SOLVABLE_BOARD_UNSOLVABLE;
		break;
	case GET_PUZZLE_SOLUTION_COULD_NOT_SOLVE_BOARD:
		retVal = IS_PUZZLE_SOLVABLE_FAILED_VALIDATING;
		break;
	}

	cleanupBoard(&boardSolution);

	return retVal;
}

bool isCellEmpty(GameState* gameState, int row, int col) {
	return gameState->puzzle.cells[row][col].value == EMPTY_CELL_VALUE;
}

bool isCellFixed(GameState* gameState, int row, int col) {
	return isBoardCellFixed(getBoardCellByRow(&(gameState->puzzle), row, col));
}

bool isCellErroneous(GameState* gameState, int row, int col) {
	return isBoardCellErroneous(getBoardCellByRow(&(gameState->puzzle), row, col));
}

int getCellValue(GameState* gameState, int row, int col) {
	return getBoardCellValue(getBoardCellByRow(&(gameState->puzzle), row, col));
}

bool shouldMarkErrors(State* state) {
	return !(state->shouldHideErrors);
}

void setMarkErrors(State* state, bool shouldMarkErrors) {
	state->shouldHideErrors = !shouldMarkErrors;
}

bool isThereMoveToUndo(GameState* gameState) {
	UNUSED(gameState);

	return false;
}

bool isThereMoveToRedo(GameState* gameState) {
	UNUSED(gameState);

	return false;
}

void freeCellsValuesCounters(GameState* gameState) {
	if (gameState->rowsCellsValuesCounters != NULL) {
		freeSpecificCellsValuesCounters(gameState->rowsCellsValuesCounters, &(gameState->puzzle));
		gameState->rowsCellsValuesCounters = NULL;
	}
	if (gameState->columnsCellsValuesCounters != NULL) {
		freeSpecificCellsValuesCounters(gameState->columnsCellsValuesCounters, &(gameState->puzzle));
		gameState->columnsCellsValuesCounters = NULL;
	}
	if (gameState->blocksCellsValuesCounters != NULL) {
		freeSpecificCellsValuesCounters(gameState->blocksCellsValuesCounters, &(gameState->puzzle));
		gameState->blocksCellsValuesCounters = NULL;
	}
}

/*bool allocateCellsValuesCounters(GameState* gameState) {
	gameState->rowsCellsValuesCounters = allocateNewSpecificCellsValuesCounters(&(gameState->puzzle));
	gameState->columnsCellsValuesCounters = allocateNewSpecificCellsValuesCounters(&(gameState->puzzle));
	gameState->blocksCellsValuesCounters = allocateNewSpecificCellsValuesCounters(&(gameState->puzzle));

	if ((gameState->rowsCellsValuesCounters == NULL) ||
		(gameState->columnsCellsValuesCounters == NULL) ||
		(gameState->blocksCellsValuesCounters == NULL)) {
		freeCellsValuesCounters(gameState);
		return false;
	}

	return true;
}*/

/*void updateCellsValuesCounters(GameState* gameState) {
	updateCellsValuesCountersByCategory(gameState->rowsCellsValuesCounters, &(gameState->puzzle), getBoardCellByRow);
	updateCellsValuesCountersByCategory(gameState->columnsCellsValuesCounters, &(gameState->puzzle), getBoardCellByColumn);
	updateCellsValuesCountersByCategory(gameState->blocksCellsValuesCounters, &(gameState->puzzle), getBoardCellByBlock);
}*/

void updateCellErroneousness(GameState* gameState, int row, int col) {
	Cell* cell = getBoardCellByRow(&(gameState->puzzle), row, col);

	int oldErroneousness = isBoardCellErroneous(cell);
	bool newErroneousness = false;
	int deltaInNumErroneous = 0;

	if (isBoardCellEmpty(cell)) {
		newErroneousness = false;

		if (oldErroneousness == true)
			deltaInNumErroneous = -1;
	} else {

		int block = whichBlock(&(gameState->puzzle), row, col);
		int value = getBoardCellValue(cell);
		newErroneousness = (gameState->rowsCellsValuesCounters[row][value] > 1) ||
						   (gameState->columnsCellsValuesCounters[col][value] > 1) ||
						   (gameState->blocksCellsValuesCounters[block][value] > 1);

		if ((oldErroneousness == true) && (newErroneousness == false))
			deltaInNumErroneous = -1;
		else if ((oldErroneousness == false) && (newErroneousness == true))
			deltaInNumErroneous = +1;
	}

	setBoardCellErroneousness(cell, newErroneousness);
	gameState->numErroneous += deltaInNumErroneous;
}

void updateCellErroneousnessInRow(GameState* gameState, int row) { /* TODO: should be called by, for example, set (erroneousness of cells in the row might have changed) */
	int MN = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < MN; i++)
		updateCellErroneousness(gameState, row, i);
}

void updateCellErroneousnessInColumn(GameState* gameState, int col) { /* TODO: should be called by, for example, set (erroneousness of cells in the column might have changed) */
	int MN = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < MN; i++)
		updateCellErroneousness(gameState, i, col);
}

void updateCellErroneousnessInBlock(GameState* gameState, int block) { /* TODO: should be called by, for example, set (erroneousness of cells in the block might have changed) */
	int MN = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < MN; i++) {
		int row = 0, col = 0;
		getRowBasedIDGivenBlockBasedID(&(gameState->puzzle), block, i, &row, &col);
		updateCellErroneousness(gameState, row, col);
	}

}

void updateCellsErroneousness(GameState* gameState) {
	int MN = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int row = 0, col = 0;

	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++)
			updateCellErroneousness(gameState, row, col);
}

bool createCellsValuesCounters(GameState* gameState) {
	gameState->rowsCellsValuesCounters = createCellsValuesCountersByCategory(&(gameState->puzzle), getBoardCellByRow);
	gameState->columnsCellsValuesCounters = createCellsValuesCountersByCategory(&(gameState->puzzle), getBoardCellByColumn);
	gameState->blocksCellsValuesCounters = createCellsValuesCountersByCategory(&(gameState->puzzle), getBoardCellByBlock);

	if ((gameState->rowsCellsValuesCounters != NULL) &&
		(gameState->columnsCellsValuesCounters != NULL) &&
		(gameState->blocksCellsValuesCounters != NULL)) {
		return true;
	}

	freeCellsValuesCounters(gameState);
	return false;
}

GameState* createGameState(Board* board, GameMode mode) {
	bool success = true;
	GameState* gameState = NULL;

	gameState = calloc(1, sizeof(GameState));
	if (gameState != NULL) {
		if (board->cells == NULL) {
			gameState->puzzle = *board;
			if (!createEmptyBoard(&(gameState->puzzle)))
				success = false;
		} else {
			if (!copyBoard(board, &(gameState->puzzle)))
				success = false;
		}

		if (success)
			if (!createCellsValuesCounters(gameState))
				success = false;

		if (success) {
			gameState->numEmpty = countNumEmptyCells(&(gameState->puzzle));
			gameState->numErroneous = 0;
			updateCellsErroneousness(gameState);
			initUndoRedo(&(gameState->moveList));

			if (mode == GAME_MODE_EDIT)
				markAllCellsAsNotFixed(&(gameState->puzzle));

			return gameState;
		}
	}

	cleanupGameState(gameState);
	return NULL;
}

void cleanupGameState(GameState* gameState) {
	if (gameState == NULL)
		return;

	freeCellsValuesCounters(gameState);

	cleanupBoard(&(gameState->puzzle));

	/* cleanup undo-redo list */
	while (!isEmpty(&(gameState->moveList.list))) {
		void* data = pop(&(gameState->moveList.list));
		free(data);
	}

	free(gameState);
}

void setCellFixedness(GameState* gameState, int row, int col, bool isFixed) {
	setBoardCellFixedness(getBoardCellByRow(&(gameState->puzzle), row, col), isFixed);
}

void setCellErroneousness(GameState* gameState, int row, int col, bool isFixed) {
	setBoardCellErroneousness(getBoardCellByRow(&(gameState->puzzle), row, col), isFixed);
}

bool exportBoard(GameState* gameState, Board* boardInOut) {
	return copyBoard(&(gameState->puzzle), boardInOut);
}

void cleanupCellLegalValuesStruct(CellLegalValues* cellLegalValues) {
	cleanupBoardCellLegalValuesStruct(cellLegalValues);
}

bool isValueLegalForCell(GameState* gameState, int row, int col, int value) {
	int block = whichBlock(&(gameState->puzzle), row, col);
	return (gameState->rowsCellsValuesCounters[row][value] == 0) &&
		   (gameState->columnsCellsValuesCounters[col][value] == 0) &&
		   (gameState->blocksCellsValuesCounters[block][value] == 0);
}

bool fillCellLegalValuesStruct(GameState* gameState, int row, int col, CellLegalValues* cellLegalValuesInOut) {
	int MN = getBlockSize_MN(gameState);
	int value = 0;

	cellLegalValuesInOut->numLegalValues = 0;
	cellLegalValuesInOut->legalValues = calloc(MN + 1, sizeof(bool));
	if (cellLegalValuesInOut->legalValues == NULL)
		return false;

	for (value = 1; value <= MN; value++) {
		if (isValueLegalForCell(gameState, row, col, value)) {
			cellLegalValuesInOut->legalValues[value] = true;
			cellLegalValuesInOut->numLegalValues++;
		}
	}
	return true;
}

/* Maintaining the invariant: at all times, all erroneous cells are marked
correctly after each change in the board. Returning the previous value
of that cell */
int setPuzzleCell(GameState* state, int row, int col, int value) {
	int prevValue = getCellValue(state, row, col);

	if (prevValue != EMPTY_CELL_VALUE) {
		state->rowsCellsValuesCounters[row][prevValue]--;
		state->columnsCellsValuesCounters[col][prevValue]--;
		state->blocksCellsValuesCounters[whichBlock(&(state->puzzle), row, col)][prevValue]--;
	}

	if (value != EMPTY_CELL_VALUE) {
		state->rowsCellsValuesCounters[row][value]++;
		state->columnsCellsValuesCounters[col][value]++;
		state->blocksCellsValuesCounters[whichBlock(&(state->puzzle), row, col)][value]++;
	}

	if (isCellEmpty(state, row, col) && value != EMPTY_CELL_VALUE) {
		state->numEmpty--;
	}
	if (!isCellEmpty(state, row, col) && value == EMPTY_CELL_VALUE) {
		state->numEmpty++;
	}
	setBoardCellValue(&(state->puzzle), row, col, value);
	updateCellsErroneousness(state); /* TODO: could do something more efficient (going over just one row, col and block) */
	return prevValue;
}

/* returns false upon memory allocation error. This function also
causes the change in the board to be reflected in the undo-redo list  */
bool makeSingleCellMove(State* state, int value, int row, int col) {
	int prevVal;
	Move* move = (Move*) calloc(1, sizeof(Move));
	if (move == NULL) { return false; }
	initList(&move->singleCellMoves);

	prevVal = setPuzzleCell(state->gameState, row, col, value);

	if (!addSingleCellMoveToMove(move, prevVal, value, row, col) || 
		!addNewMoveToList(&(state->gameState->moveList), move)) {
		free(move);
		return false;
	}

	return true;
}

/* returns false upon memory allocation error. This function also
causes the change in the board to be reflected in the undo-redo list  */
bool makeMultiCellMove(State* state, Board* newBoard) {
	int MN, row, col, prevVal, newValue;
	Node* curr;
	Move* move = (Move*) calloc(1, sizeof(Move));
	if (move == NULL) { return false; }
	MN = getBlockSize_MN(state->gameState);

	initList(&move->singleCellMoves);

	/* find all differences and build single cell move list */
	for (row = 0; row < MN; row++) {
		for (col = 0; col < MN; col++) {
			prevVal = getCellValue(state->gameState, row, col);
			newValue = getBoardCellValue(getBoardCellByRow(newBoard, row, col));
			if (prevVal != newValue) {
				if (!addSingleCellMoveToMove(move, prevVal, newValue, row, col)) {
					/* memory error, cleanup and return */
					while (!isEmpty(&(move->singleCellMoves))) {
						void* data = pop(&(move->singleCellMoves));
						free(data);
					}
					free(move);
					return false;
				}
			}
		}
	}

	/* add move to move list */
	if (!addNewMoveToList(&(state->gameState->moveList), move)) {
		/* memory error, cleanup and return */
		while (!isEmpty(&(move->singleCellMoves))) {
			void* data = pop(&(move->singleCellMoves));
			free(data);
		}
		free(move);
		return false;
	}

	/* perform all single cell moves */
	curr = getHead(&(move->singleCellMoves));
	while (curr != NULL) {
		singleCellMove* scMove = (singleCellMove*)curr->data;
		setPuzzleCell(state->gameState, scMove->row, scMove->col, scMove->newVal);
		curr = getNext(curr);
	}
	
	return true;
}

Move* undoMove(State* state) {
	singleCellMove* scMove;
	Node* currNode;
	Move* moveToUndo = undoInList(&(state->gameState->moveList));
	if (moveToUndo == NULL) {
		return NULL; /* nothing to undo */
	}
	currNode = getHead(&(moveToUndo->singleCellMoves));
	while (currNode != NULL) {
		scMove = (singleCellMove*) currNode->data;
		setPuzzleCell(state->gameState, scMove->row, scMove->col, scMove->prevVal);
		currNode = getNext(currNode);
	}
	return moveToUndo;
}

Move* redoMove(State* state) {
	singleCellMove* scMove;
	Node* currNode;
	Move* moveToRedo = redoInList(&(state->gameState->moveList));
	if (moveToRedo == NULL) {
		return NULL; /* nothing to redo */
	}
	currNode = getHead(&(moveToRedo->singleCellMoves));
	while (currNode != NULL) {
		scMove = (singleCellMove*) currNode->data;
		setPuzzleCell(state->gameState, scMove->row, scMove->col, scMove->newVal);
		currNode = getNext(currNode);
	}

	return moveToRedo;
}

bool resetMoves(State* state) {
	bool changed = false;
    while (canUndo(&(state->gameState->moveList))) {
		undoMove(state);
        changed = true;
    }
    return changed;
}

bool isSolutionSuccessful(GameState* gameState) {
	return (isBoardFilled(gameState)) &&
		   (!isBoardErroneous(gameState));
}

bool isSolutionFailing(GameState* gameState) {
	return (isBoardFilled(gameState)) &&
		   (isBoardErroneous(gameState));
}

bool calculateNumSolutions(GameState* state, int* numSolutions) { /* TODO: probably should be in another (possibly BT_solver) module */
	Stack stack;
	int curCol, curRow;
	int sum = 0;
	int MN = getBlockSize_MN(state);
	initStack(&stack);

	if (isBoardErroneous(state) || state->numEmpty == 0) {
		/* board has errors or no moves to make, no possible solutions */
		*numSolutions = 0;
		return true;
	}

	initStack(&stack);

	if (!getNextEmptyBoardCell(&(state->puzzle), 0, 0, &curRow, &curCol)) {
		*numSolutions = 0;
		return true;
	}

	/* push initial cell */
	if (!pushStack(&stack, curRow, curCol)) {
		/* memory error */
		return false;
	}

	while (peekStack(&stack, &curRow, &curCol)) {
		Cell* cell;
		int nextRow, nextCol, newValue;
		bool isLegalValue;

		cell = getBoardCellByRow(&(state->puzzle), curRow, curCol);
		if (getBoardCellValue(cell) == MN) { /* max value */
			/* back track */
			setPuzzleCell(state, curRow, curCol, EMPTY_CELL_VALUE);
			popStack(&stack);
			continue;
		}

		/* increment value, not assuming EMPTY_CELL_VALUE == 0 */
		newValue = isBoardCellEmpty(cell) ? 1 : cell->value + 1;
		/* check if board is valid after incrementing value*/
		isLegalValue = isValueLegalForCell(state, curRow, curCol, newValue);
		/* set value anyway (after checking legality) */
		setPuzzleCell(state, curRow, curCol, newValue);
		
		if (!isLegalValue) {
			/* illegal, try next value */
			continue;
		}

		if (!getNextEmptyBoardCell(&(state->puzzle), curRow, curCol, &nextRow, &nextCol)) {
			/* no more empty cells, count as solved */
			sum++;
			continue;
		}

		/* count solutions for next empty cell given current board */
		if (!pushStack(&stack, nextRow, nextCol)) {
			/* memory error, undo every change */
			while(peekStack(&stack, &curRow, &curCol)) {
				setPuzzleCell(state, curRow, curCol, EMPTY_CELL_VALUE);
				popStack(&stack);
			}
			return false;
		}
	}

	*numSolutions = sum;
	return true;
}

bool autofill(State* state, Move** outMove) {
	int MN, row, col, val;
	bool filled = false;
	bool retValue = true;
	CellLegalValues** cellsLegalValues = NULL;

	Board board = {0};
	if (!exportBoard(state->gameState, &board)) {
		return false;
	}

	if (!getSuperficiallyLegalValuesForAllBoardCells(&(state->gameState->puzzle), &cellsLegalValues)) {
		cleanupBoard(&board);
		return false;
	}

	MN = getBlockSize_MN(state->gameState);
	for (row = 0; row < MN; row++) {
		for (col = 0; col < MN; col++) {
			CellLegalValues* legal = &cellsLegalValues[row][col];
			/* if only one legal value, set it in the board */
			if (isCellEmpty(state->gameState, row, col) &&
				legal->numLegalValues == 1) {
				for (val = 1; val <= MN; val++) {
					if (legal->legalValues[val]) {
						setBoardCellValue(&board, row, col, val);
						filled = true;
						break;
					}
				}
			}
		}
	}

	if (filled) {
		if (makeMultiCellMove(state, &board)) {
			*outMove = getCurrent(&(state->gameState->moveList));
		} else {
			retValue = false;
		}
	} else {
		*outMove = NULL;
	}

	cleanupBoard(&board);
	freeCellsLegalValuesForAllBoardCells(&(state->gameState->puzzle), cellsLegalValues);
	return retValue;
}

char* getPuzzleAsString(State* state) {
	char* str = NULL;

	str = getBoardAsString(&(state->gameState->puzzle), shouldMarkErrors(state));

	return str;
}
