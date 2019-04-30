/**
 * GAME Summary:
 *
 * A module designed to allow for a Sudoku game to be prepared and later 
 * run from start to finish; in this we include a whole game (multiple
 * boards), and also one instance (i.e., a single Sudoku board).
 */

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


const Board* getPuzzle(GameState* gameState) { /* Note: that this function mustn't be exported */
	return &(gameState->puzzle);
}

int getNumEmptyCells(GameState* gameState) {
	return gameState->numEmpty;
}

int getNumErroneousCells(GameState* gameState) {
	return gameState->numErroneous;
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

	switch (getBoardSolution(getPuzzle(gameState), solutionOut)) {
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

/**
 * Free all memory allocated to all counter matrices in the GameState.
 * 
 * @param gameState 	[in, out] GameState whose counter matrices are freed
 */
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

/**
 * Checks whether the cell with the provided indices is erroneous and reflects the
 * consequences in the provided GameState: updates the numErroneous field and marks
 * the cell as erroneous if necessary.
 * 
 * @param gameState 	[in, out] GameState to be examined and updated accordingly
 * @param row 			[in] The number of row in which this cell is located
 * @param col 			[in] The number of column in which this cell is located
 */
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

/**
 * Update the erroneousness status of all cells in a row.
 * 
 * @param gameState 	[in, out] GameState to be examined and updated accordingly
 * @param row 			[in] Number of row to update
 */
void updateCellErroneousnessInRow(GameState* gameState, int row) {
	int MN = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < MN; i++)
		updateCellErroneousness(gameState, row, i);
}

/**
 * Update the erroneousness status of all cells in a column.
 * 
 * @param gameState 	[in, out] GameState to be examined and updated accordingly
 * @param col 			[in] Number of column to update
 */
void updateCellErroneousnessInColumn(GameState* gameState, int col) {
	int MN = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < MN; i++)
		updateCellErroneousness(gameState, i, col);
}

/**
 * Update the erroneousness status of all cells in a block.
 * 
 * @param gameState 	[in, out] GameState to be examined and updated accordingly
 * @param block 		[in] Number of block to update
 */
void updateCellErroneousnessInBlock(GameState* gameState, int block) {
	int MN = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < MN; i++) {
		int row = 0, col = 0;
		getRowBasedIDGivenBlockBasedID(&(gameState->puzzle), block, i, &row, &col);
		updateCellErroneousness(gameState, row, col);
	}

}

/**
 * Update the erroneousness status of all cells in the provided GameState's board.
 * 
 * @param gameState 	[in, out] GameState to be examined and updated accordingly
 */
void updateCellsErroneousness(GameState* gameState) {
	int MN = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int row = 0, col = 0;

	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++)
			updateCellErroneousness(gameState, row, col);
}

/**
 * Create all three counter matrices for the provided GameState's board, for rows, columns
 * and blocks.
 * 
 * @param gameState 	[in, out] GameState for whom the counters are created
 * @return true 		iff the procedure was successful and all counters were assigned
 * @return false 		iff a memory error occurred
 */
bool createCellsValuesCounters(GameState* gameState) {
	const Board* puzzle = getPuzzle(gameState);

	gameState->rowsCellsValuesCounters = createCellsValuesCountersByCategory(puzzle, viewBoardCellByRow);
	gameState->columnsCellsValuesCounters = createCellsValuesCountersByCategory(puzzle, viewBoardCellByColumn);
	gameState->blocksCellsValuesCounters = createCellsValuesCountersByCategory(puzzle, viewBoardCellByBlock);

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

/**
 * Set the provided cell's isFixed marker according to the provided boolean argument.
 * 
 * @param gameState 	[in, out] GameState whose board's cell is updated
 * @param row 			[in] The number of row in which this cell is located
 * @param col 			[in] The number of column in which this cell is located
 * @param isFixed 		[in] the value to place in the cell's isFixed field
 */
void setCellFixedness(GameState* gameState, int row, int col, bool isFixed) {
	setBoardCellFixedness(getBoardCellByRow(&(gameState->puzzle), row, col), isFixed);
}

/**
 * Set the provided cell's isErroneous marker according to the provided boolean argument.
 * 
 * @param gameState 		[in, out] GameState whose board's cell is updated
 * @param row 				[in] The number of row in which this cell is located
 * @param col 				[in] The number of column in which this cell is located
 * @param isErroneous 		[in] the value to place in the cell's isErroneous field
 */
void setCellErroneousness(GameState* gameState, int row, int col, bool isErroneous) {
	setBoardCellErroneousness(getBoardCellByRow(&(gameState->puzzle), row, col), isErroneous);
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

bool makeCellChangeMove(GameState* gameState, int value, int row, int col) {
	int prevVal;
	Move* move = createMove();
	if (move == NULL) { return false; }

	prevVal = setPuzzleCell(gameState, row, col, value);
	if (prevVal == value) {
		/* not documenting move if nothing has changed */
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

/**
 * Apply the changes caused by an Undo or a Redo of the provided Move to the board.
 * 
 * @param gameState 	[in, out] GameState to be updated
 * @param move 			[in] Move to undo or redo
 * @param undo 			iff true, move is undo. Else, move is redone
 */
void applyMoveToBoard(GameState* gameState, const Move* move, bool undo) {
	const Node* currNode;
	CellChange* change;

	currNode = getFirstCellChange(move);
	while (currNode != NULL) {
		change = (CellChange*) getNodeData(currNode);
		setPuzzleCell(gameState, change->row, change->col, undo ? change->prevVal : change->newVal);
		currNode = getNodeNext(currNode);
	}
}


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

	if (!diff) {
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

const Move* undoMove(GameState* gameState) {
	const Move* moveToUndo = undoInList(&(gameState->moveList));
	if (moveToUndo == NULL) {
		return NULL; /* nothing to undo */
	}
	applyMoveToBoard(gameState, moveToUndo, true);
	return moveToUndo;
}

const Move* redoMove(GameState* gameState) {
	const Move* moveToRedo = redoInList(&(gameState->moveList));
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

	/* if move had any changes */
	if (getMoveSize(move) > 0) {
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
		destroyMove(move);
		*outMove = NULL;
	}

	return true;
}

char* getPuzzleAsString(State* state) {
	char* str = NULL;

	str = getBoardAsString(&(state->gameState->puzzle), shouldMarkErrors(state));

	return str;
}
