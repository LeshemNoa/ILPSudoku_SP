#ifndef BOARD_H_
#define BOARD_H_

#include <stdbool.h>
/**
 * The board module is designed to take care of everything related to the suduko board
 * data structure itself. 
 */


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
 * CellLegalValues struct holds information about optional values that may be legally
 * placed in a particular cell. It contains the number of legal values for this cell, 
 * and a boolean array: if index i is true, then i is a legal value for the corresonding
 * cell.
 * 
 */
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

/**
 * Functions who share a signature, used to transform cell indices from one format to another.
 * 
 */
typedef void (*getCategory1BasedIDByCategory2BasedIDFunc)(const Board* board, int category1No, int indexInCategory1, int* category2No, int* indexInCateogry2);
typedef const Cell* (*viewCellsByCategoryFunc)(const Board* board, int categoryNo, int indexInCategory);

/**
 * Functions who share a signature, used to get a cell with provided indices, row, column, or
 * block based. 
 */
typedef Cell* (*getCellsByCategoryFunc)(Board* board, int categoryNo, int indexInCategory);

/**
 * Get the number of columns in a board block, N.
 * 
 * @param board			[in] The board whose parameter is requested
 * @return int 			[out] The number of columns in a board block
 */
int getNumColumnsInBoardBlock_N(const Board* board);

/**
 * Get the number of rows in a board block, N.
 * 
 * @param board			[in] The board whose parameter is requested
 * @return int 			[out] The number of rows in a board block
 */
int getNumRowsInBoardBlock_M(const Board* board);

/**
 * Get the number of cells in a board block, M*N.
 * 
 * @param board			[in] The board whose parameter is requested
 * @return int 			[out] The number of cells in a board block
 */
int getBoardBlockSize_MN(const Board* board);

/**
 * Get the total number of cells in a board.
 * 
 * @param board			[in] The board whose parameter is requested
 * @return int 			[out] The number of cells in the board 
 */
int getBoardSize_MN2(const Board* board);

/**
 * View the Cell struct of a particular board by its row number.
 * 
 * @param board 				[in] The board whose cell is requested
 * @param row 					[in] The number of row in which this cell is located
 * @param index 				[in] The index of the cell within that row
 * @return const Cell* 	[out] A read-only pointer to the const cell struct 
 * 														with those indices 
 */
const Cell* viewBoardCellByRow(const Board* board, int row, int index);

/**
 * View the Cell struct of a particular board by its column number.
 * 
 * @param board 				[in] The board whose cell is requested
 * @param column				[in] The number of column in which this cell is located
 * @param index 				[in] The index of the cell within that column
 * @return const Cell* 	A read-only pointer to the const cell struct 
 * 											with those indices 
 */
const Cell* viewBoardCellByColumn(const Board* board, int column, int index);

/**
 * View the Cell struct of a particular board by its block number. The block
 * numbers run from 0 to MN-1, left to right and top to botton. The indices of
 * cells within a block also run from 0 to MN-1 in the same manner.
 * 
 * @param board 				[in] The board whose cell is requested
 * @param block 				[in] The number of block in which this cell is located
 * @param index 				[in] The index of the cell within that block
 * @return const Cell* 	A read-only pointer to the const cell struct 
 * 											with those indices 
 */
const Cell* viewBoardCellByBlock(const Board* board, int block, int index);

/**
 * Get the Cell struct of a particular board by its row number.
 * 
 * @param board 				[in] The board whose cell is requested
 * @param row 					[in] The number of row in which this cell is located
 * @param index 				[in] The index of the cell within that row
 * @return Cell* 				A pointer to the cell struct with those indices 
 */
Cell* getBoardCellByRow(Board* board, int row, int index);

/**
 * Get the Cell struct of a particular board by its column number.
 * 
 * @param board 				[in] The board whose cell is requested
 * @param row 					[in] The number of column in which this cell is located
 * @param index 				[in] The index of the cell within that column
 * @return Cell* 				A pointer to the cell struct with those indices 
 */
Cell* getBoardCellByColumn(Board* board, int column, int index);

/**
 * Get the Cell struct of a particular board by its block number. The block
 * numbers run from 0 to MN-1, left to right and top to botton. The indices of
 * cells within a block also run from 0 to MN-1 in the same manner.
 * 
 * @param board 				[in] The board whose cell is requested
 * @param row 					[in] The number of block in which this cell is located
 * @param index 				[in] The index of the cell within that block
 * @return Cell* 				A pointer to the cell struct with those indices 
 */
Cell* getBoardCellByBlock(Board* board, int block, int index);

/**
 * whichBlock uses a linear mapping to find which block in the board a cell with given
 * indices belongs to.
 * 
 * @param row 					[in] The number of row in which this cell is located
 * @param row 					[in] The number of column in which this cell is located
 * @return int					the number of block the cell is in 
 */
int whichBlock(const Board* board, int row, int col);

/**
 * Fills provided integer pointers with the indices of the next empty cell in the
 * provided board, relative to the current cell. The board is travered by rows, 
 * so the next empty cell will be searched for among the cells whose row index
 * is higher than or equal to the current row index.
 * 
 * @param board 			[in] The board to be examined
 * @param row 				[in] The row number of the current cell
 * @param col 				[in] The column number of the current cell
 * @param outRow 			[in, out] A pointer to an integer to be filled with the row index
 * 										of the next empty cell found
 * @param outCol 			[in, out] A pointer to an integer to be filled with the column index
 * 										of the next empty cell found
 * @return true 			iff the next empty cell was found 
 * @return false 			iff there exists no empty cell in the 'next' region
 */
bool getNextEmptyBoardCell(const Board* board, int row, int col, int* outRow, int* outCol);

/**
 * Fills provided integer pointers with a cell's row-based indices, given its row-based
 * indices.
 * 
 * @param board						[in] The board to be examined
 * @param rowIn 					[in] The number of row in which this cell is located
 * @param indexInRowIn 		[in] The index of the cell within that row
 * @param row 						[in, out] A pointer to an integer to be filled with the cell's row number
 * @param indexInRow 			[in, out] A pointer to an integer to be filled with the cell's column number
 */
void getRowBasedIDGivenRowBasedID(const Board* board, int rowIn, int indexInRowIn, int* row, int* indexInRow);

/**
 * Fills provided integer pointers with a cell's row-based indices, given its block-based
 * indices.
 * 
 * @param board						[in] The board to be examined
 * @param block 					[in] The number of block in which this cell is located
 * @param indexInBlock 		[in] The index of the cell within that block
 * @param row 						[in, out] A pointer to an integer to be filled with the cell's row number
 * @param indexInRow 			[in, out] A pointer to an integer to be filled with the cell's column number
 */
void getRowBasedIDGivenBlockBasedID(const Board* board, int block, int indexInBlock, int* row, int* indexInRow);

/**
 * Fills provided integer pointers with a cell's row-based indices, given its column-based
 * indices.
 * 
 * @param board						[in] The board to be examined
 * @param column 					[in] The number of column in which this cell is located
 * @param indexInColumn 	[in] The index of the cell within that column
 * @param row 						[in, out] A pointer to an integer to be filled with the cell's row number
 * @param indexInRow 			[in, out] A pointer to an integer to be filled with the cell's column number
 */
void getRowBasedIDGivenColumnBasedID(const Board* board, int column, int indexInColumn, int* row, int* indexInRow);

/**
 * Fills provided integer pointers with a cell's row-based indices, given its column-based
 * indices.
 * 
 * @param board						[in] The board to be examined
 * @param row 						[in] The number of row in which this cell is located
 * @param indexInRow 			[in] The index of the cell within that row
 * @param column 					[in, out] A pointer to an integer to be filled with the cell's column number
 * @param indexInColumn 	[in, out] A pointer to an integer to be filled with the cell's row number
 */
void getColumnBasedIDGivenRowBasedID(const Board* board, int row, int indexInRow, int* column, int* indexInColumn);

/**
 * Fills provided integer pointers with a cell's block-based indices, given its row-based
 * indices.
 * 
 * @param board						[in] The board to be examined
 * @param row 						[in] The number of row in which this cell is located
 * @param indexInRow 			[in] The index of the cell within that row
 * @param block 					[in, out] A pointer to an integer to be filled with the cell's block number
 * @param indexInBlock    [in, out] A pointer to an integer to be filled with the cell's index
 * 												within its block
 */
void getBlockBasedIDGivenRowBasedID(const Board* board, int row, int indexInRow, int* block, int* indexInBlock);

/**
 * Check if a particular Cell struct is marked as a fixed cell.
 * 
 * @param cell 					[in] Cell whose status is checked
 * @return true 				iff the cell is marked as fixed
 * @return false 				iff the cell is not marked as fixed
 */
bool isBoardCellFixed(const Cell* cell);

/**
 * Check if a particular Cell struct is marked as erroneous.
 * 
 * @param cell 					[in] Cell whose status is checked
 * @return true 				iff the cell is marked as erroneous
 * @return false 				iff the cell is not marked as erroneous
 */
bool isBoardCellErroneous(const Cell* cell);

/**
 * Get the current value of a particular Cell struct.
 * 
 * @param cell 					[in] Cell whose value is requested
 * @return int 					the current value of that cell
 */
int getBoardCellValue(const Cell* cell);

/**
 * Check if a particular Cell struct is empty.
 * 
 * @param cell 					[in] Cell whose value is checked
 * @return true 				iff the cell's value eqauls EMPTY_CELL_VALUE
 * @return false 				otherwise
 */
bool isBoardCellEmpty(const Cell* cell);

/**
 * Sets the value field of a particular Cell struct to be EMPTY_CELL_VALUE.
 * 
 * @param cell 			[in, out] Cell whose value is set to EMPTY_CELL_VALUE
 */
void emptyBoardCell(Cell* cell);

/**
 * Sets the isFixed marker of a particular cell with the provided boolean argument.
 * 
 * @param cell 		 [in, out] Cell whose isFixed marker is set
 * @param isFixed  [in] The boolean value to set the marker to
 */
void setBoardCellFixedness(Cell* cell, bool isFixed);

/**
 * Sets the isErroneous marker of a particular cell with the provided boolean argument.
 * 
 * @param cell 		 			[in, out] Cell whose isErroneous marker is set
 * @param isErroneous 	[in] The boolean value to set the marker to
 */
void setBoardCellErroneousness(Cell* cell, bool isErroneous);

/**
 * Free all memory currently allocated to all the cells in the provided board. 
 * 
 * @param boardInOut		[in, out] The board whose cells' memory is freed. At the end of the process,
 * 											its cells field is NULL. 
 */
void cleanupBoard(Board* boardInOut);

/**
 * Create a new, uninitialized, Board struct. 
 * 
 * @param boardInOut 		[in, out] A pointer to a board struct, filled with the new board created
 * @return true 				iff the board was allocated successfully 
 * @return false 				iff a memory error occurred during this process, and the board could
 * 											not be allocated. In this case, the memory that was allocated is freed
 * 											and boardInOut is set to NULL.
 */
bool createEmptyBoard(Board* boardInOut);

/**
 * Get the current number of empty cells in the prvided Board struct. 
 * 
 * @param board 			[in] The board to be examined
 * @return int 				The number of cells whose value is EMPTY_CELL_VALUE in the board
 */
int countNumEmptyCells(const Board* board);

/**
 * Free all memory currently allocated to a value counters integer matrix. 
 * 
 * @param cellValuesCounters 		[in] A pointer to the int matrix to be freed
 * @param board 								[in] The board to which this counter array corresponds
 */
void freeSpecificCellsValuesCounters(int** cellValuesCounters, const Board* board);

/**
 * Creates a counter matrix for values of cells in a particular category of the provided
 * board, where category is row, column, or block. 
 * The i-th row corresponds to the i-th member in the specified category,
 * and the j-th index in the i-th row equals the number of cells that share the value j in the i-th
 * category member. 
 * 
 * @param board 							[in] The board to be examined
 * @param getCellFunc 				[in] A pointer to one of the viewCellsByCategoryFunc functions, which 
 * 														will be used to construct the counter matrix
 * @return int** 							The filled counter matrix corresponding to the board and the requsted
 * 														category, or NULL if a memory error occurred
 */
int** createCellsValuesCountersByCategory(const Board* board, viewCellsByCategoryFunc getCellFunc);

/**
 * Checks if there exist erroneous cells in a particular row, column, or block, specified by the caterogy
 * type and index within it. The existence of errors is check exhaustively, counting the occurences
 * of values in cells of that category. If there's a value whose count exceeds 1, there exist erroneous
 * cells in that category.
 * 
 * @param board 					[in] The board to be examined
 * @param categoryNo 			[in] The row, column or block number to be scanned for errors
 * @param getCellFunc 		[in] A pointer to one of the viewCellsByCategoryFunc functions which 
 * 												will be used to traverse the cells in the category
 * @param outErroneous 		[in, out] A pointer to be boolean to be set to true iff there exist
 * 												errors in that category
 * @return true 					iff the procedure was successful
 * @return false 					iff a memory error occurred during the check process
 */
bool checkErroneousCellsInCategory(const Board* board, int categoryNo, viewCellsByCategoryFunc getCellFunc, bool* outErroneous);

/**
 * Check if there exist erroneous cells by row, column or block, specified by category type.
 * 
 * @param board 					[in] The board to be examined
 * @param outErroneous 		[in, out] A pointer to be boolean to be set to true iff there exist
 * 												errors in that category  
 * @return true 					iff the procedure was successful
 * @return false 					iff a memory error occurred during the check process
 */
bool checkErroneousCells(const Board* board, bool* outErroneous);

/**
 * Creates a copy of the board struct provided.
 * 
 * @param boardIn 		[in] The board to be copied
 * @param boardOut 		[in, out] A pointer to a Board struct to be assigned with the copy of
 * 										the provided board
 * @return true 			iff the procedure was successful
 * @return false 			iff a memory error occurred during the process
 */
bool copyBoard(const Board* boardIn, Board* boardOut);

/**
 * Sets the value of the cell with provided indices to be the input value.
 * 
 * @param board		[in, out] Board whose cell is set
 * @param row 		[in] The number of row in which this cell is located
 * @param col 		[in] The number of column in which this cell is located
 * @param value 	[in] The value to set this cell to
 */
void setBoardCellValue(Board* board, int row, int col, int value);

/**
 * Free all memory allocated to a CellLegalValues struct.
 * 
 * @param cellLegalValues 	[in] a pointer to the struct to be destroyed
 */
void cleanupBoardCellLegalValuesStruct(CellLegalValues* cellLegalValues);

/**
 * Check if a particular value is a valid selection for a particular cell.
 * 
 * @param boardIn 	[in] The board to be examined
 * @param row 			[in] The row number of the cell being checked
 * @param col 			[in] The column number of the cell being checked
 * @param value 		[in] The value whose validity for the cell is checked	
 * @return true 		iff the value is indeed a valid selection for the cell
 * @return false 		otherwise
 */
bool isValueLegalForBoardCell(const Board* boardIn, int row, int col, int value);

/**
 * Fill the CellLegalValues struct for a particular cell.
 * 
 * @param boardIn 	[in] The board to be examined
 * @param row 			[in] The row number of the cell being checked
 * @param col 			[in] The column number of the cell being checked
 * @param cellLegalValuesInOut [in, out] A pointer to the CellLegalValues struct to be filled
 * 															according to the provided board's setup
 * @return true 			iff the procedure was successful
 * @return false 			iff a memory error occurred during the process
 */
bool fillBoardCellLegalValuesStruct(const Board* boardIn, int row, int col, CellLegalValues* cellLegalValuesInOut);

/**
 *  Free all memory allocated to all the CellLegalValues struct of an entire board.
 * 
 * @param boardIn 							[in] Board struct to which the CellLegalValues matrix is
 * 															associated.
 * @param cellsLegalValuesOut 	[in, out] A matrix of CellLegalValues structs, corresponding to
 * 															each cell	in the board 
 */
void freeCellsLegalValuesForAllBoardCells(const Board* boardIn, CellLegalValues** cellsLegalValuesOut);

/**
 * Constructs a matrix of CellLegalValues structs corresponding to each cell in the board.
 * 
 * @param boardIn 						[in] The board to be examined
 * @param cellsLegalValuesOut [in, out] A pointer to a CellLegalValues matrix to be assigned with
 * 														the result
 * @return true 							iff the procedure was successful
 * @return false 							iff a memory error occurred during the process
 */
bool getSuperficiallyLegalValuesForAllBoardCells(const Board* boardIn, CellLegalValues*** cellsLegalValuesOut);

/**
 * Errors that may occur during loading a board from a file. 
 */
typedef enum {
	LOAD_BOARD_FROM_FILE_FILE_COULD_NOT_BE_OPENED = 1,
	LOAD_BOARD_FROM_FILE_COULD_NOT_PARSE_BOARD_DIMENSIONS,
	LOAD_BOARD_FROM_FILE_DIMENSION_ARE_NOT_POSITIVE,
	LOAD_BOARD_FROM_FILE_BAD_FORMAT_FAILED_TO_READ_A_CELL,
	LOAD_BOARD_FROM_FILE_BAD_FORMAT_FILE_CONTAINS_TOO_MUCH_CONTENT,
	LOAD_BOARD_FROM_FILE_CELL_VALUE_NOT_IN_RANGE,
	LOAD_BOARD_FROM_FILE_MEMORY_ALLOCATION_FAILURE
} LoadBoardFromFileErrorCode;
/**
 * Loads a board from a file and constructs a board struct accordingly.
 * 
 * @param filePath 							[in] Path to the file from which the board will be loaded
 * @param boardInOut 						[in, out] A pointer to a board to which the constructed board will
 * 															be assigned. 
 * @return LoadBoardFromFileErrorCode 
 */
LoadBoardFromFileErrorCode loadBoardFromFile(char* filePath, Board* boardInOut);

/**
 * Errors that may occur during saving a board to a file. 
 * 
 */
typedef enum {
	SAVE_BOARD_TO_FILE_FILE_COULD_NOT_BE_OPENED = 1,
	SAVE_BOARD_TO_FILE_DIMENSIONS_COULD_NOT_BE_WRITTEN,
	SAVE_BOARD_TO_FILE_FAILED_TO_WRITE_A_CELL
} SaveBoardToFileErrorCode;
/**
 * Saves a board to a file in the specified format.
 * 
 * @param filePath 		[in] Path to file where the board will be saved
 * @param board 			[in] The board to save
 * @return SaveBoardToFileErrorCode 
 */
SaveBoardToFileErrorCode saveBoardToFile(char* filePath, const Board* board);

/**
 * Set all the cells' isFixed marker to false.
 * 
 * @param board 	[in, out] Board to which this will be done to.
 */
void markAllCellsAsNotFixed(Board* board);

/**
 * Set all the cells which are non-empty in the provided board to be fixed by setting
 * their isFixed marker to true.
 * 
 * @param board [in, out] Board to which this will be done to.
 */
void markFilledCellsAsFixed(Board* board);

/**
 * Errors that may occur while solving a board with LP. 
 */
typedef enum {
	GET_BOARD_SOLUTION_SUCCESS,
	GET_BOARD_SOLUTION_BOARD_UNSOLVABLE,
	GET_BOARD_SOLUTION_COULD_NOT_SOLVE_BOARD,
	GET_BOARD_SOLUTION_MEMORY_ALLOCATION_FAILURE
} getBoardSolutionErrorCode;
/**
 * Solves a particular board using LP.
 * 
 * @param board 								[in] The board to be solved
 * @param solutionOut 					[in, out] A pointer to a board struct, which will be assigned
 * 															with a solution to the board on success
 * @return getBoardSolutionErrorCode 
 */
getBoardSolutionErrorCode getBoardSolution(const Board* board, Board* solutionOut);

/**
 * Errors that may occur while checking whether a board is solvable.
 */
typedef enum {
	IS_BOARD_SOLVABLE_BOARD_SOLVABLE,
	IS_BOARD_SOLVABLE_BOARD_UNSOLVABLE,
	IS_BOARD_SOLVABLE_COULD_NOT_SOLVE_BOARD,
	IS_BOARD_SOLVABLE_MEMORY_ALLOCATION_FAILURE
} isBoardSolvableErrorCode;
/**
 * Checks if there exists some valid solution to a particular board.
 * 
 * @param board 						[in] Board to be checked
 * @return isBoardSolvableErrorCode 
 */
isBoardSolvableErrorCode isBoardSolvable(const Board* board);

/**
 * Errors that may occur during guessing values for all puzzle cells with LP. 
 */
typedef enum {
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_MEMORY_ALLOCATION_FAILURE = 1,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_SOLVED,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_NOT_SOLVABLE,
	GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_COULD_NOT_SOLVE_BOARD
} GuessValuesForAllPuzzleCellsErrorCode;

GuessValuesForAllPuzzleCellsErrorCode guessValuesForAllPuzzleCells(const Board* board, double**** valuesScoreOut);

char* getBoardAsString(const Board* board, bool shouldMarkErrors);

#endif /* BOARD_H_ */
