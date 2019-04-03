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

GameState* createNewGameState(State* state, int numRowsInBlock_M, int numColumnsInBlock_N) {
	int MN = numRowsInBlock_M * numColumnsInBlock_N;
	GameState* gameState = NULL;

	gameState = calloc(1, sizeof(GameState));
	if (gameState != NULL) {
		gameState->puzzle.numRowsInBlock_M = numRowsInBlock_M;
		gameState->puzzle.numColumnsInBlock_N = numColumnsInBlock_N;
		gameState->puzzle.cells = calloc(MN, sizeof(Cell*));
		if (gameState->puzzle.cells != NULL) {
			int row = 0;
			for (row = 0; row < MN; row++) {
				gameState->puzzle.cells[row] = calloc(MN, sizeof(Cell));
				if (gameState->puzzle.cells[row] == NULL) {
					break;
				}
			}
			if (row == MN) {
				state->gameState = gameState;
				return gameState;
			}
		}
	}
	cleanupGameState(state);
	return NULL;
}

void cleanupGameState(State* state) {
	GameState* gameState = state->gameState;
	int MN = 0;

	if (gameState == NULL)
		return;

	MN = gameState->puzzle.numRowsInBlock_M * gameState->puzzle.numColumnsInBlock_N;

	if (gameState->puzzle.cells != NULL) {
		 int row = 0;
		 for (row = 0; row < MN; row++) {
			 if (gameState->puzzle.cells[row] != NULL) {
				 free(gameState->puzzle.cells[row]);
				 gameState->puzzle.cells[row] = NULL;
			 }
		 }
		 free(gameState->puzzle.cells);
		 gameState->puzzle.cells = NULL;
	}

	free(gameState);
	state->gameState = NULL;

}
