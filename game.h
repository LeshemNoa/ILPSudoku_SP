/**
 * GAME Summary:
 *
 * A module designed to allow for a Sudoku game to be prepared and later 
 * run from start to finish; in this we include a whole game (multiple
 * boards), and also one instance (i.e., a single Sudoku board).
 *
 * getPuzzle - returns the puzzle
 * getNumEmptyCells - returns the number of empty cells
 * getBlockSize_MN - returns MN
 * getPuzzleBoardSize_MN2 - returns MN^2
 * isIndexInRange - checks if the cell index is in the right range
 * isCellValueInRange - checks if the cell value is in the right range
 * getCurModeString - returns a string representing the current game mode
 * isBoardFilled - checks if the game board is filled
 * isBoardErroneous - checks if the game board is erroneous
 * getPuzzleSolution - returns a solution to the puzzle
 * isPuzzleSolvable - checks if the puzzle is solvable
 * isCellEmpty - checks if a cell is empty
 * isCellFixed - checks if a cell is fixed
 * isCellErroneous - checks if a cell is erroneous
 * getCellValue - returns the value of a cell
 * shouldMarkErrors - checks if the game should mark errors or not
 * setMarkErrors - sets whether the game should mark errors or not
 * createGameState - creates a new GameState object
 * cleanupGameState - cleanups a GameState object
 * exportBoard - exports the game board
 * cleanupCellLegalValuesStruct - cleanup legal values struct
 * isValueLegalForCell - checks if the value is legal for a cell
 * setPuzzleCell - sets the value of a cell
 * makeMove - makes a move
 * makeCellChangeMove - makes a Cell Change move
 * makeMultiCellMove - makes a Multi Cell move)
 * autofill - performs autofill
 * undoMove - undos a move
 * redoMove - redos a move
 * resetMoves - resets the game board
 * isSolutionSuccessful - checks if the current setting of the game board is right
 * isSolutionFailing - checks if the solution of the current setting of the game board is not right 
 * getPuzzleAsString - returns a string representing the game puzzle
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

/**
 * Creates a new gameState struct containing a copy of the provided board and associated data: 
 * counts empty cells and initializes the undo-redo list. 
 * 
 * @param board 		[in] The board to be copied into the GameState
 * @param mode 			[in] iff mode == GAME_EDIT_MODE, mark all cells as not fixed				
 * @return GameState* 	[out] A pointer to the new GameState
 */
GameState* createGameState(Board* board, GameMode mode);

/**
 * Destroys the GameState struct and frees all memory allocated to it.
 * 
 * @param state 	[in] pointer to the GameState struct to be freed
 */
void cleanupGameState(GameState* state);

/**
 * Produces a copy of the GameState's Board struct and assigns it to the provided pointer.
 * 
 * @param gameState 		[in] The GameState whose Board is exported
 * @param boardInOut 		[in, out] A pointer to be assigned with the board's copy
 * @return true 			iff the procedure was successful
 * @return false 			iff a memory error occurred
 */
bool exportBoard(GameState* gameState, Board* boardInOut);

/**
 * Free all memory allocated to a CellLegalValues struct.
 * 
 * @param cellLegalValuesInOut 
 */
void cleanupCellLegalValuesStruct(CellLegalValues* cellLegalValuesInOut);

/**
 * Checks if a particular value is a valid selection for a particular cell in the 
 * provided GameState's board.
 * 
 * @param gameState 	[in] GameState whose board's cell is examined
 * @param row 			[in] The row number of the cell being checked
 * @param col 			[in] The column number of the cell being checked
 * @param value 		[in] The value whose validity for the cell is checked	
 * @return true 		iff the value is indeed a valid selection for the cell
 * @return false 		otherwise
 */
bool isValueLegalForCell(GameState* gameState, int row, int col, int value);

/**
 * Sets the value of the cell with provided indices, and reflecting the consequences of
 * that across the GameState's Board: updates the value counters, marks new errors that may
 * have appeared, and updated the board's numEmpty field if necessary.
 * 
 * @param gameState 		[in, out] GameState whose board is changed
 * @param row 				[in] The number of row in which this cell is located
 * @param indexInRow 		[in] The number of column in which this cell is located
 * @param value 			[in] value to be set in this cell
 * @return int 				the previous value of that cell, for further processing
 */
int setPuzzleCell(GameState* gameState, int row, int indexInRow, int value);

/**
 * Adds provided Move struct to the GameStates UndoRedoList, and applies the changes
 * the Move consists of to the board.
 * 
 * @param gameState 		[in, out] GameState to which the Move is applied
 * @param move 				[in] Move to apply
 * @return true 			iff the procedure was susccessful
 * @return false 			iff a memory error occurred
 */
bool makeMove(GameState* gameState, Move* move);

/**
 * Makes a move that involves a single cell only, reflecting the change in the board
 * and the undo-redo list.
 * 
 * @param gameState 	[in, out] GameState to which the Move is applied
 * @param value 		[in] The value to which the cell is set
 * @param row 			[in] The number of row in which this cell is located 
 * @param col 			[in] The number of column in which this cell is located
 * @return true 		iff the procedure was successful
 * @return false 		iff a memory error occurred
 */
bool makeCellChangeMove(GameState* gameState, int value, int row, int col);

/**
 * To make a move that involves multiple cells, makeMultiCellMove takes a new Board struct
 * describing the game's setup right after the change. It compares the new board with the old
 * one contained in the GameState struct provided, and documents the differences in the board,
 * cell by cell. Finally it also documents the move in the undo-redo list.
 * 
 * @param gameState 	[in, out] GameState whose Board is updated
 * @param newBoard 		[in] The board describing the game setup after the change took place
 * @return true 		iff the procedure was successful
 * @return false 		iff a memory error occurred
 */
bool makeMultiCellMove(GameState* gameState, Board* newBoard);

/**
 * Autofills the GameState's board according to the instructions, that is: if a cell
 * has only one legal value in the current game's setup, it is set to it, although it 
 * may be erroneous due to prior errors. The changes are reflected in the board and 
 * documented in the Move struct that is provided as output.
 * 
 * @param gameState 	[in, out] GameState to which autofill is applied
 * @param outMove 		[in, out] Pointer to be assigned with a pointed to the Move struct
 * 						produced according to the changes in the board
* @return true 			iff the procedure was successful
 * @return false 		iff a memory error occurred
 */
bool autofill(GameState* gameState, Move** outMove);

/**
 * Applies an Undo command to the provided GameState, based on its undo-redo list.
 * 
 * @param gameState 		[in, out] GameState to which Undo is applied
 * @return const Move* 		A read-only pointer to the Move struct, documenting the move
 * 							that has just been undone
 */
const Move* undoMove(GameState* gameState);

/**
 * Applies a Redo command to the provided GameState, based on its undo-redo list.
 * 
 * @param gameState 		[in, out] GameState to which Undo is applied
 * @return const Move* 		A read-only pointer to the Move struct, documenting the move
 * 							that has just been redone
 */
const Move* redoMove(GameState* gameState);

/**
 * Applies a Reset command to the provided GameState, by undoing all moves in its undo-redo
 * list in order.
 * 
 * @param gameState 	[in, out] GameState to which Reset is applied
 * @return true 		iff the board has changed as a result
 * @return false 		otherwise
 */
bool resetMoves(GameState* gameState);

/**
 * Checks whether the GameState's board is successfully solved.
 * 
 * @param gameState 	[in] The GameState examined
 * @return true 		iff the board is filled completely with no errors
 * @return false 		otherwise
 */
bool isSolutionSuccessful(GameState* gameState);

/**
 * Checks whether the GameState's board is not successfully solved.
 * 
 * @param gameState 	[in] The GameState examined 
 * @return true 		iff the board is not full or it contains errors
 * @return false 		iff the board is successfully solved
 */
bool isSolutionFailing(GameState* gameState);

/**
 * Get the board contained in the provided State struct as a string in the specified format
 * for files.
 * 
 * @param state 		[in] State whose Board member is to be written into a string
 * @return char* 		String description of the board
 */
char* getPuzzleAsString(State* state);

#endif /* GAME_H_ */
