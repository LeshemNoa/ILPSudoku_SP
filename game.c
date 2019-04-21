#include "game.h"

#define UNUSED(x) (void)(x)

#define GAME_MODE_INIT_STRING ("INIT")
#define GAME_MODE_EDIT_STRING ("EDIT")
#define GAME_MODE_SOLVE_STRING ("SOLVE")

typedef Cell* (*getCellsByCategoryFunc)(Board* board, int categoryNo, int indexInCategory);

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
};

int getNumEmptyCells(GameState* gameState) {
	return gameState->numEmpty;
}

int getNumColumnsInBlock_N(GameState* gameState) {
	return gameState->puzzle.numColumnsInBlock_N;
}

int getNumRowsInBlock_M(GameState* gameState) {
	return gameState->puzzle.numRowsInBlock_M;
}

int getBlockSize_MN(GameState* gameState) {
	return gameState->puzzle.numRowsInBlock_M * gameState->puzzle.numColumnsInBlock_N;
}

int getBoardSize_MN2(GameState* gameState) {
	int blockSize = getBlockSize_MN(gameState);
	return blockSize * blockSize;
}

bool isIndexInRange(GameState* gameState, int index) {
	return index >= 1 && index <= getBlockSize_MN(gameState);
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

void getRowBasedIDGivenRowBasedID(Board* board, int rowIn, int indexInRowIn, int* row, int* indexInRow) {
	UNUSED(board);

	*row = rowIn;
	*indexInRow = indexInRowIn;
}

Cell* getBoardCellByRow(Board* board, int row, int index) { /* Note: not to be exported */
	return &(board->cells[row][index]);
}

void getRowBasedIDGivenColumnBasedID(Board* board, int column, int indexInColumn, int* row, int* indexInRow) {
	UNUSED(board);

	*row = indexInColumn;
	*indexInRow = column;
}

Cell* getBoardCellByColumn(Board* board, int column, int index) { /* Note: not to be exported */
	int row = 0, indexInRow = 0;
	getRowBasedIDGivenColumnBasedID(board, column, index, &row, &indexInRow);
	return getBoardCellByRow(board, row, indexInRow);
}

void getRowBasedIDGivenBlockBasedID(Board* board, int block, int indexInBlock, int* row, int* indexInRow) {
	int colInBlocksMatrix = block % board->numRowsInBlock_M;
	int rowInBlocksMatrix = block / board->numRowsInBlock_M;
	int colInBlock = indexInBlock % board->numColumnsInBlock_N;
	int rowInBlock = indexInBlock / board->numColumnsInBlock_N;

	*row = rowInBlocksMatrix * board->numRowsInBlock_M + rowInBlock;
	*indexInRow = colInBlocksMatrix * board->numColumnsInBlock_N + colInBlock;
}

Cell* getBoardCellByBlock(Board* board, int block, int index) { /* Note: not to be exported (TODO: figure out why I wrote this)*/
	int row = 0, indexInRow = 0;
	getRowBasedIDGivenBlockBasedID(board, block, index, &row, &indexInRow);
	return getBoardCellByRow(board, row, indexInRow);
}

int whichBlock(Board* board, int row, int col) {
	int colInBlocksMatrix = col / board->numColumnsInBlock_N;
	int rowInBlocksMatrix = row / board->numRowsInBlock_M;

	return rowInBlocksMatrix * board->numRowsInBlock_M + colInBlocksMatrix;
}

bool isBoardCellFixed(Cell* cell) {
	return cell->isFixed;
}

bool isBoardCellErroneous(Cell* cell) {
	return cell->isErroneous;
}

int getBoardCellValue(Cell* cell) {
	return cell->value;
}

bool isBoardCellEmpty(Cell* cell) {
	return getBoardCellValue(cell) == EMPTY_CELL_VALUE;
}

void emptyBoardCell(Cell* cell) {
	cell->value = EMPTY_CELL_VALUE;
}

void setBoardCellFixedness(Cell* cell, bool isFixed) {
	cell->isFixed = isFixed;
}

void setBoardCellErroneousness(Cell* cell, bool isErroneous) {
	cell->isErroneous = isErroneous;
}

int countNumErroneousCells(Board* board) {
	int numErroneous = 0;
	int NM = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int row = 0;
	int col = 0;

	for (row = 0; row < NM; row++)
		for (col = 0; col < NM; col++)
			if (isBoardCellErroneous(getBoardCellByRow(board, row, col)))
				numErroneous++;

	return numErroneous;
}

bool isBoardErroneous(GameState* gameState) {
	return (gameState->numErroneous > 0);
}

bool isBoardSolvable(GameState* gameState) {
	UNUSED(gameState);

	return true;
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

void cleanupBoard(Board* boardInOut) {
	int MN = boardInOut->numRowsInBlock_M * boardInOut->numColumnsInBlock_N;

	if (boardInOut->cells != NULL) {
		 int row = 0;
		 for (row = 0; row < MN; row++) {
			 if (boardInOut->cells[row] != NULL) {
				 free(boardInOut->cells[row]);
				 boardInOut->cells[row] = NULL;
			 }
		 }
		 free(boardInOut->cells);
		 boardInOut->cells = NULL;
	}
}

bool createEmptyBoard(Board* boardInOut) {
	int MN = boardInOut->numRowsInBlock_M * boardInOut->numColumnsInBlock_N;
	boardInOut->cells = calloc(MN, sizeof(Cell*));
	if (boardInOut->cells != NULL) {
		int row = 0;
		for (row = 0; row < MN; row++) {
			boardInOut->cells[row] = calloc(MN, sizeof(Cell));
			if (boardInOut->cells[row] == NULL)
				break;
		}
		if (row == MN)
			return true;
	}

	cleanupBoard(boardInOut);
	return false;
}

int countNumEmptyCells(Board* board) {
	int numEmptyCells = 0;
	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	int row = 0;
	int col = 0;
	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++)
			if (isBoardCellEmpty(getBoardCellByRow(board, row, col)))
				numEmptyCells++;

	return numEmptyCells;
}

void freeSpecificCellsValuesCounters(int** cellValuesCounters, Board* board) { /* TODO: reconsider...: function needs only MN but I decided to have a Board parameter for symmetry's sake (with respect to the creating func) */
	int i = 0;
	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	if (cellValuesCounters == NULL)
		return;

	for (i = 0; i < MN; i++)
		if (cellValuesCounters[i] != NULL) {
			free(cellValuesCounters[i]);
			cellValuesCounters[i] = NULL;
		}

	free(cellValuesCounters);
}

int** allocateNewSpecificCellsValuesCounters(Board* board) {
	int** cellsValuesCounters = NULL;
	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	cellsValuesCounters = calloc(MN, sizeof(int*));
	if (cellsValuesCounters != NULL) {
		int success = true;
		int i = 0;
		for (i = 0; i < MN; i++) {
			cellsValuesCounters[i] = calloc(MN + 1, sizeof(int)); /* Note: +1 for convenience, because values are 1-based */
			if (cellsValuesCounters[i] == NULL) {
				success = false;
				break;
			}
		}

		if (!success) {
			freeSpecificCellsValuesCounters(cellsValuesCounters, board);
			cellsValuesCounters = NULL;
		}
	}

	return cellsValuesCounters;
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

void updateCellsValuesCountersInCategory(int* categoryNoCellsValuesCounters, Board* board, int categoryNo, getCellsByCategoryFunc getCellFunc) {
	int NM = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int index = 0;

	for (index = 0; index < NM; index++) {
		Cell* cell = getCellFunc(board, categoryNo, index);
		if (!isBoardCellEmpty(cell)) {
			int value = getBoardCellValue(cell);
			categoryNoCellsValuesCounters[value]++;
		}
	}
}

void updateCellsValuesCountersByCategory(int** categoryCellsValuesCounters, Board* board, getCellsByCategoryFunc getCellFunc) {
	int numCategories = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int categoryIndex = 0;

	for (categoryIndex = 0; categoryIndex < numCategories; categoryIndex++)
		updateCellsValuesCountersInCategory(categoryCellsValuesCounters[categoryIndex], board, categoryIndex, getCellFunc);
}

/*void updateCellsValuesCounters(GameState* gameState) {
	updateCellsValuesCountersByCategory(gameState->rowsCellsValuesCounters, &(gameState->puzzle), getBoardCellByRow);
	updateCellsValuesCountersByCategory(gameState->columnsCellsValuesCounters, &(gameState->puzzle), getBoardCellByColumn);
	updateCellsValuesCountersByCategory(gameState->blocksCellsValuesCounters, &(gameState->puzzle), getBoardCellByBlock);
}*/

void updateCellErroneousness(GameState* gameState, int row, int col) {
	Cell* cell = getBoardCellByRow(&(gameState->puzzle), row, col);
	if (!isBoardCellEmpty(cell)) {
		int oldErroneousness = isBoardCellErroneous(cell);
		int deltaInNumErroneous = 0;

		int block = whichBlock(&(gameState->puzzle), row, col);
		int value = getBoardCellValue(cell);
		bool erroneousness = (gameState->rowsCellsValuesCounters[row][value] > 1) ||
							 (gameState->columnsCellsValuesCounters[col][value] > 1) ||
							 (gameState->blocksCellsValuesCounters[block][value] > 1);

		setBoardCellErroneousness(cell, erroneousness);

		if ((oldErroneousness == true) && (erroneousness == false))
			deltaInNumErroneous = -1;
		else if ((oldErroneousness == false) && (erroneousness == true))
			deltaInNumErroneous = +1;

		gameState->numErroneous += deltaInNumErroneous;
	}

}

void updateCellErroneousnessInRow(GameState* gameState, int row) { /* TODO: should be called by, for example, set (erroneousness of cells in the row might have changed) */
	int NM = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < NM; i++)
		updateCellErroneousness(gameState, row, i);
}

void updateCellErroneousnessInColumn(GameState* gameState, int col) { /* TODO: should be called by, for example, set (erroneousness of cells in the column might have changed) */
	int NM = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < NM; i++)
		updateCellErroneousness(gameState, i, col);
}

void updateCellErroneousnessInBlock(GameState* gameState, int block) { /* TODO: should be called by, for example, set (erroneousness of cells in the block might have changed) */
	int NM = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int i = 0;
	for (i = 0; i < NM; i++) {
		int row = 0, col = 0;
		getRowBasedIDGivenBlockBasedID(&(gameState->puzzle), block, i, &row, &col);
		updateCellErroneousness(gameState, row, col);
	}

}

void updateCellsErroneousness(GameState* gameState) {
	int NM = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;
	int row = 0, col = 0;

	for (row = 0; row < NM; row++)
		for (col = 0; col < NM; col++)
			updateCellErroneousness(gameState, row, col);
}

int** createCellsValuesCountersByCategory(Board* board, getCellsByCategoryFunc getCellFunc) {
	int** cellsValuesCounters = NULL;

	cellsValuesCounters = allocateNewSpecificCellsValuesCounters(board);
	if (cellsValuesCounters != NULL) {
		updateCellsValuesCountersByCategory(cellsValuesCounters, board, getCellFunc);
	}

	return cellsValuesCounters;
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

GameState* createGameState(Board* board) {
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
			updateCellsErroneousness(gameState);
			gameState->numErroneous = countNumErroneousCells(&(gameState->puzzle));

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

	free(gameState);
}

void setCellFixedness(GameState* gameState, int row, int col, bool isFixed) {
	setBoardCellFixedness(getBoardCellByRow(&(gameState->puzzle), row, col), isFixed);
}

void setCellErroneousness(GameState* gameState, int row, int col, bool isFixed) {
	setBoardCellErroneousness(getBoardCellByRow(&(gameState->puzzle), row, col), isFixed);
}

void markAllCellsAsNotFixed(GameState* gameState) {
	int nm = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;

	int row = 0;
	int col = 0;
	for (row = 0; row < nm; row++)
		for (col = 0; col < nm; col++)
			setCellFixedness(gameState, row, col, false);
}

void zeroArray(int* arr, int size) {
	int i = 0;
	for (i = 0; i < size; i++)
		arr[i] = 0;
}

bool findErroneousCellsInCategory(Board* board, int categoryNo, getCellsByCategoryFunc getCellFunc) {
	int NM = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int index = 0;

	int* valuesCounters = calloc(NM + 1, sizeof(int));
	if (valuesCounters == NULL)
		return false;

	for (index = 0; index < NM; index++) {
		Cell* cell = getCellFunc(board, categoryNo, index);
		if (!isBoardCellEmpty(cell)) {
			int value = getBoardCellValue(cell);
			valuesCounters[value]++;
		}
	}

	for (index = 0; index < NM; index++) {
		Cell* cell = getCellFunc(board, categoryNo, index);
		if (!isBoardCellEmpty(cell)) {
			int value = getBoardCellValue(cell);
			if (valuesCounters[value] > 1)
				setBoardCellErroneousness(cell, true);
		}
	}

	free(valuesCounters);
	return true;
}

bool findErroneousCellsByCategory(Board* board, getCellsByCategoryFunc getCellFunc) {
	int numCategories = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int index = 0;

	for (index = 0; index < numCategories; index++)
		if (!findErroneousCellsInCategory(board, index, getCellFunc))
			return false;

	return true;
}

bool findErroneousCells(Board* board) {
	if (!findErroneousCellsByCategory(board, getBoardCellByRow))
		return false;

	if (!findErroneousCellsByCategory(board, getBoardCellByColumn))
			return false;

	if (!findErroneousCellsByCategory(board, getBoardCellByBlock))
			return false;

	return true;
}

bool copyBoard(Board* boardIn, Board* boardOut) {
	int row = 0;
	int col = 0;
	int nm = 0;

	boardOut->numRowsInBlock_M = boardIn->numRowsInBlock_M;
	boardOut->numColumnsInBlock_N = boardIn->numColumnsInBlock_N;
	if (!createEmptyBoard(boardOut)) {
		return false;
	}

	nm = boardOut->numColumnsInBlock_N * boardOut->numRowsInBlock_M;

	for (row = 0; row < nm; row++)
		for (col = 0; col < nm; col++) {
			Cell* cellOut = getBoardCellByRow(boardOut, row, col);
			Cell* cellIn = getBoardCellByRow(boardIn, row, col);
			*cellOut = *cellIn;
		}

	return true;
}

bool exportBoard(GameState* gameState, Board* boardInOut) {
	return copyBoard(&(gameState->puzzle), boardInOut);
}

void cleanupCellLegalValuesStruct(CellLegalValues* cellLegalValuesInOut) {
	if (cellLegalValuesInOut == NULL)
		return;

	if (cellLegalValuesInOut->legalValues != NULL) {
		free(cellLegalValuesInOut->legalValues);
		cellLegalValuesInOut->legalValues = NULL;
	}

	cellLegalValuesInOut->numLegalValues = 0;
}

bool isValueLegalForCell(GameState* gameState, int row, int col, int value) {
	int block = whichBlock(&(gameState->puzzle), row, col);
	return (gameState->rowsCellsValuesCounters[row][value] == 0) &&
		   (gameState->columnsCellsValuesCounters[col][value] == 0) &&
		   (gameState->blocksCellsValuesCounters[block][value] == 0);
}

bool fillCellLegalValuesStruct(GameState* gameState, int row, int col, CellLegalValues* cellLegalValuesInOut) {
	int MN = gameState->puzzle.numRowsInBlock_M * gameState->puzzle.numColumnsInBlock_N;
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

bool getSuperficiallyLegalValuesForCell(GameState* gameStateIn, Board* boardIn, int row, int col, CellLegalValues* cellLegalValuesInOut) {
	bool retValue = true;

	GameState* gameState = NULL;
	bool shouldCleanUpGameState = false;

	if ((gameStateIn != NULL) && (boardIn == NULL)) {
		shouldCleanUpGameState = false;
		gameState = gameStateIn;
	} else if ((gameStateIn == NULL) && (boardIn != NULL)) {
		shouldCleanUpGameState = true;
		gameState = createGameState(boardIn);
		if (gameState == NULL)
			return false;

	} else {
		return false;
	}

	if (!fillCellLegalValuesStruct(gameState, row, col, cellLegalValuesInOut)) {
		cleanupCellLegalValuesStruct(cellLegalValuesInOut);
		retValue = false;
	}

	if (shouldCleanUpGameState)
		cleanupGameState(gameState);

	return retValue;
}

void freeCellsLegalValuesForAllCells(GameState* gameStateIn, Board* boardIn, CellLegalValues** cellsLegalValuesOut) {
	int MN = 0;
	int row = 0, col = 0;

	Board* board = NULL;

	if ((gameStateIn != NULL) && (boardIn == NULL)) {
		board = &(gameStateIn->puzzle);
	} else if ((gameStateIn == NULL) && (boardIn != NULL)) {
		board = boardIn;
	} else {
		return;
	}

	MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	if (cellsLegalValuesOut == NULL)
		return;

	for (row = 0; row < MN; row++)
		if (cellsLegalValuesOut[row] != NULL) {
			for (col = 0; col < MN; col++)
				cleanupCellLegalValuesStruct(&(cellsLegalValuesOut[row][col]));
			free(cellsLegalValuesOut[row]);
			cellsLegalValuesOut[row] = NULL;
		}

	free(cellsLegalValuesOut);
}

bool getSuperficiallyLegalValuesForAllCells(GameState* gameStateIn, Board* boardIn, CellLegalValues*** cellsLegalValuesOut) {
	bool retValue = true;
	CellLegalValues** cellsLegalValues = NULL;

	int MN = 0;

	GameState* gameState = NULL;
	bool shouldCleanUpGameState = false;

	if ((gameStateIn != NULL) && (boardIn == NULL)) {
		shouldCleanUpGameState = false;
		gameState = gameStateIn;
	} else if ((gameStateIn == NULL) && (boardIn != NULL)) {
		shouldCleanUpGameState = true;
		gameState = createGameState(boardIn);
		if (gameState == NULL)
			return false;
	} else {
		return false;
	}

	MN = gameState->puzzle.numRowsInBlock_M * gameState->puzzle.numColumnsInBlock_N;

	cellsLegalValues = calloc(MN, sizeof(CellLegalValues*));
	if (cellsLegalValues == NULL)
		retValue = false;
	else {
		int row = 0;
		for (row = 0; row < MN; row++) {
			cellsLegalValues[row] = calloc(MN, sizeof(CellLegalValues));
			if (cellsLegalValues[row] == NULL) {
				retValue = false;
				break;
			} else {
				int col = 0;
				for (col = 0; col < MN; col++)
					if (!getSuperficiallyLegalValuesForCell(gameState, NULL, row, col, &(cellsLegalValues[row][col]))) {
						retValue = false;
						break;
					}
				if (!retValue)
					break;
			}
		}
	}

	if (!retValue)
		freeCellsLegalValuesForAllCells(gameState, NULL, cellsLegalValues);
	else
		*cellsLegalValuesOut = cellsLegalValues;

	if (shouldCleanUpGameState)
		cleanupGameState(gameState);

	return retValue;
}

void setTempFunc(GameState* gameState, int row, int indexInRow, int value) { /* TODO: will be replaced with actual set func from other branch */
	Cell* cell = getBoardCellByRow(&(gameState->puzzle), row, indexInRow);

	if (!isBoardCellEmpty(cell)) {
		int oldValue = cell->value;
		gameState->rowsCellsValuesCounters[row][oldValue]--;
		gameState->columnsCellsValuesCounters[indexInRow][oldValue]--;
		gameState->blocksCellsValuesCounters[whichBlock(&(gameState->puzzle), row, indexInRow)][oldValue]--;
	} else {
		gameState->numEmpty--;
	}

	cell->value = value;
	gameState->rowsCellsValuesCounters[row][value]++;
	gameState->columnsCellsValuesCounters[indexInRow][value]++;
	gameState->blocksCellsValuesCounters[whichBlock(&(gameState->puzzle), row, indexInRow)][value]++;

	updateCellsErroneousness(gameState); /* TODO: could do something more efficient (going over just one row, col and block) */
}
