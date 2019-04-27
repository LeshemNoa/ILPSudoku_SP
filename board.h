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


typedef void (*getCategory1BasedIDByCategory2BasedIDFunc)(const Board* board, int category1No, int indexInCategory1, int* category2No, int* indexInCateogry2);

typedef const Cell* (*viewCellsByCategoryFunc)(const Board* board, int categoryNo, int indexInCategory); /* CR: do we need both the view and get functions? if so, perhaps only some of them need be exported? */

typedef Cell* (*getCellsByCategoryFunc)(Board* board, int categoryNo, int indexInCategory);

int getNumColumnsInBoardBlock_N(const Board* board);

int getNumRowsInBoardBlock_M(const Board* board);

int getBoardBlockSize_MN(const Board* board);

int getBoardSize_MN2(const Board* board);

int getNumFilledBoardCells(const Board* board);

const Cell* viewBoardCellByRow(const Board* board, int row, int index);

const Cell* viewBoardCellByColumn(const Board* board, int column, int index);

const Cell* viewBoardCellByBlock(const Board* board, int block, int index);

Cell* getBoardCellByRow(Board* board, int row, int index);

Cell* getBoardCellByColumn(Board* board, int column, int index);

Cell* getBoardCellByBlock(Board* board, int block, int index);

int whichBlock(const Board* board, int row, int col);

int getBlockNumberByCell(const Board* board, int row, int col);

bool getNextEmptyBoardCell(const Board* board, int row, int col, int* outRow, int* outCol);

void getRowBasedIDGivenRowBasedID(const Board* board, int rowIn, int indexInRowIn, int* row, int* indexInRow);

void getRowBasedIDGivenBlockBasedID(const Board* board, int block, int indexInBlock, int* row, int* indexInRow);

void getRowBasedIDGivenColumnBasedID(const Board* board, int column, int indexInColumn, int* row, int* indexInRow);

void getColumnBasedIDGivenRowBasedID(const Board* board, int row, int indexInRow, int* column, int* indexInColumn);

void getBlockBasedIDGivenRowBasedID(const Board* board, int row, int indexInRow, int* block, int* indexInBlock);

bool isBoardCellFixed(const Cell* cell);

bool isBoardCellErroneous(const Cell* cell);

int getBoardCellValue(const Cell* cell);

bool isBoardCellEmpty(const Cell* cell);

void emptyBoardCell(Cell* cell);

void setBoardCellFixedness(Cell* cell, bool isFixed);

void setBoardCellErroneousness(Cell* cell, bool isErroneous);

void cleanupBoard(Board* boardInOut);

bool createEmptyBoard(Board* boardInOut);

int countNumEmptyCells(const Board* board);

void freeSpecificCellsValuesCounters(int** cellValuesCounters, const Board* board);

int** createCellsValuesCountersByCategory(const Board* board, viewCellsByCategoryFunc getCellFunc);

bool checkErroneousCellsInCategory(const Board* board, int categoryNo, viewCellsByCategoryFunc getCellFunc, bool* outErroneous);

bool checkErroneousCells(const Board* board, bool* outErroneous);

bool findErroneousCells(Board* board);

bool copyBoard(const Board* boardIn, Board* boardOut);

void setBoardCellValue(Board* board, int row, int col, int value);

void cleanupBoardCellLegalValuesStruct(CellLegalValues* cellLegalValues);

bool isValueLegalForBoardCell(const Board* boardIn, int row, int col, int value);

bool fillBoardCellLegalValuesStruct(const Board* boardIn, int row, int col, CellLegalValues* cellLegalValuesInOut);

void freeCellsLegalValuesForAllBoardCells(const Board* boardIn, CellLegalValues** cellsLegalValuesOut);

bool getSuperficiallyLegalValuesForAllBoardCells(const Board* boardIn, CellLegalValues*** cellsLegalValuesOut);

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

getBoardSolutionErrorCode getBoardSolution(const Board* board, Board* solutionOut);

typedef enum {
	IS_BOARD_SOLVABLE_BOARD_SOLVABLE,
	IS_BOARD_SOLVABLE_BOARD_UNSOLVABLE,
	IS_BOARD_SOLVABLE_COULD_NOT_SOLVE_BOARD,
	IS_BOARD_SOLVABLE_MEMORY_ALLOCATION_FAILURE
} isBoardSolvableErrorCode;

isBoardSolvableErrorCode isBoardSolvable(const Board* board);

typedef enum {
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_MEMORY_ALLOCATION_FAILURE = 1,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_SOLVED,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_NOT_SOLVABLE,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_COULD_NOT_SOLVE_BOARD
} GuessValuesForAllPuzzleCellsErrorCode;

GuessValuesForAllPuzzleCellsErrorCode guessValuesForAllPuzzleCells(const Board* board, double**** valuesScoreOut);

char* getBoardAsString(const Board* board, bool shouldMarkErrors);

#endif /* BOARD_H_ */
