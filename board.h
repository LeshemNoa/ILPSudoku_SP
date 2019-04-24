#ifndef BOARD_H_
#define BOARD_H_

#include <stdbool.h>

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

typedef struct {
	int numLegalValues;
    bool* legalValues;} CellLegalValues;

/**
  * Board struct represents a sudoku board.
  */
typedef struct {
   	int numRowsInBlock_M;
   	int numColumnsInBlock_N;
   	Cell** cells;} Board;


typedef void (*getCategory1BasedIDByCategory2BasedIDFunc)(Board* board, int category1No, int indexInCategory1, int* category2No, int* indexInCateogry2);

typedef Cell* (*getCellsByCategoryFunc)(Board* board, int categoryNo, int indexInCategory);

int getNumColumnsInBoardBlock_N(Board* board);

int getNumRowsInBoardBlock_M(Board* board);

int getBoardBlockSize_MN(Board* board);

int getBoardSize_MN2(Board* board);

int getNumFilledBoardCells(Board* board);

Cell* getBoardCellByRow(Board* board, int row, int index);

Cell* getBoardCellByColumn(Board* board, int column, int index);

Cell* getBoardCellByBlock(Board* board, int block, int index);

int whichBlock(Board* board, int row, int col);

int getBlockNumberByCell(Board* board, int row, int col);

bool getNextEmptyBoardCell(Board* board, int row, int col, int* outRow, int* outCol);

void getRowBasedIDGivenRowBasedID(Board* board, int rowIn, int indexInRowIn, int* row, int* indexInRow);

void getRowBasedIDGivenBlockBasedID(Board* board, int block, int indexInBlock, int* row, int* indexInRow);

void getRowBasedIDGivenColumnBasedID(Board* board, int column, int indexInColumn, int* row, int* indexInRow);

void getColumnBasedIDGivenRowBasedID(Board* board, int row, int indexInRow, int* column, int* indexInColumn);

void getBlockBasedIDGivenRowBasedID(Board* board, int row, int indexInRow, int* block, int* indexInBlock);

bool isBoardCellFixed(Cell* cell);

bool isBoardCellErroneous(Cell* cell);

int getBoardCellValue(Cell* cell);

bool isBoardCellEmpty(Cell* cell);

void emptyBoardCell(Cell* cell);

void setBoardCellFixedness(Cell* cell, bool isFixed);

void setBoardCellErroneousness(Cell* cell, bool isErroneous);

void cleanupBoard(Board* boardInOut);

bool createEmptyBoard(Board* boardInOut);

int countNumEmptyCells(Board* board);

void freeSpecificCellsValuesCounters(int** cellValuesCounters, Board* board);

int** createCellsValuesCountersByCategory(Board* board, getCellsByCategoryFunc getCellFunc);

bool checkErroneousCellsInCategory(Board* board, int categoryNo, getCellsByCategoryFunc getCellFunc, bool* outErroneous);

bool checkErroneousCells(Board* board, bool* outErroneous);

bool findErroneousCells(Board* board);

bool copyBoard(Board* boardIn, Board* boardOut);

bool calculateNumSolutions(Board* board, int* numSolutions);

void setBoardCellValue(Board* board, int row, int col, int value);

void cleanupBoardCellLegalValuesStruct(CellLegalValues* cellLegalValues);

bool isValueLegalForBoardCell(Board* boardIn, int row, int col, int value);

bool fillBoardCellLegalValuesStruct(Board* boardIn, int row, int col, CellLegalValues* cellLegalValuesInOut);

void freeCellsLegalValuesForAllBoardCells(Board* boardIn, CellLegalValues** cellsLegalValuesOut);

bool getSuperficiallyLegalValuesForAllBoardCells(Board* boardIn, CellLegalValues*** cellsLegalValuesOut);

typedef enum {
	LOAD_BOARD_FROM_FILE_FILE_COULD_NOT_BE_OPENED = 1,
	LOAD_BOARD_FROM_FILE_COULD_NOT_PARSE_BOARD_DIMENSIONS,
	LOAD_BOARD_FROM_FILE_DIMENSION_ARE_NOT_POSITIVE,
	LOAD_BOARD_FROM_FILE_BAD_FORMAT_FAILED_TO_READ_A_CELL,
	LOAD_BOARD_FROM_FILE_BAD_FORMAT_FILE_CONTAINS_TOO_MUCH_CONTENT,
	LOAD_BOARD_FROM_FILE_CELL_VALUE_NOT_IN_RANGE,
	LOAD_BOARD_FROM_FILE_MEMORY_ALLOCATION_FAILURE
} LoadBoardFromFileErrorCode;
LoadBoardFromFileErrorCode loadBoardFromFile(char* filePath, Board* boardInOut);

typedef enum {
	SAVE_BOARD_TO_FILE_FILE_COULD_NOT_BE_OPENED = 1,
	SAVE_BOARD_TO_FILE_DIMENSIONS_COULD_NOT_BE_WRITTEN,
	SAVE_BOARD_TO_FILE_FAILED_TO_WRITE_A_CELL
} SaveBoardToFileErrorCode;
SaveBoardToFileErrorCode saveBoardToFile(char* filePath, Board* board);

void markAllCellsAsNotFixed(Board* board);

void markFilledCellsAsFixed(Board* board);

typedef enum {
	GET_BOARD_SOLUTION_SUCCESS,
	GET_BOARD_SOLUTION_BOARD_UNSOLVABLE,
	GET_BOARD_SOLUTION_COULD_NOT_SOLVE_BOARD,
	GET_BOARD_SOLUTION_MEMORY_ALLOCATION_FAILURE
} getBoardSolutionErrorCode;

getBoardSolutionErrorCode getBoardSolution(Board* board, Board* solutionOut);

typedef enum {
	IS_BOARD_SOLVABLE_BOARD_SOLVABLE,
	IS_BOARD_SOLVABLE_BOARD_UNSOLVABLE,
	IS_BOARD_SOLVABLE_COULD_NOT_SOLVE_BOARD,
	IS_BOARD_SOLVABLE_MEMORY_ALLOCATION_FAILURE
} isBoardSolvableErrorCode;

isBoardSolvableErrorCode isBoardSolvable(Board* board);

typedef enum {
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_MEMORY_ALLOCATION_FAILURE = 1,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_SOLVED,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_NOT_SOLVABLE,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_COULD_NOT_SOLVE_BOARD
} GuessValuesForAllPuzzleCellsErrorCode;

GuessValuesForAllPuzzleCellsErrorCode guessValuesForAllPuzzleCells(Board* board, double**** valuesScoreOut);

char* getBoardAsString(Board* board, bool shouldMarkErrors);

#endif /* BOARD_H_ */
