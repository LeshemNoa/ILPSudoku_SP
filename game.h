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
#include "linked_list.h" 

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
    bool isFixed;
	bool isErroneous;} Cell;

/**
 * Board struct represents a sudoku board.
 */
typedef struct {
	int numRowsInBlock_M;
	int numColumnsInBlock_N;
	Cell** cells;} Board;
	

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

typedef struct {
	int numLegalValues;
    bool* legalValues;} CellLegalValues;

typedef void (*getRowBasedIDByCategoryBasedIDFunc)(Board* board, int categoryNo, int indexInCategory, int* row, int* indexInRow);


int getNumEmptyCells(GameState* gameState);

int getNumFilledBoardCells(Board* board);

int getNumColumnsInBlock_N(GameState* gameState);

int getNumRowsInBlock_M(GameState* gameState);

int getBlockSize_MN(GameState* gameState);

int getBoardSize_MN2(GameState* gameState);

bool isIndexInRange(GameState* gameState, int index);

bool isCellValueInRange(GameState* gameState, int value);

char* getCurModeString(State* state);

Cell* getBoardCellByRow(Board* board, int row, int index);

void getRowBasedIDGivenRowBasedID(Board* board, int rowIn, int indexInRowIn, int* row, int* indexInRow);

void getRowBasedIDGivenBlockBasedID(Board* board, int block, int indexInBlock, int* row, int* indexInRow);

void getRowBasedIDGivenColumnBasedID(Board* board, int column, int indexInColumn, int* row, int* indexInRow);

bool isBoardCellFixed(Cell* cell);

int getBoardCellValue(Cell* cell);

bool isBoardCellEmpty(Cell* cell);

void emptyBoardCell(Cell* cell);

bool isBoardErroneous(GameState* gameState);

bool isBoardSolvable(GameState* gameState);

bool isCellEmpty(GameState* gameState, int row, int col);

bool isCellFixed(GameState* gameState, int row, int col);

bool isCellErroneous(GameState* gameState, int row, int col);

int getCellValue(GameState* gameState, int row, int col);

bool shouldMarkErrors(State* state);

void setMarkErrors(State* state, bool shouldMarkErrors);

bool isThereMoveToUndo(GameState* gameState);

bool isThereMoveToRedo(GameState* gameState);

void cleanupBoard(Board* boardInOut);

void cleanupBoard(Board* boardInOut);

bool createEmptyBoard(Board* boardInOut);

GameState* createGameState(Board* board);

void cleanupGameState(GameState* state);

void markAllCellsAsNotFixed(GameState* gameState);

/*bool findErroneousCells(Board* board);*/

bool copyBoard(Board* boardIn, Board* boardOut);

bool exportBoard(GameState* gameState, Board* boardInOut);

void cleanupCellLegalValuesStruct(CellLegalValues* cellLegalValuesInOut);

bool isValueLegalForCell(GameState* gameState, int row, int col, int value);

bool fillCellLegalValuesStruct(GameState* gameState, int row, int col, CellLegalValues* cellLegalValuesInOut);

void freeCellsLegalValuesForAllCells(GameState* gameStateIn, Board* boardIn, CellLegalValues** cellsLegalValuesOut);

bool getSuperficiallyLegalValuesForAllCells(GameState* gameStateIn, Board* boardIn, CellLegalValues*** cellsLegalValuesOut);

int setPuzzleCell(GameState* gameState, int row, int indexInRow, int value);

bool setPuzzleCellMove(State* state, int value, int row, int col);

void undoMove(State* state);

void redoMove(State* state);

bool calculateNumSolutions(GameState* state, int* numSolutions);

#endif /* GAME_H_ */
