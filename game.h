/**
 * GAME Summary:
 *
 * A module designed to allow for a Sudoku game to be prepared and later 
 * run from start to finish; in this we include a whole game (multiple
 * boards), and also one instance (i.e., a single Sudoku board).
 *
 * <LIST_OF_FUNCTIONS_NEEDS_TO_FOLLOW>
 */

#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>

#include "board.h"
#include "move.h"

/**
 * GameState struct represents a single Sudoku game (this meaning that one
 * specific board is "on the table").
 * In order to run one instance of a game of Sudoku, one must acquire (through
 * the createGameState function) such a struct, and then use it to bring about
 * the actual game.
 * Note: The contents of the struct are hidden from the user of this module.
 * 		 One must manipulate the struct received from createGameState through
 *		 functions found in this moudle (it is the only way).
 */
typedef struct GameState GameState;

/**
 * GameMode is an enum that lists all three modes the game itself can run in:
 * - Init, when the game is only started and no board was yet loaded; or,
 *		   alternatively, when a board was solved successfully.
 * - Edit, when a board is loaded for editting it.
 * - Solve, when a board is loaded for solving it.
 */
typedef enum GameMode {
	GAME_MODE_INIT,
	GAME_MODE_EDIT,
	GAME_MODE_SOLVE} GameMode;

/**
 * State struct represents a whole Sudoku game (an instance of this struct is
 * used to run multiple instances of a Sudoku game (i.e., multiple boards), one
 * after the other). This struct then is the superviser, if you would, of the
 * whole game.
 * It contains a GameMode field, to specify the mode the game is currently in;
 * 			   a bool field, to specify whether errors should be hidden from the
 *			   				 user or not;
 *			   and a GameState* field, to hold the current game instance, which_open_mode
 *									   is currently being used by the user.
 */
typedef struct {
	GameMode gameMode;
	bool shouldHideErrors;
	GameState* gameState;} State;

/**
 * getPuzzle returns the board of a given GameState object.
 *
 * @param gameState			[in] a pointer to the GameState object whose board is to be returned.
 *
 * @return const Board*		[out] the board of the given GameState object
 */
const Board* getPuzzle(GameState* gameState);

/**
 * getNumEmptyCells returns the number of empty cells in the board of a given GameState object.
 *
 * @param gameState		[in] a pointer to the GameState object
 *
 * @return int			[out] the number of empty cells in the board of the given GameState object
 */
int getNumEmptyCells(GameState* gameState);

/**
 * getBlockSize_MN returns size of a block in the board of a given GameState object.
 *
 * @param gameState		[in] a pointer to the GameState object
 *
 * @return int			[out] the size of a block in the board of the given GameState object
 */
int getBlockSize_MN(GameState* gameState);

/**
 * getPuzzleBoardSize_MN2 returns size of a block in the board of a given GameState object, squared.
 *
 * @param gameState		[in] a pointer to the GameState object
 *
 * @return int			[out] the size of a block in the board of the given GameState object, squared
 */
int getPuzzleBoardSize_MN2(GameState* gameState);

/**
 * isIndexInRange returns a value indicating a certain index is in the right range to be a zero-based
 * 				  coordinate of a cell of the board which resides in the given GameState object (i.e.,
 *				  if it is between 0 and getBlockSize_MN (excluding the latter)).
 *
 * @param gameState		[in] a pointer to the GameState object
 * @param index			[in] the index to be checked
 *
 * @return bool			[out] true if the index is in the appropriate range, false otherwise
 */
bool isIndexInRange(GameState* gameState, int index);

/**
 * isCellValueInRange returns a value indicating a certain value is in the right range to be put in a
 * 				  	  cell of the board which resides in the given GameState object (i.e., if it is
 *					  between 1 and getBlockSize_MN, or if equals EMPTY_CELL_VALUE, a legal value as well).
 *
 * @param gameState		[in] a pointer to the GameState
 * @param value			[in] the value to be checked
 *
 * @return bool			[out] true if the value is in the appropriate range, false otherwise
 */
bool isCellValueInRange(GameState* gameState, int value);

/**
 * getCurModeString returns a string representing the current mode of the given game whose State object is
					passed to the function.
 *
 * @param state		[in] a pointer to the State object whose game mode is to be translated to a string
 *
 * @return char*	[out] a string representing the mode of the given game
 */
char* getCurModeString(State* state);

/**
 * isBoardFilled returns a value indicating whether the board which resides the given GameState object is
 *				 filled (i.e., if there is no empty cell in it).
 *
 * @param gameState		[in] a pointer to the GameState
 *
 * @return bool			[out] true if the board is filled, false otherwise
 */
bool isBoardFilled(GameState* gameState);

/**
 * isBoardErroneous returns a value indicating whether any of the cells of the board residing in the given
 *					GameState* object hold an erroneous value, rendering the entire board erroneous.
 *
 * @param gameState		[in] a pointer to the GameState
 *
 * @return bool			[out] true if the board is erroneous, false otherwise.
 */
bool isBoardErroneous(GameState* gameState);

/**
 * getPuzzleSolutionErrorCode is an enum that lists all possible return values from the function getPuzzleSolution.
 * The entries within in are named in a self-explanatory manner.
 */
typedef enum {
	GET_PUZZLE_SOLUTION_SUCCESS,
	GET_PUZZLE_SOLUTION_BOARD_UNSOLVABLE,
	GET_PUZZLE_SOLUTION_COULD_NOT_SOLVE_BOARD,
	GET_PUZZLE_SOLUTION_MEMORY_ALLOCATION_FAILURE
} getPuzzleSolutionErrorCode;

/**
 * getPuzzleSolution returns a solution (if one is found) to the board which resides in the given GameState object.
 *
 * @param gameState							[in] a pointer to the GameState
 * @param solutionOut						[in, out] a pointer to a Board struct in which the solution will be saved
 *
 * @return getPuzzleSolutionErrorCode		[out] a value indicating whether the function succeeded, or else if some error has occurred
 */
getPuzzleSolutionErrorCode getPuzzleSolution(GameState* gameState, Board* solutionOut);

/**
 * isPuzzleSolvableErrorCode is an enum that lists all possible return values from the function isPuzzleSolvable.
 * The entries within in are named in a self-explanatory manner.
 */
typedef enum {
	IS_PUZZLE_SOLVABLE_BOARD_SOLVABLE,
	IS_PUZZLE_SOLVABLE_BOARD_UNSOLVABLE,
	IS_PUZZLE_SOLVABLE_FAILED_VALIDATING,
	IS_PUZZLE_SOLVABLE_MEMORY_ALLOCATION_FAILURE
} isPuzzleSolvableErrorCode;

/**
 * isPuzzleSolvable returns a value indicating whether the board which resides within the given GameState object is solvable or not.
 *
 * @param gameState							[in] a pointer to the GameState
 *
 * @return isPuzzleSolvableErrorCode		[out] a value indicating whether the function succeeded and the board is solved, or else if some error has occurred
 */
isPuzzleSolvableErrorCode isPuzzleSolvable(GameState* gameState);

/**
 * isCellEmpty returns a value indicating whether the cell (col, row) in the board which resides in the given GameState object is empty or not.
 *
 * @param gameState		[in] a pointer to the GameState
 * @param row			[in] row index of cell
 * @param col			[in] column index of the cell
 *
 * @return bool			[out] true if said cell is empty, false otherwise
 */
bool isCellEmpty(GameState* gameState, int row, int col);

/**
 * isCellFixed returns a value indicating whether the cell (col, row) in the board which resides in the given GameState object is fixed or not.
 *
 * @param gameState		[in] a pointer to the GameState
 * @param row			[in] row index of cell
 * @param col			[in] column index of the cell
 *
 * @return bool			[out] true if said cell is fixed, false otherwise
 */
bool isCellFixed(GameState* gameState, int row, int col);

/**
 * isCellEmpty returns a value indicating whether the cell (col, row) in the board which resides in the given GameState object is erroneous or not.
 *
 * @param gameState		[in] a pointer to the GameState
 * @param row			[in] row index of cell
 * @param col			[in] column index of the cell
 *
 * @return bool			[out] true if said cell is erroneous, false otherwise
 */
bool isCellErroneous(GameState* gameState, int row, int col);

/**
 * getCellValue returns the value that is in cell (col, row) in the board which resides in the given GameState object.
 *
 * @param gameState		[in] a pointer to the GameState
 * @param row			[in] row index of cell
 * @param col			[in] column index of the cell
 *
 * @return bool			[out] the value in the cell
 */
int getCellValue(GameState* gameState, int row, int col);

/**
 * shouldMarkErrors returns whether currently the game whose State object is given displays error to the user or not.
 *
 * @param state		[in] pointer to the State
 *
 * @return bool		[out] true if errors are being marked, false otherwise
 */
bool shouldMarkErrors(State* state);

/**
 * setMarkErrors sets whether currently the game whose State object is given is to display error to the user or not.
 *
 * @param state				[in] pointer to the State
 * @param shouldMarkErrors 	[in] true if errors should be marked, false otherwise
 *
 * @return void
 */
void setMarkErrors(State* state, bool shouldMarkErrors);

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
