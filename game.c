#include "game.h"

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
	bool shouldMarkErrors;
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

bool isBoardErroneous(GameState* gameState) {
	UNUSED(gameState);

	return false;
}

bool isBoardSolvable(GameState* gameState) {
	UNUSED(gameState);

	return true;
}

bool isCellEmpty(GameState* gameState, int row, int col) {
	return gameState->puzzle.cells[row][col].value == EMPTY_CELL_VALUE;
}

bool isCellFixed(GameState* gameState, int row, int col) {
	return gameState->puzzle.cells[row][col].isFixed;
}

bool isCellErroneous(GameState* gameState, int row, int col) {
	return gameState->puzzle.cells[row][col].isErroneous;
}

int getCellValue(GameState* gameState, int row, int col) {
	return gameState->puzzle.cells[row][col].value;
}

bool shouldMarkErrors(GameState* gameState) {
	return gameState->shouldMarkErrors;
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

void setCellFixedness(GameState* gameState, int row, int col, bool isFixed) {
	gameState->puzzle.cells[row][col].isFixed = isFixed;
}

void markAllCellsAsNotFixed(GameState* gameState) {
	int nm = gameState->puzzle.numColumnsInBlock_N * gameState->puzzle.numRowsInBlock_M;

	int row = 0;
	int col = 0;
	for (row = 0; row < nm; row++)
		for (col = 0; col < nm; col++)
			setCellFixedness(gameState, row, col, false);
}
