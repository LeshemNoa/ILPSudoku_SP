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

Cell* getBoardCellByRow(Board* board, int row, int index) { /* Note: not to be exported */
	return &(board->cells[row][index]);
}

Cell* getBoardCellByColumn(Board* board, int column, int index) { /* Note: not to be exported */
	return getBoardCellByRow(board, index, column);
}

Cell* getBoardCellByBlock(Board* board, int block, int index) { /* Note: not to be exported */
	int colInBlocksMatrix = block % board->numRowsInBlock_M;
	int rowInBlocksMatrix = block / board->numRowsInBlock_M;
	int colInBlock = index % board->numColumnsInBlock_N;
	int rowInBlock = index / board->numColumnsInBlock_N;

	int row = rowInBlocksMatrix * board->numRowsInBlock_M + rowInBlock;
	int col = colInBlocksMatrix * board->numColumnsInBlock_N + colInBlock;

	return getBoardCellByRow(board, row, col);
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

int getNumErroneousCells(Board* board) {
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

Board* createBoard(Board* boardInOut, int numRowsInBlock_M, int numColumnsInBlock_N) {
	Board tempBoard;

	int MN = numRowsInBlock_M * numColumnsInBlock_N;
	tempBoard.cells = calloc(MN, sizeof(Cell*));
	if (tempBoard.cells != NULL) {
		int row = 0;
		for (row = 0; row < MN; row++) {
			tempBoard.cells[row] = calloc(MN, sizeof(Cell));
			if (tempBoard.cells[row] == NULL) {
				break;
			}
		}
		if (row == MN) {
			boardInOut->cells = tempBoard.cells;
			boardInOut->numRowsInBlock_M = numRowsInBlock_M;
			boardInOut->numColumnsInBlock_N = numColumnsInBlock_N;
			return boardInOut;
		}
	}
	cleanupBoard(boardInOut);
	return NULL;
}

GameState* createGameState(State* state, int numRowsInBlock_M, int numColumnsInBlock_N, Board* board) {
	GameState* gameState = NULL;

	gameState = calloc(1, sizeof(GameState));
	if (gameState != NULL) {
		if (board == NULL) {
			Board* board = NULL;
			board = createBoard(&(gameState->puzzle), numRowsInBlock_M, numColumnsInBlock_N);
			if (board != NULL) {
				state->gameState = gameState;
				return gameState;
			}
		} else {
			gameState->puzzle = *board; /* Note: we copy the struct, hence the pre-allocation Cells** within the given board is retained */
			gameState->numErroneous = getNumErroneousCells(board);
			state->gameState = gameState;
			return gameState;
		}
	}
	cleanupGameState(state);
	return NULL;
}

void cleanupGameState(State* state) {
	GameState* gameState = state->gameState;

	if (gameState == NULL)
		return;

	cleanupBoard(&(gameState->puzzle));

	free(gameState);
	state->gameState = NULL;
}

void setBoardCellFixedness(Cell* cell, bool isFixed) {
	cell->isFixed = isFixed;
}

void setCellFixedness(GameState* gameState, int row, int col, bool isFixed) {
	setBoardCellFixedness(getBoardCellByRow(&(gameState->puzzle), row, col), isFixed);
}

void setBoardCellErroneousness(Cell* cell, bool isErroneous) {
	if (!isErroneous) {
		cell->isErroneous = false;
		return;
	}

	if (!isBoardCellFixed(cell))
		cell->isErroneous = true;
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

bool exportBoard(GameState* gameState, Board* boardInOut) {
	int row = 0;
	int col = 0;
	int nm = 0;

	if (!createBoard(boardInOut, gameState->puzzle.numRowsInBlock_M, gameState->puzzle.numColumnsInBlock_N)) {
		return false;
	}

	nm = boardInOut->numColumnsInBlock_N * boardInOut->numRowsInBlock_M;

	for (row = 0; row < nm; row++)
		for (col = 0; col < nm; col++)
			*(getBoardCellByRow(boardInOut, row, col)) = *(getBoardCellByRow(&(gameState->puzzle), row, col));

	return true;
}
