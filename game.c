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
	int numRowsInBlock_M;
	int numColumnsInBlock_N;
	Board puzzle;
	int numEmpty;
	int numErroneous;
};

int getNumEmptyCells(GameState* gameState) {
	return gameState->numEmpty;
}

int getBlockSize_MN(GameState* gameState) {
	return gameState->numRowsInBlock_M * gameState->numColumnsInBlock_N;
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
	UNUSED(gameState);
	UNUSED(row);
	UNUSED(col);

	return true;
}

bool isCellFixed(GameState* gameState, int row, int col) {
	UNUSED(gameState);
	UNUSED(row);
	UNUSED(col);

	return false;
}

bool isThereMoveToUndo(GameState* gameState) {
	UNUSED(gameState);

	return false;
}

bool isThereMoveToRedo(GameState* gameState) {
	UNUSED(gameState);

	return false;
}
