#include <stdlib.h>

#include "game.h"

#include "undo_redo_list.h"
#include "rec_stack.h"

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
	UndoRedoList moveList;
};

int getNumEmptyCells(GameState* gameState) {
	return gameState->numEmpty;
}

int getNumErroneousCells(GameState* gameState) {
	return gameState->numErroneous;
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

int getBlockNumberByCell(Board* board, int row, int col) {
	return ((row / board->numRowsInBlock_M) * board->numRowsInBlock_M) +
			(col / board->numColumnsInBlock_N);
}

bool getNextEmptyBoardCell(Board* board, int row, int col, int* outRow, int* outCol) {
	Cell* curr;
	int MN = board->numColumnsInBlock_N * board->numRowsInBlock_M;

	while (row < MN) {
		curr = getBoardCellByRow(board, row, col);
		if (curr->value == EMPTY_CELL_VALUE) {
			*outRow = row;
			*outCol = col;
			return true;
		}

		if (col == MN - 1) {
			row = row + 1;
			col = 0;
		} else {
			/* row = row; */
			col = col + 1;
		}
	}

	return false;
}

int getNumEmptyBoardCells(Board* board) {
	int i, j;
	int numEmpty = 0;
	Cell* curr;
	int MN = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	for (i = 0; i < MN; i++) {
		for (j = 0; j < MN; j++) {
			 curr = getBoardCellByRow(board, i, j);
			if (curr->value == EMPTY_CELL_VALUE) {
				numEmpty++;
			}
		}
	}
	return numEmpty;
}

int getNumFilledBoardCells(Board* board) {
	int N, M, boardSize;
	N = board->numColumnsInBlock_N;
	M = board->numRowsInBlock_M;
	boardSize = N * N * M * M;
	return boardSize - getNumEmptyBoardCells(board);
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

/*int countNumErroneousCells(Board* board) {
	int numErroneous = 0;
	int NM = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int row = 0;
	int col = 0;

	for (row = 0; row < NM; row++)
		for (col = 0; col < NM; col++)
			if (isBoardCellErroneous(getBoardCellByRow(board, row, col)))
				numErroneous++;

	return numErroneous;
}*/

bool isBoardFilled(GameState* gameState) {
	return (gameState->numEmpty == 0);
}

bool isBoardErroneous(GameState* gameState) {
	return (gameState->numErroneous > 0);
}

bool isBoardSolvable(GameState* gameState) { /* TODO: this func */
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
			gameState->numErroneous = 0;
			updateCellsErroneousness(gameState);
			initUndoRedo(&(gameState->moveList)); /* TODO: have this freed at cleanupGameState */

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

bool checkErroneousCellsInCategory(Board* board, int categoryNo, getCellsByCategoryFunc getCellFunc, bool* outErroneous) {
	int NM = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int index = 0;

	int* valuesCounters = calloc(NM + 1, sizeof(int));
	if (valuesCounters == NULL)
		return false;

	*outErroneous = false;
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
			if (valuesCounters[value] > 1) {
				*outErroneous = true;
				break;
			}
		}
	}

	free(valuesCounters);
	return true;
}

bool checkErroneousCellsByCategory(Board* board, getCellsByCategoryFunc getCellFunc, bool* outErroneous) {
	int numCategories = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int index = 0;

	for (index = 0; index < numCategories; index++)
		if (!checkErroneousCellsInCategory(board, index, getCellFunc, outErroneous))
			return false;

	return true;
}

bool checkErroneousCells(Board* board, bool* outErroneous) {
	if (!checkErroneousCellsByCategory(board, getBoardCellByRow, outErroneous))
		return false;
	
	if (outErroneous) {
		return true;
	}

	if (!checkErroneousCellsByCategory(board, getBoardCellByColumn, outErroneous))
			return false;

	if (outErroneous) {
		return true;
	}

	if (!checkErroneousCellsByCategory(board, getBoardCellByBlock, outErroneous))
			return false;

	return true;
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

/****** From here on - Noa's additions *******/

void setCellValue(Board* board, int row, int col, int value) {
	board->cells[row][col].value = value;
}

/* Maintaining the invariant: at all times, all erroneous cells are marked
correctly after each change in the board. Returning the previous value
of that cell */
int setPuzzleCell(State* state, int row, int col, int value) {
	int prevValue = getCellValue(state->gameState, row, col);

	if (prevValue != EMPTY_CELL_VALUE) {
		state->gameState->rowsCellsValuesCounters[row][prevValue]--;
		state->gameState->columnsCellsValuesCounters[col][prevValue]--;
		state->gameState->blocksCellsValuesCounters[whichBlock(&(state->gameState->puzzle), row, col)][prevValue]--;
	}

	if (value != EMPTY_CELL_VALUE) {
		state->gameState->rowsCellsValuesCounters[row][value]++;
		state->gameState->columnsCellsValuesCounters[col][value]++;
		state->gameState->blocksCellsValuesCounters[whichBlock(&(state->gameState->puzzle), row, col)][value]++;
	}

	if (isCellEmpty(state->gameState, row, col) && value != EMPTY_CELL_VALUE) {
		state->gameState->numEmpty--;
	}
	if (!isCellEmpty(state->gameState, row, col) && value == EMPTY_CELL_VALUE) {
		state->gameState->numEmpty++;
	}
	setCellValue(&(state->gameState->puzzle), row, col, value);
	updateCellsErroneousness(state->gameState); /* TODO: could do something more efficient (going over just one row, col and block) */
	return prevValue;
}

/* returns false upon memory allocation error. This function also
causes the change in the board to be reflected in the undo-redo list  */
bool setPuzzleCellMove(State* state, int value, int row, int col) {
	int prevVal;
	Move* move = (Move*) calloc(1, sizeof(Move));
	if (move == NULL) { return false; }
	initList(&move->singleCellMoves);

	prevVal = setPuzzleCell(state, row, col, value);
	/*findErroneousCells(&(state->gameState->puzzle));*/

	if (addSingleCellMoveToMove(move, prevVal, value, col, row)) {
		return addNewMoveToList(&(state->gameState->moveList), move);
	}
	else {
		free(move);
		return false;
	}
}

void undoMove(State* state) {
	singleCellMove* scMove;
	Node* currNode;
	Move* moveToUndo = undoInList(&(state->gameState->moveList));
	if (moveToUndo == NULL) {
		return; /* nothing to undo */
	}
	currNode = moveToUndo->singleCellMoves.head;
	while (currNode != NULL) {
		scMove = (singleCellMove*) currNode->data;
		setPuzzleCell(state, scMove->row, scMove->col, scMove->prevVal);
		currNode = currNode->next;
	}
	findErroneousCells(&(state->gameState->puzzle));
}

void redoMove(State* state) {
	singleCellMove* scMove;
	Node* currNode;
	Move* moveToRedo = redoInList(&(state->gameState->moveList));
	if (moveToRedo == NULL) {
		return; /* nothing to redo */
	}
	currNode = moveToRedo->singleCellMoves.head;
	while (currNode != NULL) {
		scMove = (singleCellMove*) currNode->data;
		setPuzzleCell(state, scMove->row, scMove->col, scMove->newVal);
		currNode = currNode->next;
	}
	findErroneousCells(&(state->gameState->puzzle));
}

bool calculateNumSolutions(Board* board, int* numSolutions) {
	Stack stack;
	bool erroneous = false;
	int curCol, curRow;
	int sum = 0;
	int MN = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	initStack(&stack);

	/* check if board has any errors to begin with */
	if (!checkErroneousCells(board, &erroneous)) {
		return false;
	}

	if (erroneous) {
		/* board has errors, no possible solutions */
		*numSolutions = 0;
		return true;
	}

	if (!getNextEmptyBoardCell(board, 0, 0, &curRow, &curCol)) {
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
		int nextRow = 0, nextCol = 0;

		cell = getBoardCellByRow(board, curRow, curCol);
		if (getBoardCellValue(cell) == MN) { /* max value */
			/* back track */
			setCellValue(board, curRow, curCol, EMPTY_CELL_VALUE);
			popStack(&stack);
			continue;
		}

		/* increment value, not assuming EMPTY_CELL_VALUE == 0 */
		if (isBoardCellEmpty(cell)) {
			setCellValue(board, curRow, curCol, 1);
		} else {
			setCellValue(board, curRow, curCol, cell->value + 1);
		}

		/* check if board is still valid after incrementing value*/
		if (
			!checkErroneousCellsInCategory(board, curRow, getBoardCellByRow, &erroneous) ||	erroneous ||
			!checkErroneousCellsInCategory(board, curCol, getBoardCellByColumn, &erroneous) || erroneous ||
			!checkErroneousCellsInCategory(
				board, 
				getBlockNumberByCell(board, curRow, curCol), 
				getBoardCellByBlock,
				&erroneous) || erroneous)
		{
			if (erroneous) {
				continue;
			} else {
				/* memory error */
				while(popStack(&stack)) {
					/* empty the stack */
				}
				return false;	
			}
		}

		if (!getNextEmptyBoardCell(board, curRow, curCol, &nextRow, &nextCol)) {
			/* no more empty cells, count as solved */
			sum++;
			continue;
		}

		/* count solutions for next empty cell given current board */
		if (!pushStack(&stack, nextRow, nextCol)) {
			/* memory error */
			while(popStack(&stack)) {
				/* empty the stack */
			}
			return false;
		}
	}

	*numSolutions = sum;
	return true;
}

bool isSolutionSuccessful(GameState* gameState) {
	return (isBoardFilled(gameState)) &&
		   (!isBoardErroneous(gameState));
}

bool isSolutionFailing(GameState* gameState) {
	return (isBoardFilled(gameState)) &&
		   (isBoardErroneous(gameState));
}

