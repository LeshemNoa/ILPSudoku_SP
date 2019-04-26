#include <stdlib.h>
#include <stdio.h>

#include "board.h"

#include "LP_solver.h"

#define UNUSED(x) (void)(x)

#define ERROR_SUCCESS (0)

#define FIXED_CELL_MARKER_IN_FILE ('.')
#define EMPTY_CELL_VALUE_IN_FILE (0)

#define BLOCK_SEPARATOR ('|')
#define SPACE_CHARACTER (' ')
#define EMPTY_CELL_STRING ("  ")
#define FIXED_CELL_MARKER ('.')
#define ERROENOUS_CELL_MARKER ('*')
#define DASH_CHARACTER ('-')

#define LENGTH_OF_STRING_REPRESENTING_CELL (4)

int getNumColumnsInBoardBlock_N(const Board* board) {
	return board->numColumnsInBlock_N;
}

int getNumRowsInBoardBlock_M(const Board* board) {
	return board->numRowsInBlock_M;
}

int getBoardBlockSize_MN(const Board* board) {
	return getNumRowsInBoardBlock_M(board) * getNumColumnsInBoardBlock_N(board);
}

int getBoardSize_MN2(const Board* board) {
	int blockSize = getBoardBlockSize_MN(board);
	return blockSize * blockSize;
}

void getRowBasedIDGivenRowBasedID(const Board* board, int rowIn, int indexInRowIn, int* row, int* indexInRow) {
	UNUSED(board);

	*row = rowIn;
	*indexInRow = indexInRowIn;
}

const Cell* viewBoardCellByRow(const Board* board, int row, int index) {
	return &(board->cells[row][index]);
}

Cell* getBoardCellByRow(Board* board, int row, int index) {
	return &(board->cells[row][index]);
}

void getRowBasedIDGivenColumnBasedID(const Board* board, int column, int indexInColumn, int* row, int* indexInRow) {
	UNUSED(board);

	*row = indexInColumn;
	*indexInRow = column;
}

const Cell* viewBoardCellByColumn(const Board* board, int column, int index) {
	int row = 0, indexInRow = 0;
	getRowBasedIDGivenColumnBasedID(board, column, index, &row, &indexInRow);
	return viewBoardCellByRow(board, row, indexInRow);
}

Cell* getBoardCellByColumn(Board* board, int column, int index) {
	int row = 0, indexInRow = 0;
	getRowBasedIDGivenColumnBasedID(board, column, index, &row, &indexInRow);
	return getBoardCellByRow(board, row, indexInRow);
}

void getRowBasedIDGivenBlockBasedID(const Board* board, int block, int indexInBlock, int* row, int* indexInRow) {
	int colInBlocksMatrix = block % board->numRowsInBlock_M;
	int rowInBlocksMatrix = block / board->numRowsInBlock_M;
	int colInBlock = indexInBlock % board->numColumnsInBlock_N;
	int rowInBlock = indexInBlock / board->numColumnsInBlock_N;

	*row = rowInBlocksMatrix * board->numRowsInBlock_M + rowInBlock;
	*indexInRow = colInBlocksMatrix * board->numColumnsInBlock_N + colInBlock;
}

void getColumnBasedIDGivenRowBasedID(const Board* board, int row, int indexInRow, int* column, int* indexInColumn) {
	UNUSED(board);

	*column = indexInRow;
	*indexInColumn = row;
}

void getBlockBasedIDGivenRowBasedID(const Board* board, int row, int indexInRow, int* block, int* indexInBlock) {
	int rowInBlock = row % board->numRowsInBlock_M;
	int columnInBlock = indexInRow % board->numColumnsInBlock_N;
	*indexInBlock = (rowInBlock * board->numColumnsInBlock_N) + columnInBlock;

	*block = whichBlock(board, row, indexInRow);
}

const Cell* viewBoardCellByBlock(const Board* board, int block, int index) {
	int row = 0, indexInRow = 0;
	getRowBasedIDGivenBlockBasedID(board, block, index, &row, &indexInRow);
	return viewBoardCellByRow(board, row, indexInRow);
}

Cell* getBoardCellByBlock(Board* board, int block, int index) {
	int row = 0, indexInRow = 0;
	getRowBasedIDGivenBlockBasedID(board, block, index, &row, &indexInRow);
	return getBoardCellByRow(board, row, indexInRow);
}

int whichBlock(const Board* board, int row, int col) {
	int colInBlocksMatrix = col / board->numColumnsInBlock_N;
	int rowInBlocksMatrix = row / board->numRowsInBlock_M;

	return rowInBlocksMatrix * board->numRowsInBlock_M + colInBlocksMatrix;
}

int getBlockNumberByCell(const Board* board, int row, int col) {
	return ((row / board->numRowsInBlock_M) * board->numRowsInBlock_M) +
			(col / board->numColumnsInBlock_N);
}

bool getNextEmptyBoardCell(const Board* board, int row, int col, int* outRow, int* outCol) {
	const Cell* curr;
	int r, c, MN = getBoardBlockSize_MN(board);

	for (r = row; r < MN; r++) {
		for (c = (r == row) ? col : 0; c < MN; c++) {
			curr = viewBoardCellByRow(board, r, c);
			if (curr->value == EMPTY_CELL_VALUE) {
				*outRow = r;
				*outCol = c;
				return true;
			}
		}
	}

	return false;
}

int getNumEmptyBoardCells(const Board* board) {
	int i, j;
	int numEmpty = 0;
	const Cell* curr;
	int MN = getBoardBlockSize_MN(board);
	for (i = 0; i < MN; i++) {
		for (j = 0; j < MN; j++) {
			 curr = viewBoardCellByRow(board, i, j);
			if (curr->value == EMPTY_CELL_VALUE) {
				numEmpty++;
			}
		}
	}
	return numEmpty;
}

int getNumFilledBoardCells(const Board* board) {
	return getBoardSize_MN2(board) - getNumEmptyBoardCells(board);
}

bool isBoardCellFixed(const Cell* cell) {
	return cell->isFixed;
}

bool isBoardCellErroneous(const Cell* cell) {
	return cell->isErroneous;
}

int getBoardCellValue(const Cell* cell) {
	return cell->value;
}

bool isBoardCellEmpty(const Cell* cell) {
	return getBoardCellValue(cell) == EMPTY_CELL_VALUE;
}

void emptyBoardCell(Cell* cell) {
	cell->value = EMPTY_CELL_VALUE;
}

void setBoardCellFixedness(Cell* cell, bool isFixed) {
	cell->isFixed = isFixed;
}

void setBoardCellErroneousness(Cell* cell, bool isErroneous) {
	cell->isErroneous = isErroneous;
}

void cleanupBoard(Board* boardInOut) {
	int MN = getBoardBlockSize_MN(boardInOut);

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

bool createEmptyBoard(Board* boardInOut) {
	int MN = getBoardBlockSize_MN(boardInOut);
	boardInOut->cells = calloc(MN, sizeof(Cell*));
	if (boardInOut->cells != NULL) {
		int row = 0;
		for (row = 0; row < MN; row++) {
			boardInOut->cells[row] = calloc(MN, sizeof(Cell));
			if (boardInOut->cells[row] == NULL)
				break;
		}
		if (row == MN)
			return true;
	}

	cleanupBoard(boardInOut);
	return false;
}

int countNumEmptyCells(const Board* board) {
	int numEmptyCells = 0;
	int MN = getBoardBlockSize_MN(board);

	int row = 0;
	int col = 0;
	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++)
			if (isBoardCellEmpty(viewBoardCellByRow(board, row, col)))
				numEmptyCells++;

	return numEmptyCells;
}

void freeSpecificCellsValuesCounters(int** cellValuesCounters, const Board* board) {
	int i = 0;
	int MN = getBoardBlockSize_MN(board);

	if (cellValuesCounters == NULL)
		return;

	for (i = 0; i < MN; i++)
		if (cellValuesCounters[i] != NULL) {
			free(cellValuesCounters[i]);
			cellValuesCounters[i] = NULL;
		}

	free(cellValuesCounters);
}

int** allocateNewSpecificCellsValuesCounters(const Board* board) {
	int** cellsValuesCounters = NULL;
	int MN = getBoardBlockSize_MN(board);

	cellsValuesCounters = calloc(MN, sizeof(int*));
	if (cellsValuesCounters != NULL) {
		int success = true;
		int i = 0;
		for (i = 0; i < MN; i++) {
			cellsValuesCounters[i] = calloc(MN + 1, sizeof(int)); /* Note: +1 for convenience, because values are 1-based */
			if (cellsValuesCounters[i] == NULL) {
				success = false;
				break;
			}
		}

		if (!success) {
			freeSpecificCellsValuesCounters(cellsValuesCounters, board);
			cellsValuesCounters = NULL;
		}
	}

	return cellsValuesCounters;
}

void updateCellsValuesCountersInCategory(int* categoryNoCellsValuesCounters, const Board* board, int categoryNo, viewCellsByCategoryFunc getCellFunc) {
	int MN = getBoardBlockSize_MN(board);
	int index = 0;

	for (index = 0; index < MN; index++) {
		const Cell* cell = getCellFunc(board, categoryNo, index);
		if (!isBoardCellEmpty(cell)) {
			int value = getBoardCellValue(cell);
			categoryNoCellsValuesCounters[value]++;
		}
	}
}

void updateCellsValuesCountersByCategory(int** categoryCellsValuesCounters, const Board* board, viewCellsByCategoryFunc getCellFunc) {
	int numCategories = getBoardBlockSize_MN(board);
	int categoryIndex = 0;

	for (categoryIndex = 0; categoryIndex < numCategories; categoryIndex++)
		updateCellsValuesCountersInCategory(categoryCellsValuesCounters[categoryIndex], board, categoryIndex, getCellFunc);
}

int** createCellsValuesCountersByCategory(const Board* board, viewCellsByCategoryFunc getCellFunc) {
	int** cellsValuesCounters = NULL;

	cellsValuesCounters = allocateNewSpecificCellsValuesCounters(board);
	if (cellsValuesCounters != NULL) {
		updateCellsValuesCountersByCategory(cellsValuesCounters, board, getCellFunc);
	}

	return cellsValuesCounters;
}

bool checkErroneousCellsInCategory(const Board* board, int categoryNo, viewCellsByCategoryFunc getCellFunc, bool* outErroneous) {
	int MN = getBoardBlockSize_MN(board);
	int index = 0;

	int* valuesCounters = calloc(MN + 1, sizeof(int));
	if (valuesCounters == NULL)
		return false;

	*outErroneous = false;
	for (index = 0; index < MN; index++) {
		const Cell* cell = getCellFunc(board, categoryNo, index);
		if (!isBoardCellEmpty(cell)) {
			int value = getBoardCellValue(cell);
			valuesCounters[value]++;
		}
	}

	for (index = 0; index < MN; index++) {
		const Cell* cell = getCellFunc(board, categoryNo, index);
		if (!isBoardCellEmpty(cell)) {
			int value = getBoardCellValue(cell);
			if (valuesCounters[value] > 1) {
				*outErroneous = true;
				break;
			}
		}
	}

	free(valuesCounters);
	return true;
}

bool checkErroneousCellsByCategory(const Board* board, viewCellsByCategoryFunc getCellFunc, bool* outErroneous) {
	int numCategories = getBoardBlockSize_MN(board);
	int index = 0;

	for (index = 0; index < numCategories; index++)
		if (!checkErroneousCellsInCategory(board, index, getCellFunc, outErroneous))
			return false;

	return true;
}

bool checkErroneousCells(const Board* board, bool* outErroneous) {
	if (!checkErroneousCellsByCategory(board, viewBoardCellByRow, outErroneous))
		return false;

	if (outErroneous) {
		return true;
	}

	if (!checkErroneousCellsByCategory(board, viewBoardCellByColumn, outErroneous))
			return false;

	if (outErroneous) {
		return true;
	}

	if (!checkErroneousCellsByCategory(board, viewBoardCellByBlock, outErroneous))
			return false;

	return true;
}

bool findErroneousCellsInCategory(Board* board, int categoryNo, getCellsByCategoryFunc getCellFunc) {
	int MN = getBoardBlockSize_MN(board);
	int index = 0;

	int* valuesCounters = calloc(MN + 1, sizeof(int));
	if (valuesCounters == NULL)
		return false;

	for (index = 0; index < MN; index++) {
		Cell* cell = getCellFunc(board, categoryNo, index);
		if (!isBoardCellEmpty(cell)) {
			int value = getBoardCellValue(cell);
			valuesCounters[value]++;
		}
	}

	for (index = 0; index < MN; index++) {
		Cell* cell = getCellFunc(board, categoryNo, index);
		if (!isBoardCellEmpty(cell)) {
			int value = getBoardCellValue(cell);
			if (valuesCounters[value] > 1)
				setBoardCellErroneousness(cell, true);
		}
	}

	free(valuesCounters);
	return true;
}

bool findErroneousCellsByCategory(Board* board, getCellsByCategoryFunc getCellFunc) {
	int numCategories = getBoardBlockSize_MN(board);
	int index = 0;

	for (index = 0; index < numCategories; index++)
		if (!findErroneousCellsInCategory(board, index, getCellFunc))
			return false;

	return true;
}

bool findErroneousCells(Board* board) {
	if (!findErroneousCellsByCategory(board, getBoardCellByRow))
		return false;

	if (!findErroneousCellsByCategory(board, getBoardCellByColumn))
			return false;

	if (!findErroneousCellsByCategory(board, getBoardCellByBlock))
			return false;

	return true;
}

bool copyBoard(const Board* boardIn, Board* boardOut) {
	int row = 0;
	int col = 0;
	int MN = 0;

	boardOut->numRowsInBlock_M = boardIn->numRowsInBlock_M;
	boardOut->numColumnsInBlock_N = boardIn->numColumnsInBlock_N;
	if (!createEmptyBoard(boardOut)) {
		return false;
	}

	MN = getBoardBlockSize_MN(boardOut);

	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++) {
			Cell* cellOut = getBoardCellByRow(boardOut, row, col);
			const Cell* cellIn = viewBoardCellByRow(boardIn, row, col);
			*cellOut = *cellIn;
		}

	return true;
}

void setBoardCellValue(Board* board, int row, int col, int value) {
	board->cells[row][col].value = value;
}

void cleanupBoardCellLegalValuesStruct(CellLegalValues* cellLegalValues) {
	if (cellLegalValues == NULL)
		return;

	if (cellLegalValues->legalValues != NULL) {
		free(cellLegalValues->legalValues);
		cellLegalValues->legalValues = NULL;
	}

	cellLegalValues->numLegalValues = 0;
}

bool isValueLegalForBoardCellInCategory(const Board* boardIn, int row, int col, int value, viewCellsByCategoryFunc getCellFunc, getCategory1BasedIDByCategory2BasedIDFunc switchIDFunc) {
	int MN = getBoardBlockSize_MN(boardIn);
	int cellCategoryNo = 0;
	int cellIndexInCategory = 0;
	int index = 0;

	switchIDFunc(boardIn, row, col, &cellCategoryNo, &cellIndexInCategory);

	index = 0;
	for (index = 0; index < MN; index++) {
		const Cell* curCell = NULL;
		if (index == cellIndexInCategory)
			continue;
		curCell = getCellFunc(boardIn, cellCategoryNo, index);
		if (!isBoardCellEmpty(curCell)) {
			if (getBoardCellValue(curCell) == value)
				return false;
		}
	}

	return true;
}

bool isValueLegalForBoardCell(const Board* boardIn, int row, int col, int value) {
	return isValueLegalForBoardCellInCategory(boardIn, row, col, value, viewBoardCellByRow, getRowBasedIDGivenRowBasedID) &&
		   isValueLegalForBoardCellInCategory(boardIn, row, col, value, viewBoardCellByColumn, getColumnBasedIDGivenRowBasedID) &&
		   isValueLegalForBoardCellInCategory(boardIn, row, col, value, viewBoardCellByBlock, getBlockBasedIDGivenRowBasedID);
}

bool fillBoardCellLegalValuesStruct(const Board* boardIn, int row, int col, CellLegalValues* cellLegalValuesInOut) {
	int MN = getBoardBlockSize_MN(boardIn);
	int value = 0;

	cellLegalValuesInOut->numLegalValues = 0;
	cellLegalValuesInOut->legalValues = calloc(MN + 1, sizeof(bool));
	if (cellLegalValuesInOut->legalValues == NULL)
		return false;

	for (value = 1; value <= MN; value++)
		if (isValueLegalForBoardCell(boardIn, row, col, value)) {
			cellLegalValuesInOut->legalValues[value] = true;
			cellLegalValuesInOut->numLegalValues++;
		}

	return true;
}

bool getSuperficiallyLegalValuesForBoardCell(const Board* boardIn, int row, int col, CellLegalValues* cellLegalValuesInOut) {
	bool retValue = true;

	if (!fillBoardCellLegalValuesStruct(boardIn, row, col, cellLegalValuesInOut)) {
		cleanupBoardCellLegalValuesStruct(cellLegalValuesInOut);
		retValue = false;
	}

	return retValue;
}

void freeCellsLegalValuesForAllBoardCells(const Board* boardIn, CellLegalValues** cellsLegalValuesOut) {
	int MN = 0;
	int row = 0, col = 0;

	MN = getBoardBlockSize_MN(boardIn);

	if (cellsLegalValuesOut == NULL)
		return;

	for (row = 0; row < MN; row++)
		if (cellsLegalValuesOut[row] != NULL) {
			for (col = 0; col < MN; col++)
				cleanupBoardCellLegalValuesStruct(&(cellsLegalValuesOut[row][col]));
			free(cellsLegalValuesOut[row]);
			cellsLegalValuesOut[row] = NULL;
		}

	free(cellsLegalValuesOut);
}

bool getSuperficiallyLegalValuesForAllBoardCells(const Board* boardIn, CellLegalValues*** cellsLegalValuesOut) {
	bool retValue = true;
	CellLegalValues** cellsLegalValues = NULL;

	int MN = 0;

	MN = getBoardBlockSize_MN(boardIn);

	cellsLegalValues = calloc(MN, sizeof(CellLegalValues*));
	if (cellsLegalValues == NULL)
		retValue = false;
	else {
		int row = 0;
		for (row = 0; row < MN; row++) {
			cellsLegalValues[row] = calloc(MN, sizeof(CellLegalValues));
			if (cellsLegalValues[row] == NULL) {
				retValue = false;
				break;
			} else {
				int col = 0;
				for (col = 0; col < MN; col++)
					if (!getSuperficiallyLegalValuesForBoardCell(boardIn, row, col, &(cellsLegalValues[row][col]))) {
						retValue = false;
						break;
					}
				if (!retValue)
					break;
			}
		}
	}

	if (!retValue)
		freeCellsLegalValuesForAllBoardCells(boardIn, cellsLegalValues);
	else
		*cellsLegalValuesOut = cellsLegalValues;

	return retValue;
}

bool isFileEmpty(FILE* file) {
	char chr = '\0';
	int fscanfRetVal = 0;
	fscanfRetVal = fscanf(file, " %c", &chr);
	return fscanfRetVal == EOF;
}

bool readCellFromFileToBoard(FILE* file, Cell* destination, bool isLastCell) {
	char isFixedChar = '\0';

	int fscanfRetVal = 0;
	fscanfRetVal = fscanf(file, " %d%c ", &(destination->value), &isFixedChar);

	if (fscanfRetVal == 2) {
		destination->isFixed = (isFixedChar == FIXED_CELL_MARKER_IN_FILE);
		return true;
	}

	if ((fscanfRetVal == 1) && isLastCell) {
		destination->isFixed = false;
		return true;
	}

	return false;
}

bool readCellsFromFileToBoard(FILE* file, Board* boardInOut) {
	int MN = getBoardBlockSize_MN(boardInOut);

	int row = 0, col = 0;
	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++)
			if (!readCellFromFileToBoard(file, &(boardInOut->cells[row][col]), (row + 1 == MN) && (col + 1 == MN)))
				return false;

	return true;
}

bool areCellValuesInRange(Board* board) {
	int MN = getBoardBlockSize_MN(board);
	int row = 0;
	int col = 0;
	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++) {
			int value = board->cells[row][col].value;
			if ((value != EMPTY_CELL_VALUE_IN_FILE) && ((value < 1) || (value > MN)))
				return false;
		}

	return true;
}

LoadBoardFromFileErrorCode loadBoardFromFile(char* filePath, Board* boardInOut) {
	LoadBoardFromFileErrorCode retVal = ERROR_SUCCESS;
	FILE* file = NULL;
	int fscanfRetVal = 0;
	int n = 0, m = 0;

	file = fopen(filePath, "r");
	if (file == NULL) {
		retVal = LOAD_BOARD_FROM_FILE_FILE_COULD_NOT_BE_OPENED;
	} else {
		fscanfRetVal = fscanf(file, " %d %d ", &m, &n);
		if (fscanfRetVal != 2) {
			retVal = LOAD_BOARD_FROM_FILE_COULD_NOT_PARSE_BOARD_DIMENSIONS;
		} else {
			if (!((n > 0) && (m > 0))) {
				retVal = LOAD_BOARD_FROM_FILE_DIMENSION_ARE_NOT_POSITIVE;
			} else {
				Board tempBoard = {0};
				tempBoard.numRowsInBlock_M = m; tempBoard.numColumnsInBlock_N = n;
				if (!createEmptyBoard(&tempBoard)) {
					retVal = LOAD_BOARD_FROM_FILE_MEMORY_ALLOCATION_FAILURE;
				} else {
					if (!readCellsFromFileToBoard(file, &tempBoard)) {
						retVal = LOAD_BOARD_FROM_FILE_BAD_FORMAT_FAILED_TO_READ_A_CELL;
					} else {
						if (!isFileEmpty(file)) {
							retVal = LOAD_BOARD_FROM_FILE_BAD_FORMAT_FILE_CONTAINS_TOO_MUCH_CONTENT;
						} else {
							if (!areCellValuesInRange(&tempBoard)) {
								retVal = LOAD_BOARD_FROM_FILE_CELL_VALUE_NOT_IN_RANGE;
							} else {
								*boardInOut = tempBoard;
							}
						}
					}
				}
				if (retVal != ERROR_SUCCESS)
					cleanupBoard(&tempBoard);
			}
		}
		fclose(file);
	}
	return retVal;
}


bool writeCellFromBoardToFile(FILE* file, Cell* cell, bool isLastInRow) {
	int fprintfRetVal = 0;
	fprintfRetVal = fprintf(file, "%d", cell->value);
	if (fprintfRetVal <= 0)
		return false;

	if (isBoardCellFixed(cell)) {
			fprintfRetVal = fprintf(file, "%c", FIXED_CELL_MARKER_IN_FILE);
			if (fprintfRetVal <= 0)
				return false;
	}

	if (!isLastInRow) {
		fprintfRetVal = fprintf(file, " ");
		if (fprintfRetVal <= 0)
			return false;
	}

	return true;
}

bool writeCellsFromBoardToFile(FILE* file, Board* boardInOut) {
	int MN = boardInOut->numColumnsInBlock_N * boardInOut->numRowsInBlock_M;

	int row = 0, col = 0;
	for (row = 0; row < MN; row++) {
		for (col = 0; col < MN; col++)
			if (!writeCellFromBoardToFile(file, &(boardInOut->cells[row][col]), (col + 1 == MN)))
				return false;
		fprintf(file, "\n");
	}

	return true;
}

SaveBoardToFileErrorCode saveBoardToFile(char* filePath, Board* board) {
	SaveBoardToFileErrorCode retVal = ERROR_SUCCESS;
	FILE* file = NULL;
	int fprintfRetVal = 0;

	file = fopen(filePath, "w");
	if (file == NULL) {
		retVal = SAVE_BOARD_TO_FILE_FILE_COULD_NOT_BE_OPENED;
	} else {
		fprintfRetVal = fprintf(file, "%d %d\n", board->numRowsInBlock_M, board->numColumnsInBlock_N);
		if (fprintfRetVal <= 0) {
			retVal = SAVE_BOARD_TO_FILE_DIMENSIONS_COULD_NOT_BE_WRITTEN;
		} else {
			if (!writeCellsFromBoardToFile(file, board)) {
				retVal = SAVE_BOARD_TO_FILE_FAILED_TO_WRITE_A_CELL;
			}
		}
		fclose(file);
	}
	return retVal;
}

void markAllCellsAsNotFixed(Board* board) {
	int MN = getBoardBlockSize_MN(board);

	int row = 0;
	int col = 0;
	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++)
			setBoardCellFixedness(getBoardCellByRow(board, row, col), false);
}

void markFilledCellsAsFixed(Board* board) {
	int MN = getBoardBlockSize_MN(board);

	int row = 0;
	int col = 0;
	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++) {
			Cell* cell = getBoardCellByRow(board, row, col);
			if (!isBoardCellEmpty(cell))
				setBoardCellFixedness(cell, true);
		}

}

getBoardSolutionErrorCode getBoardSolution(const Board* board, Board* solutionOut) {
	getBoardSolutionErrorCode retVal = GET_BOARD_SOLUTION_SUCCESS;

	Board boardCopy = {0};

	if (!copyBoard(board, &boardCopy)) {
		retVal = GET_BOARD_SOLUTION_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	switch (solveBoardUsingLinearProgramming(SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_ILP, &boardCopy, solutionOut, NULL)) {
		case SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SUCCESS:
			retVal = GET_BOARD_SOLUTION_SUCCESS;
			break;
		case SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE:
			retVal =  GET_BOARD_SOLUTION_MEMORY_ALLOCATION_FAILURE;
			break;
		case SOLVE_BOARD_USING_LINEAR_PROGRAMMING_BOARD_ISNT_SOLVABLE:
			retVal =  GET_BOARD_SOLUTION_BOARD_UNSOLVABLE;
			break;
		default:
			retVal = GET_BOARD_SOLUTION_COULD_NOT_SOLVE_BOARD;
			break;
	}

	if (retVal != GET_BOARD_SOLUTION_SUCCESS) {
		cleanupBoard(solutionOut);
	}

	cleanupBoard(&boardCopy);

	return retVal;
}

isBoardSolvableErrorCode isBoardSolvable(const Board* board) {
	isBoardSolvableErrorCode retVal = IS_BOARD_SOLVABLE_BOARD_SOLVABLE;

	Board boardSolution = {0};

	switch (getBoardSolution(board, &boardSolution)) {
	case GET_BOARD_SOLUTION_SUCCESS:
		retVal = IS_BOARD_SOLVABLE_BOARD_SOLVABLE;
		break;
	case GET_BOARD_SOLUTION_MEMORY_ALLOCATION_FAILURE:
		retVal = IS_BOARD_SOLVABLE_MEMORY_ALLOCATION_FAILURE;
		break;
	case GET_BOARD_SOLUTION_BOARD_UNSOLVABLE:
		retVal = IS_BOARD_SOLVABLE_BOARD_UNSOLVABLE;
		break;
	case GET_BOARD_SOLUTION_COULD_NOT_SOLVE_BOARD:
		retVal = IS_BOARD_SOLVABLE_COULD_NOT_SOLVE_BOARD;
		break;
	}

	cleanupBoard(&boardSolution);

	return retVal;
}

GuessValuesForAllPuzzleCellsErrorCode guessValuesForAllPuzzleCells(const Board* board, double**** valuesScoreOut) {
	GuessValuesForAllPuzzleCellsErrorCode retVal = GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_SOLVED;

	double*** valuesScores = NULL;

	if (!allocateValuesScoresArr(&valuesScores, board)) {
		retVal = GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	switch (solveBoardUsingLinearProgramming(SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_LP, board, NULL, valuesScores)) {
		case SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SUCCESS:
			retVal = GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_SOLVED;
			break;
		case SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE:
			retVal = GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_MEMORY_ALLOCATION_FAILURE;
			break;
		case SOLVE_BOARD_USING_LINEAR_PROGRAMMING_BOARD_ISNT_SOLVABLE:
			retVal = GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_NOT_SOLVABLE;
			break;
		default:
			retVal = GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_COULD_NOT_SOLVE_BOARD;
			break;
	}

	if (retVal == GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_SOLVED)
		*valuesScoreOut = valuesScores;
	else
		freeValuesScoresArr(valuesScores, board);

	return retVal;
}


int getLineLength(const Board* board) {
	return LENGTH_OF_STRING_REPRESENTING_CELL * (getBoardBlockSize_MN(board)) + getNumRowsInBoardBlock_M(board) + 1 + 1;
}

char* writeSeparatorLine(char* str, const Board* board) {
	int i = 0;
	for (i = 0; i < getLineLength(board) - 1; i++)
		str += sprintf(str, "%c", DASH_CHARACTER);

	str += sprintf(str, "\n");
	return str;
}

char* writeCell(char* str, const Board* board, int row, int col, bool shouldMarkErrors) {
	str += sprintf(str, "%c", SPACE_CHARACTER);

	if (!isBoardCellEmpty(viewBoardCellByRow(board, row, col)))
		str += sprintf(str, "%2d", getBoardCellValue(viewBoardCellByRow(board, row, col)));
	else
		str += sprintf(str, "%s", EMPTY_CELL_STRING);

	if (isBoardCellFixed(viewBoardCellByRow(board, row, col)))
		str += sprintf(str, "%c", FIXED_CELL_MARKER);
	else if (isBoardCellErroneous(viewBoardCellByRow(board, row, col)) && shouldMarkErrors)
			str += sprintf(str, "%c", ERROENOUS_CELL_MARKER);
	else str += sprintf(str, "%c", SPACE_CHARACTER);

	return str;
}

char* writeRow(char* str, const Board* board, int rowsBlock, int rowInBlock, bool shouldMarkErrors) {
	int M = getNumRowsInBoardBlock_M(board);
	int N = getNumColumnsInBoardBlock_N(board);
	int row = rowsBlock * M + rowInBlock;
	int col = 0;

	for (col = 0; col < N * M; col++) {
		if (col % N == 0)
			str += sprintf(str, "%c", BLOCK_SEPARATOR);
		str = writeCell(str, board, row, col, shouldMarkErrors);
	}
	str += sprintf(str, "%c", BLOCK_SEPARATOR);

	str += sprintf(str, "\n");

	return str;
}

char* writeRowsBlock(char* str, const Board* board, int rowsBlock, bool shouldMarkErrors) {
	int rowInBlock = 0;
	int numRowsInBlock = getNumRowsInBoardBlock_M(board);

	for(rowInBlock = 0; rowInBlock < numRowsInBlock; rowInBlock++) {
		str = writeRow(str, board, rowsBlock, rowInBlock, shouldMarkErrors);
	}

	return str;
}

char* getBoardAsString(const Board* board, bool shouldMarkErrors) {
	char* str = NULL;
	char* strPointer = NULL;
	int rowsBlock = 0;
	int numRowsBlocks = getNumColumnsInBoardBlock_N(board);

	int numCharsRequired = (getLineLength(board) * (getBoardBlockSize_MN(board) + getNumColumnsInBoardBlock_N(board) + 1)) + 1;
	str = calloc(numCharsRequired, sizeof(char));
	if (str != NULL) {

		strPointer = str;
		strPointer = writeSeparatorLine(strPointer, board);
		for (rowsBlock = 0; rowsBlock < numRowsBlocks; rowsBlock++) {
			strPointer = writeRowsBlock(strPointer, board, rowsBlock, shouldMarkErrors);
			strPointer = writeSeparatorLine(strPointer, board);
		}
	}

	return str;
}
