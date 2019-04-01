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
#include <stdlib.h>
#include <stdio.h>

/**
 * The value defined in this constant would mark an empty cell in the sudoku board.
 */
#define EMPTY_CELL_VALUE (0)


/**
 * Cell struct represents one cell of a sudoku puzzle board. It contains the value
 * in the cell, and a marker determining whether the cell is fixed (immutable) or not.
 */
typedef struct {
	int value;
    bool isFixed;} Cell;

/**
 * Board struct represents a sudoku board.
 */
typedef struct {
	Cell** cells;} Board; /* TODO: this will require at least an N and M fields, too */
	

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
	GameState* gameState;} State;


int getNumEmptyCells(GameState* gameState);

int getBlockSize_MN(GameState* gameState);

int getBoardSize_MN2(GameState* gameState);

bool isIndexInRange(GameState* gameState, int index);

bool isCellValueInRange(GameState* gameState, int value);

char* getCurModeString(State* state);

bool isBoardErroneous(GameState* gameState);

bool isBoardSolvable(GameState* gameState);

bool isCellEmpty(GameState* gameState, int row, int col);

bool isCellFixed(GameState* gameState, int row, int col);

bool isThereMoveToUndo(GameState* gameState);

bool isThereMoveToRedo(GameState* gameState);

#endif /* GAME_H_ */
