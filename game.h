/**
 * GAME Summary:
 *
 * A module designed to allow for a sudoku game to be run from start to finish.
 *
 * initialise - Creates a new sudoku game
 * destruct - demolishes a sudoku game when it's of no use
 * exportBoard - exports a boarding representing the current GameState of the game
 * isGameWon - checks whether the game is over
 * getCellValue - returns the value of a specific cell in the board
 * setCellValue - sets the value of a specific cell in the board
 * isCellValueValid - checks if a value is valid for a specific cell in the board
 * isCellFixed - checks whether a certain cell is fixed in the board
 * isCellEmpty - checks whether a certain cell is empty (devoid of a value)
 * emptyCell - empties a cell
 * set - used to accommodate a 'set' command from the user
 * hint - used to accommodate a 'hint' command from the user
 * setPuzzleSolution - sets the stored solution of a sudoku game
 */

#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>

#include "board.h"
#include "move.h"

/**
 *  GameState struct represents a sudoku game in its current GameState.
 *  In order to run a game of sudoku, one must acquire (through the initialise
 *  function) such a struct, and then use it to bring about the actual game.
 */
typedef struct GameState GameState;

typedef enum GameMode {
	GAME_MODE_INIT,
	GAME_MODE_EDIT,
	GAME_MODE_SOLVE} GameMode;

typedef struct {
	GameMode gameMode;
	bool shouldHideErrors;
	GameState* gameState;} State;

const Board* getPuzzle(GameState* gameState); /* CR: the whole point of hiding GameState's implementation was that the Board wouldn't be accessible. This function cannot appear in the header of game */

int getNumEmptyCells(GameState* gameState);

int getNumColumnsInBlock_N(GameState* gameState);

int getNumRowsInBlock_M(GameState* gameState);

int getBlockSize_MN(GameState* gameState);

int getPuzzleBoardSize_MN2(GameState* gameState);

bool isIndexInRange(GameState* gameState, int index);

bool isCellValueInRange(GameState* gameState, int value);

char* getCurModeString(State* state);

bool isBoardFilled(GameState* gameState);

bool isBoardErroneous(GameState* gameState);

typedef enum {
	GET_PUZZLE_SOLUTION_SUCCESS,
	GET_PUZZLE_SOLUTION_BOARD_UNSOLVABLE,
	GET_PUZZLE_SOLUTION_COULD_NOT_SOLVE_BOARD,
	GET_PUZZLE_SOLUTION_MEMORY_ALLOCATION_FAILURE
} getPuzzleSolutionErrorCode;

getPuzzleSolutionErrorCode getPuzzleSolution(GameState* gameState, Board* solutionOut);

typedef enum {
	IS_PUZZLE_SOLVABLE_BOARD_SOLVABLE,
	IS_PUZZLE_SOLVABLE_BOARD_UNSOLVABLE,
	IS_PUZZLE_SOLVABLE_FAILED_VALIDATING,
	IS_PUZZLE_SOLVABLE_MEMORY_ALLOCATION_FAILURE
} isPuzzleSolvableErrorCode;

isPuzzleSolvableErrorCode isPuzzleSolvable(GameState* gameState);

bool isCellEmpty(GameState* gameState, int row, int col);

bool isCellFixed(GameState* gameState, int row, int col);

bool isCellErroneous(GameState* gameState, int row, int col);

int getCellValue(GameState* gameState, int row, int col);

bool shouldMarkErrors(State* state);

void setMarkErrors(State* state, bool shouldMarkErrors);

bool isThereMoveToUndo(GameState* gameState);

bool isThereMoveToRedo(GameState* gameState);

GameState* createGameState(Board* board, GameMode mode);

void cleanupGameState(GameState* state);

bool exportBoard(GameState* gameState, Board* boardInOut);

void cleanupCellLegalValuesStruct(CellLegalValues* cellLegalValuesInOut);

bool isValueLegalForCell(GameState* gameState, int row, int col, int value);

bool fillCellLegalValuesStruct(GameState* gameState, int row, int col, CellLegalValues* cellLegalValuesInOut);

int setPuzzleCell(GameState* gameState, int row, int indexInRow, int value);

bool makeMove(GameState* gameState, Move* move);

bool makeCellChangeMove(GameState* gameState, int value, int row, int col);

bool makeMultiCellMove(GameState* gameState, Board* newBoard);

bool autofill(GameState* gameState, Move** outMove);

/* Note: returned Move should not be freed or changed! */
const Move* undoMove(GameState* gameState);

/* Note: returned Move should not be freed or changed! */
const Move* redoMove(GameState* gameState);

bool resetMoves(GameState* gameState);

bool isSolutionSuccessful(GameState* gameState);

bool isSolutionFailing(GameState* gameState);

char* getPuzzleAsString(State* state);

#endif /* GAME_H_ */
