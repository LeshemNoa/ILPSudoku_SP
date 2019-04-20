#include "ILP_solver.h"

#define UNUSED(x) (void)(x)

/* TODO: get rid of all printfs here (and of stdio.o include in header) */

void freeIntAndIndexBasedLegalValuesForAllCells(Board* board, int*** cellLegalValuesIntBased) {
	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	if (cellLegalValuesIntBased != NULL) {
		int row = 0;
		for (row = 0; row < MN; row++) {
			if (cellLegalValuesIntBased[row] != NULL) {
				int col = 0;
				for (col = 0; col < MN; col++) {
					if (cellLegalValuesIntBased[row][col] != NULL) {
						free(cellLegalValuesIntBased[row][col]);
						cellLegalValuesIntBased[row][col] = NULL;
					}
				}
				free(cellLegalValuesIntBased[row]);
				cellLegalValuesIntBased[row] = NULL;
			}
		}
		free(cellLegalValuesIntBased);
	}
}

bool convertBooleanBasedLegalValuesForAllCellsToIntAndIndexBased(Board* board, CellLegalValues** cellsLegalValues, int**** cellLegalValuesIntBasedOut) {
	bool retValue = true;
	int*** cellLegalValuesIntBased = NULL;

	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;
	cellLegalValuesIntBased = calloc(MN, sizeof(int**));
	if (cellLegalValuesIntBased == NULL)
		retValue = false;
	else {
		int row = 0;
		for (row = 0; row < MN; row++) {
			cellLegalValuesIntBased[row] = calloc(MN, sizeof(int*));
			if (cellLegalValuesIntBased[row] == NULL) {
				retValue = false;
				break;
			} else {
				int col = 0;
				for (col = 0; col < MN; col++) {
					if (!isBoardCellEmpty(getBoardCellByRow(board, row, col)))
						continue;
					cellLegalValuesIntBased[row][col] = calloc(MN + 1, sizeof(int)); /* Note: +1 because values are 1-based (the first item will hold the number of legal values) */
					if (cellLegalValuesIntBased[row][col] == NULL) {
						retValue = false;
						break;
					} else {
						int value = 1;
						int legalValueIndex = 0;

						cellLegalValuesIntBased[row][col][0] = cellsLegalValues[row][col].numLegalValues;
						for (value = 1; value <= MN; value++) {
							if (cellsLegalValues[row][col].legalValues[value]) {
								legalValueIndex++;
								cellLegalValuesIntBased[row][col][value] = legalValueIndex;
							}
						}
					}
				}
				if (!retValue)
					break;
			}
		}
	}

	if (retValue) {
		*cellLegalValuesIntBasedOut = cellLegalValuesIntBased;
	} else {
		freeIntAndIndexBasedLegalValuesForAllCells(board, cellLegalValuesIntBased);
	}

	return retValue;

}

bool getLegalValuesForAllCells(Board* board, int**** cellLegalValuesIntBasedOut) {
	bool retValue = true;
	CellLegalValues** cellsLegalValues = NULL;
	int*** cellLegalValuesIntBased = NULL;

	if (!getSuperficiallyLegalValuesForAllCells(NULL, board, &cellsLegalValues)) {
		return false;
	}

	if (convertBooleanBasedLegalValuesForAllCellsToIntAndIndexBased(board, cellsLegalValues, &cellLegalValuesIntBased)) {
		*cellLegalValuesIntBasedOut = cellLegalValuesIntBased;
	} else
		retValue = false;

	freeCellsLegalValuesForAllCells(NULL, board, cellsLegalValues);

	return retValue;
}

int getTotalNumLegalValuesAndMakeNumsOfLegalValuesIncremental(Board* board, int*** cellLegalValuesIntBased) {
	int totalNumLegalValues = 0;
	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	int row = 0, col = 0;
	for (row = 0; row < MN; row++) {
		for (col = 0; col < MN; col++) {
			if (cellLegalValuesIntBased[row][col] != NULL) {
				int tmpCurNumLegalValues = cellLegalValuesIntBased[row][col][0];
				cellLegalValuesIntBased[row][col][0] = totalNumLegalValues;
				totalNumLegalValues += tmpCurNumLegalValues;
			}
		}
	}

	return totalNumLegalValues;
}

GRBenv* getNewGRBEnvironment() {
	int error = 0;

	GRBenv* env = NULL;

	error = GRBloadenv(&env, "mip1.log"); /* TODO: delete file path when done (it works when NULL!) */
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env)); /* TODO: delete this when done */
		return NULL;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0); /* TODO: probably (check!) delete this when done */
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	}

	return env;
}

void freeGRBEnvironment(GRBenv* env) {
	GRBfreeenv(env);
}

GRBmodel* getNewGRBModel(GRBenv* env) {
	int error = 0;

	GRBmodel* model = NULL;

	error = GRBnewmodel(env, &model, NULL, 0, NULL, NULL, NULL, NULL, NULL); /* Note: Seems like name of model can be NULL (parameter 3) */
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		return NULL;
	}

	return model;
}

void freeGRBModel(GRBmodel* model) {
	GRBfreemodel(model);
}

bool addVariablesAndObjectiveFunctionToModel(GRBenv* env, GRBmodel* model, int numVars) { /* TODO: don't really need env here, i think */
	int retVal = true;

	double* obj = NULL;
	char* vtype = NULL;

	vtype = calloc(numVars, sizeof(char));
	if (vtype == NULL)
		retVal = false;

	if (retVal) {
		int error = 0;

		int i = 0;
		for (i = 0; i < numVars; i++)
			vtype[i] = GRB_BINARY;

		error = GRBaddvars(model, numVars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
		if (!error) {
			error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE); /* TODO: needs to be controlled - whether max or min */
			if (!error) {
				error = GRBupdatemodel(model);
				  if (!error) {
				  } else {
					  retVal = false;
					  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
				  }
			} else {
				retVal = false;
				printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
			}
		} else {
			retVal = false;
			printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		}

		free(vtype);
		vtype = NULL;
	}

	return retVal;
}

int getIndexOfSpecificLegalValueOfCertainCell(int row, int col, int value, int*** cellLegalValuesIntBased) {
	int baseIndex = 0;
	int relativeIndex = 0;

	if (cellLegalValuesIntBased[row][col] == NULL)
		return -1;

	relativeIndex = cellLegalValuesIntBased[row][col][value];
	if (relativeIndex <= 0)
		return -1;

	relativeIndex--; /* Note: this is required in current implementation (because relative indices originally start at 1 in cellLegalValuesIntBased)*/

	baseIndex = cellLegalValuesIntBased[row][col][0];
	return baseIndex + relativeIndex;
}

bool addCellConstraint(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, int row, int col) {
	int retVal = false;

	int* ind = NULL;
	double* val = NULL;

	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	UNUSED(numVars); /* TODO: deal with this */

	ind = calloc(MN, sizeof(int));
	val = calloc(MN, sizeof(double));

	if ((ind != NULL) && (val != NULL)) {
		int numLegalValues = 0;

		int value = 1;
		for (value = 1; value <= MN; value++) {
			int index = getIndexOfSpecificLegalValueOfCertainCell(row, col, value, cellLegalValuesIntBased);
			if (index >= 0) {
				ind[numLegalValues] = index;
				val[numLegalValues] = 1;
				numLegalValues++;
			}
		}

		if (numLegalValues == 0)
			retVal = true;
		else if (numLegalValues > 0) {
			int error1 = 0, error2 = 0;
			error1 = GRBaddconstr(model, numLegalValues, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
			error2 = GRBaddconstr(model, numLegalValues, ind, val, GRB_GREATER_EQUAL, 1.0, NULL);
			if (error1 || error2) {
				  printf("ERROR %d 1st GRBaddconstr(): %s\n", error1, GRBgeterrormsg(env)); /* TODO: only referencing error1 */
			} else {
				retVal = true;
			}
		}

	}

	if (val != NULL) {
		free(val);
		val = NULL;
	}
	if (ind != NULL) {
		free(ind);
		ind = NULL;
	}

	return retVal;
}

bool addCellsConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased) {
	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;
	int row = 0, col = 0;
	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++)
			if (!addCellConstraint(env, model, board, numVars, cellLegalValuesIntBased, row, col))
				return false;
	return true;
}

bool addCategoryInstanceValueConstraint(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, int categoryNo, int value, getRowBasedIDByCategoryBasedIDFunc getRowBasedIDfunc) {
	int retVal = false;

	int* ind = NULL;
	double* val = NULL;

	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	UNUSED(numVars); /* TODO: deal with this */

	ind = calloc(MN, sizeof(int));
	val = calloc(MN, sizeof(double));

	if ((ind != NULL) && (val != NULL)) {
		int numRelevantIndices = 0;

		int indexInCategory = 0;
		for (indexInCategory = 0; indexInCategory < MN; indexInCategory++) {
			int row = 0, col = 0;
			int index = 0;
			getRowBasedIDfunc(board, categoryNo, indexInCategory, &row, &col);
			index = getIndexOfSpecificLegalValueOfCertainCell(row, col, value, cellLegalValuesIntBased);
			if (index >= 0) {
				ind[numRelevantIndices] = index;
				val[numRelevantIndices] = 1;
				numRelevantIndices++;
			}
		}

		if (numRelevantIndices == 0)
			retVal = true;
		else if (numRelevantIndices > 0) {
			int error1 = 0, error2 = 0;
			error1 = GRBaddconstr(model, numRelevantIndices, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
			error2 = GRBaddconstr(model, numRelevantIndices, ind, val, GRB_GREATER_EQUAL, 1.0, NULL);
			if (error1 || error2) {
			  printf("ERROR %d 1st GRBaddconstr(): %s\n", error1, GRBgeterrormsg(env)); /* TODO: only referencing error1 */
			} else {
				retVal = true;
			}
		}
	}

	if (val != NULL) {
		free(val);
		val = NULL;
	}
	if (ind != NULL) {
		free(ind);
		ind = NULL;
	}

	return retVal;
}

bool addCategoryInstanceConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, int categoryNo, getRowBasedIDByCategoryBasedIDFunc getRowBasedIDfunc) {
	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	int value = 1;
	for (value = 1; value <= MN; value++)
		if (!addCategoryInstanceValueConstraint(env, model, board, numVars, cellLegalValuesIntBased, categoryNo, value, getRowBasedIDfunc))
			return false;

	return true;
}

bool addCategoryConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, getRowBasedIDByCategoryBasedIDFunc getRowBasedIDfunc) {
	int MN = board->numRowsInBlock_M * board->numColumnsInBlock_N;

	int categoryNo = 0;
	for (categoryNo = 0; categoryNo < MN; categoryNo++) {
		if (!addCategoryInstanceConstraints(env, model, board, numVars, cellLegalValuesIntBased, categoryNo, getRowBasedIDfunc))
			return false;
	}

	return true;
}

bool addCategoriesConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased) {
	return addCategoryConstraints(env, model, board, numVars, cellLegalValuesIntBased, getRowBasedIDGivenRowBasedID) &&
		   addCategoryConstraints(env, model, board, numVars, cellLegalValuesIntBased, getRowBasedIDGivenColumnBasedID) &&
		   addCategoryConstraints(env, model, board, numVars, cellLegalValuesIntBased, getRowBasedIDGivenBlockBasedID);
}

bool addSudokuConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased) {
	return addCellsConstraints(env, model, board, numVars, cellLegalValuesIntBased) &&
		   addCategoriesConstraints(env, model, board, numVars, cellLegalValuesIntBased);
}

typedef enum {
	SOLVE_MODEL_SUCCESS,
	SOLVE_MODEL_NO_SOLUTION_FOUND,
	SOLVE_MODEL_MODEL_IS_UNSOLVABLE,
	SOLVE_MODEL_OTHER_ERROR
} solveModelErrorCode;
solveModelErrorCode solveModel(GRBenv* env, GRBmodel* model) {
	int error = 0;

	int optimstatus = 0;

	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		return SOLVE_MODEL_OTHER_ERROR;
	}

	error = GRBwrite(model, "mip1.lp"); /* TODO: delete this in due time */
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		return SOLVE_MODEL_OTHER_ERROR;
	}

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		return SOLVE_MODEL_OTHER_ERROR;
	}

	if (optimstatus == GRB_OPTIMAL) {
	    return SOLVE_MODEL_SUCCESS;
	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		return SOLVE_MODEL_NO_SOLUTION_FOUND;
	}
	/* error or calculation stopped */
	else {
		return SOLVE_MODEL_MODEL_IS_UNSOLVABLE;
	}
}

void applySolutionToBoard(double* sol, int numVars, int*** cellLegalValuesIntBased, Board* boardSolution) {
	int MN = boardSolution->numRowsInBlock_M * boardSolution->numColumnsInBlock_N;

	int row = 0, col = 0;

	UNUSED(numVars); /* TODO: deal with this */

	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++) {
			if (isBoardCellEmpty(getBoardCellByRow(boardSolution, row, col))) {
				int value = 1;
				for (value = 1; value <= MN; value++) {
					int index = getIndexOfSpecificLegalValueOfCertainCell(row, col, value, cellLegalValuesIntBased);
					if (index >= 0) {
						if (sol[index] == 1.0) {
							boardSolution->cells[row][col].value = value;
							break;
						}
					}
				}
			}
		}
}

typedef enum {
	GET_SOLUTION_SUCCESS,
	GET_SOLUTION_MEMORY_ALLOCATION_FAILURE,
	GET_SOLUTION_GRB_ERROR
} getSolutionErrorCode;
getSolutionErrorCode getSolution(GRBenv* env, GRBmodel* model, int numVars, int*** cellLegalValuesIntBased, Board* boardSolution) {
	getSolutionErrorCode retVal = GET_SOLUTION_SUCCESS;

	int error = 0;

	double objval = 0;
	double* sol = NULL;

	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval); /* TODO: probably relevant for LP (not ILP) */
	if (error) {
		printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
		retVal = GET_SOLUTION_GRB_ERROR;
		return retVal;
	}

	sol = calloc(numVars, sizeof(double));
	if (sol == NULL) {
		retVal = GET_SOLUTION_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, numVars, sol);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		retVal = GET_SOLUTION_GRB_ERROR;
	} else {
		applySolutionToBoard(sol, numVars, cellLegalValuesIntBased, boardSolution);
	}

	free(sol);
	sol = NULL;

	return retVal;
}

solveBoardUsingLinearProgrammingErrorCode solveBoardUsingIntegerLinearProgramming(Board* board, Board* boardSolution) {
	solveBoardUsingLinearProgrammingErrorCode retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SUCCESS;

	int*** cellLegalValuesIntBased = NULL;
	int numVars = 0;
	GRBenv* env = NULL;

	if (!copyBoard(board, boardSolution)) {
		retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	if (!getLegalValuesForAllCells(board, &cellLegalValuesIntBased)) {
		retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}
	numVars = getTotalNumLegalValuesAndMakeNumsOfLegalValuesIncremental(board, cellLegalValuesIntBased);

	env = getNewGRBEnvironment();
	if (env == NULL)
		retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_CREATING_NEW_GRB_ENVIRONMENT;
	else {
		GRBmodel* model = getNewGRBModel(env);
		if (model == NULL)
			retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_CREATING_NEW_GRB_MODEL;
		else {
			if (!addVariablesAndObjectiveFunctionToModel(env, model, numVars)) /* In LP (not ILP) some indication will be sent from here to indicate the kind of objective func required */
				retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_ADDING_VARS_AND_OBJECTIVE_FUNC;
			else {
				if (!addSudokuConstraints(env, model, board, numVars, cellLegalValuesIntBased))
					retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_ADDING_CONSTRAINTS;
				else {
					switch (solveModel(env, model)) {
					case SOLVE_MODEL_SUCCESS:
						switch (getSolution(env, model, numVars, cellLegalValuesIntBased, boardSolution)) {
							case GET_SOLUTION_SUCCESS:
								break;
							case GET_SOLUTION_MEMORY_ALLOCATION_FAILURE:
								retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE;
								break;
							case GET_SOLUTION_GRB_ERROR:
								retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_ACQUIRING_MODEL_SOLUTION;
								break;
						}
						break;
					case SOLVE_MODEL_NO_SOLUTION_FOUND:
					case SOLVE_MODEL_MODEL_IS_UNSOLVABLE:
						retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_BOARD_ISNT_SOLVABLE;
						break;
					case SOLVE_MODEL_OTHER_ERROR:
						retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_OTHER_FAILURE;
						break;
					}
				}
			}
			freeGRBModel(model);
		}
		freeGRBEnvironment(env);
	}

	freeIntAndIndexBasedLegalValuesForAllCells(board, cellLegalValuesIntBased);

	return retVal;
}
