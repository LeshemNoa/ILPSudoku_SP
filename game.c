#include <stdlib.h>

#include "game.h"

#include "undo_redo_list.h"

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

Board* getPuzzle(GameState* gameState) { /* Note: that this function mustn't be exported */
	return &(gameState->puzzle);
}

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

	cleanupUndoRedoList(&(gameState->moveList));

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
int setPuzzleCell(GameState* gameState, int row, int col, int value) {
	int block, prevValue = getCellValue(gameState, row, col);
	if (prevValue == value) {
		/* Nothing to be done */
		return value;
	}

	block = whichBlock(&(gameState->puzzle), row, col);

	if (prevValue != EMPTY_CELL_VALUE) {
		gameState->rowsCellsValuesCounters[row][prevValue]--;
		gameState->columnsCellsValuesCounters[col][prevValue]--;
		gameState->blocksCellsValuesCounters[block][prevValue]--;
	}

	if (value != EMPTY_CELL_VALUE) {
		gameState->rowsCellsValuesCounters[row][value]++;
		gameState->columnsCellsValuesCounters[col][value]++;
		gameState->blocksCellsValuesCounters[block][value]++;
	}

	if (isCellEmpty(gameState, row, col) && value != EMPTY_CELL_VALUE) {
		gameState->numEmpty--;
	}
	if (!isCellEmpty(gameState, row, col) && value == EMPTY_CELL_VALUE) {
		gameState->numEmpty++;
	}
	setBoardCellValue(&(gameState->puzzle), row, col, value);
	updateCellErroneousnessInRow(gameState, row);
	updateCellErroneousnessInColumn(gameState, col);
	updateCellErroneousnessInBlock(gameState, block);
	return prevValue;
}

/* returns false upon memory allocation error. This function also
causes the change in the board to be reflected in the undo-redo list  */
bool makeCellChangeMove(GameState* gameState, int value, int row, int col) {
	int prevVal;
	Move* move = createMove();
	if (move == NULL) { return false; }

	prevVal = setPuzzleCell(gameState, row, col, value);
	if (prevVal == value) {
		/* not documenting move if nothing has changed */ /* CR: alright. but again, what happens is autofill or generate fail to change anything? do we have an empty move being added to undo-redo list? */
		destroyMove(move);
		return true;
	}

	if (!addCellChangeToMove(move, prevVal, value, row, col) || 
		!addNewMoveToList(&(gameState->moveList), move)) {
		destroyMove(move);
		return false;
	}

	return true;
}

void applyMoveToBoard(GameState* gameState, const Move* move, bool undo) {
	Node* currNode;
	CellChange* change;

	currNode = getFirstCellChange(move);
	while (currNode != NULL) {
		change = (CellChange*) getNodeData(currNode);
		setPuzzleCell(gameState, change->row, change->col, undo ? change->prevVal : change->newVal);
		currNode = getNodeNext(currNode);
	}
}


/* returns false upon memory allocation error. This function also
causes the change in the board to be reflected in the undo-redo list  */
bool makeMove(GameState* gameState, Move* move) {
	if (move == NULL) { return false; }

	/* add move to move list */
	if (!addNewMoveToList(&(gameState->moveList), move)) {
		return false;
	}

	/* perform all single cell moves */
	applyMoveToBoard(gameState, move, false);
	return true;
}

/* returns false upon memory allocation error. This function also
causes the change in the board to be reflected in the undo-redo list  */
bool makeMultiCellMove(GameState* gameState, Board* newBoard) {
	bool diff = false;
	int MN, row, col, prevVal, newValue;
	Move* move = createMove();
	if (move == NULL) { return false; }
	MN = getBlockSize_MN(gameState);

	/* find all differences and build single cell move list */
	for (row = 0; row < MN; row++) {
		for (col = 0; col < MN; col++) {
			prevVal = getCellValue(gameState, row, col);
			newValue = getBoardCellValue(getBoardCellByRow(newBoard, row, col));
			if (prevVal != newValue) {
				diff = true;
				if (!addCellChangeToMove(move, prevVal, newValue, row, col)) {
					/* memory error, cleanup and return */
					destroyMove(move);
					return false;
				}
			}
		}
	}

	if (!diff) { /* CR: well that settles it for generate. How about autofill and guess? (and set) */
		/* not documenting move if nothing has changed */
		destroyMove(move);
		return true;
	}

	if (!makeMove(gameState, move)) {
		/* memory error, cleanup and return */
		destroyMove(move);
		return false;
	}

	return true;
}

const Move* undoMove(GameState* gameState) { /* CR: const is a cool solution */
	Move* moveToUndo = undoInList(&(gameState->moveList));
	if (moveToUndo == NULL) {
		return NULL; /* nothing to undo */
	}
	applyMoveToBoard(gameState, moveToUndo, true);
	return moveToUndo;
}

const Move* redoMove(GameState* gameState) {
	Move* moveToRedo = redoInList(&(gameState->moveList));
	if (moveToRedo == NULL) {
		return NULL; /* nothing to redo */
	}
	applyMoveToBoard(gameState, moveToRedo, false);
	return moveToRedo;
}

bool resetMoves(GameState* gameState) {
	bool changed = false;
    while (canUndo(&(gameState->moveList))) {
		undoMove(gameState);
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

bool autofill(GameState* gameState, Move** outMove) {
	int MN, row, col, val;
	CellLegalValues** cellsLegalValues = NULL;
	Move* move = createMove();
	if (move == NULL) {
		return false;
	}

	if (!getSuperficiallyLegalValuesForAllBoardCells(&(gameState->puzzle), &cellsLegalValues)) {
		destroyMove(move);
		return false;
	}

	MN = getBlockSize_MN(gameState);
	for (row = 0; row < MN; row++) {
		for (col = 0; col < MN; col++) {
			CellLegalValues* legalValuesStruct = &(cellsLegalValues[row][col]);
			/* if only one legal value, set it in the board */
			if (isCellEmpty(gameState, row, col) &&
				legalValuesStruct->numLegalValues == 1) {
				for (val = 1; val <= MN; val++) {
					if (legalValuesStruct->legalValues[val]) {
						if (!addCellChangeToMove(move, EMPTY_CELL_VALUE, val, row, col)) {
							destroyMove(move);
							freeCellsLegalValuesForAllBoardCells(&(gameState->puzzle), cellsLegalValues);
							return false;
						}
						break;
					}
				}
			}
		}
	}
	/* we are done with this */
	freeCellsLegalValuesForAllBoardCells(&(gameState->puzzle), cellsLegalValues);

	/* if move had any changes */ /* CR: and that settles it for autofill */
	if (getCellChangesSize(move) > 0) {
		/* add move to move list */
		if (!addNewMoveToList(&(gameState->moveList), move)) {
			/* memory error, cleanup and return */
			destroyMove(move);
			return false;
		}

		/* perform all single cell moves on board */
		applyMoveToBoard(gameState, move, false);
		*outMove = move;
	} else {
		*outMove = NULL;
	}

	return true;
}

char* getPuzzleAsString(State* state) {
	char* str = NULL;

	str = getBoardAsString(&(state->gameState->puzzle), shouldMarkErrors(state));

	return str;
}
