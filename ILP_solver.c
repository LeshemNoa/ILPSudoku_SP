#include <stdlib.h>

#include "ILP_solver.h"

#include "gurobi_c.h"

#define UNUSED(x) (void)(x)

void freeIntAndIndexBasedLegalValuesForAllCells(Board* board, int*** cellLegalValuesIntBased) {
	int MN = getBoardBlockSize_MN(board);

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

	int MN = getBoardBlockSize_MN(board);
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

	if (!getSuperficiallyLegalValuesForAllBoardCells(board, &cellsLegalValues)) {
		return false;
	}

	if (convertBooleanBasedLegalValuesForAllCellsToIntAndIndexBased(board, cellsLegalValues, &cellLegalValuesIntBased)) {
		*cellLegalValuesIntBasedOut = cellLegalValuesIntBased;
	} else
		retValue = false;

	freeCellsLegalValuesForAllBoardCells(board, cellsLegalValues);

	return retValue;
}

int getTotalNumLegalValuesAndMakeNumsOfLegalValuesIncremental(Board* board, int*** cellLegalValuesIntBased) {
	int totalNumLegalValues = 0;
	int MN = getBoardBlockSize_MN(board);

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

void freeGRBEnvironment(GRBenv* env) {
	GRBfreeenv(env);
}

GRBenv* getNewGRBEnvironment() {
	int error = 0;

	GRBenv* env = NULL;

	error = GRBloadenv(&env, NULL);
	if (error)
		return NULL;

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		freeGRBEnvironment(env);
		env = NULL;
	}

	return env;
}

GRBmodel* getNewGRBModel(GRBenv* env) {
	int error = 0;

	GRBmodel* model = NULL;

	error = GRBnewmodel(env, &model, NULL, 0, NULL, NULL, NULL, NULL, NULL); /* Note: Seems like name of model can be NULL (parameter 3) */
	if (error)
		return NULL;

	return model;
}

void freeGRBModel(GRBmodel* model) {
	GRBfreemodel(model);
}

typedef enum {
	ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_SUCCESS,
	ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_MEMORY_ALLOCATION_FAILURE,
	ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_GRB_COULD_NOT_ADD_VARS,
	ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_GRB_COULD_NOT_SET_OBJECTIVE,
	ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_GRB_COULD_NOT_UPDATE_MODEL
} addVariablesAndObjectiveFunctionToModelErrorCode;
addVariablesAndObjectiveFunctionToModelErrorCode addVariablesAndObjectiveFunctionToModel(GRBenv* env, GRBmodel* model, int numVars, Board* board, solveBoardUsingLinearProgrammingSolvingMode solvingMode) {
	addVariablesAndObjectiveFunctionToModelErrorCode retVal = ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_SUCCESS;

	int MN = 0;

	double* obj = NULL;
	char* vtype = NULL;

	UNUSED(env);

	if (board != NULL)
		MN = getBoardBlockSize_MN(board);

	obj = calloc(numVars, sizeof(double));
	vtype = calloc(numVars, sizeof(char));
	if ((obj == NULL) || (vtype == NULL))
		retVal = ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_MEMORY_ALLOCATION_FAILURE;
	else {
		int error = 0;

		int i = 0;
		for (i = 0; i < numVars; i++)
			if (solvingMode == SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_ILP) {
				obj[i] = 0;
				vtype[i] = GRB_BINARY;
			}
			else if (solvingMode == SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_LP) {
				obj[i] = (rand() % (MN * MN)) + 1; /* Note: this factor (MN^2) seems to be effective */
				vtype[i] = GRB_CONTINUOUS;
			}

		error = GRBaddvars(model, numVars, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
		if (!error) {
			error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
			if (!error) {
				error = GRBupdatemodel(model);
				  if (!error) {
				  } else
					  retVal = ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_GRB_COULD_NOT_UPDATE_MODEL;
			} else
				retVal = ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_GRB_COULD_NOT_SET_OBJECTIVE;
		} else
			retVal = ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_GRB_COULD_NOT_ADD_VARS;

	}

	if (obj != NULL)
		free(obj);
	if (vtype != NULL)
		free(vtype);

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

typedef enum {
	ADD_CONSTRAINTS_FUNCS_SUCCESS,
	ADD_CONSTRAINTS_FUNCS_MEMORY_ALLOCATION_FAILURE,
	ADD_CONSTRAINTS_FUNCS_GRB_COULD_NOT_ADD_CONSTRAINT
} addConstraintsFuncsErrorCode;

addConstraintsFuncsErrorCode addCellConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, int row, int col, solveBoardUsingLinearProgrammingSolvingMode solvingMode) {
	addConstraintsFuncsErrorCode retVal = ADD_CONSTRAINTS_FUNCS_SUCCESS;

	int* ind = NULL;
	double* val = NULL;

	int MN = getBoardBlockSize_MN(board);

	UNUSED(env);
	UNUSED(numVars);

	ind = calloc(MN, sizeof(int));
	val = calloc(MN, sizeof(double));

	if ((ind == NULL) || (val == NULL))
		retVal = ADD_CONSTRAINTS_FUNCS_MEMORY_ALLOCATION_FAILURE;
	else {
		int numLegalValues = 0;

		int value = 1;
		for (value = 1; value <= MN; value++) {
			int index = getIndexOfSpecificLegalValueOfCertainCell(row, col, value, cellLegalValuesIntBased);
			if (index >= 0) {

				if (solvingMode == SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_LP) {
					int error1 = 0, error2 = 0;
					double coefficient = 1;
					error1 = GRBaddconstr(model, 1, &index, &coefficient, GRB_LESS_EQUAL, 1.0, NULL);
					error2 = GRBaddconstr(model, 1, &index, &coefficient, GRB_GREATER_EQUAL, 0.0, NULL);
					if (error1 || error2)
						retVal = ADD_CONSTRAINTS_FUNCS_GRB_COULD_NOT_ADD_CONSTRAINT;
				}

				ind[numLegalValues] = index;
				val[numLegalValues] = 1;
				numLegalValues++;
			}
		}

		if (numLegalValues > 0) {
			int error1 = 0, error2 = 0;
			error1 = GRBaddconstr(model, numLegalValues, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
			error2 = GRBaddconstr(model, numLegalValues, ind, val, GRB_GREATER_EQUAL, 1.0, NULL);
			if (error1 || error2)
				  retVal = ADD_CONSTRAINTS_FUNCS_GRB_COULD_NOT_ADD_CONSTRAINT;
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

addConstraintsFuncsErrorCode addCellsConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, solveBoardUsingLinearProgrammingSolvingMode solvingMode) {
	int MN = getBoardBlockSize_MN(board);
	int row = 0, col = 0;
	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++) {
			addConstraintsFuncsErrorCode retVal = addCellConstraints(env, model, board, numVars, cellLegalValuesIntBased, row, col, solvingMode);
			if (retVal != ADD_CONSTRAINTS_FUNCS_SUCCESS)
				return retVal;
		}
	return ADD_CONSTRAINTS_FUNCS_SUCCESS;
}

addConstraintsFuncsErrorCode addCategoryInstanceValueConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, int categoryNo, int value, getCategory1BasedIDByCategory2BasedIDFunc getRowBasedIDfunc) {
	addConstraintsFuncsErrorCode retVal = ADD_CONSTRAINTS_FUNCS_SUCCESS;

	int* ind = NULL;
	double* val = NULL;

	int MN = getBoardBlockSize_MN(board);

	UNUSED(env);
	UNUSED(numVars);

	ind = calloc(MN, sizeof(int));
	val = calloc(MN, sizeof(double));

	if ((ind == NULL) || (val == NULL))
		retVal = ADD_CONSTRAINTS_FUNCS_MEMORY_ALLOCATION_FAILURE;
	else {
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

		if (numRelevantIndices > 0) {
			int error1 = 0, error2 = 0;
			error1 = GRBaddconstr(model, numRelevantIndices, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
			error2 = GRBaddconstr(model, numRelevantIndices, ind, val, GRB_GREATER_EQUAL, 1.0, NULL);
			if (error1 || error2)
				retVal = ADD_CONSTRAINTS_FUNCS_GRB_COULD_NOT_ADD_CONSTRAINT;
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

addConstraintsFuncsErrorCode addCategoryInstanceConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, int categoryNo, getCategory1BasedIDByCategory2BasedIDFunc getRowBasedIDfunc) {
	int MN = getBoardBlockSize_MN(board);
	int value = 1;
	for (value = 1; value <= MN; value++) {
		addConstraintsFuncsErrorCode retVal = addCategoryInstanceValueConstraints(env, model, board, numVars, cellLegalValuesIntBased, categoryNo, value, getRowBasedIDfunc);
		if (retVal != ADD_CONSTRAINTS_FUNCS_SUCCESS)
			return retVal;
	}

	return ADD_CONSTRAINTS_FUNCS_SUCCESS;
}

addConstraintsFuncsErrorCode addCategoryConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, getCategory1BasedIDByCategory2BasedIDFunc getRowBasedIDfunc) {
	int MN = getBoardBlockSize_MN(board);

	int categoryNo = 0;
	for (categoryNo = 0; categoryNo < MN; categoryNo++) {
		addConstraintsFuncsErrorCode retVal = addCategoryInstanceConstraints(env, model, board, numVars, cellLegalValuesIntBased, categoryNo, getRowBasedIDfunc);
		if (retVal != ADD_CONSTRAINTS_FUNCS_SUCCESS)
			return retVal;
	}

	return ADD_CONSTRAINTS_FUNCS_SUCCESS;
}

addConstraintsFuncsErrorCode addCategoriesConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased) {
	addConstraintsFuncsErrorCode retVal = ADD_CONSTRAINTS_FUNCS_SUCCESS;

	retVal = addCategoryConstraints(env, model, board, numVars, cellLegalValuesIntBased, getRowBasedIDGivenRowBasedID);
	if (retVal != ADD_CONSTRAINTS_FUNCS_SUCCESS)
		return retVal;
	retVal = addCategoryConstraints(env, model, board, numVars, cellLegalValuesIntBased, getRowBasedIDGivenColumnBasedID);
	if (retVal != ADD_CONSTRAINTS_FUNCS_SUCCESS)
			return retVal;
	retVal = addCategoryConstraints(env, model, board, numVars, cellLegalValuesIntBased, getRowBasedIDGivenBlockBasedID);
	if (retVal != ADD_CONSTRAINTS_FUNCS_SUCCESS)
			return retVal;

	return ADD_CONSTRAINTS_FUNCS_SUCCESS;
}

addConstraintsFuncsErrorCode addSudokuConstraints(GRBenv* env, GRBmodel* model, Board* board, int numVars, int*** cellLegalValuesIntBased, solveBoardUsingLinearProgrammingSolvingMode solvingMode) {
	addConstraintsFuncsErrorCode retVal = ADD_CONSTRAINTS_FUNCS_SUCCESS;

	retVal = addCellsConstraints(env, model, board, numVars, cellLegalValuesIntBased, solvingMode);
	if (retVal != ADD_CONSTRAINTS_FUNCS_SUCCESS)
		return retVal;
	retVal = addCategoriesConstraints(env, model, board, numVars, cellLegalValuesIntBased);
	if (retVal != ADD_CONSTRAINTS_FUNCS_SUCCESS)
			return retVal;

	return ADD_CONSTRAINTS_FUNCS_SUCCESS;
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

	UNUSED(env);

	error = GRBoptimize(model);
	if (error)
		return SOLVE_MODEL_OTHER_ERROR;

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error)
		return SOLVE_MODEL_OTHER_ERROR;

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
	int MN = getBoardBlockSize_MN(boardSolution);

	int row = 0, col = 0;

	UNUSED(numVars);

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

void applySolutionToValuesScoresArray(double* sol, int numVars, int*** cellLegalValuesIntBased, Board* board, double*** allCellsValuesScores) {
	int MN = getBoardBlockSize_MN(board);

	int row = 0, col = 0;

	UNUSED(numVars);

	for (row = 0; row < MN; row++)
		for (col = 0; col < MN; col++) {
			if (isBoardCellEmpty(getBoardCellByRow(board, row, col))) {
				int value = 1;
				for (value = 1; value <= MN; value++) {
					int index = 0;

					allCellsValuesScores[row][col][value] = 0;

					index = getIndexOfSpecificLegalValueOfCertainCell(row, col, value, cellLegalValuesIntBased);
					if (index >= 0) {
						allCellsValuesScores[row][col][value] = sol[index];
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
getSolutionErrorCode getSolution(GRBenv* env, GRBmodel* model, int numVars, int*** cellLegalValuesIntBased, Board* board, Board* boardSolution, double*** allCellsValuesScores, solveBoardUsingLinearProgrammingSolvingMode solvingMode) {
	getSolutionErrorCode retVal = GET_SOLUTION_SUCCESS;

	int error = 0;

	double* sol = NULL;

	UNUSED(env);

	sol = calloc(numVars, sizeof(double));
	if (sol == NULL) {
		retVal = GET_SOLUTION_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, numVars, sol);
	if (error) {
		retVal = GET_SOLUTION_GRB_ERROR;
	} else {
		if (solvingMode == SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_ILP) {
			applySolutionToBoard(sol, numVars, cellLegalValuesIntBased, boardSolution);
		} else if (solvingMode == SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_LP) {
			applySolutionToValuesScoresArray(sol, numVars, cellLegalValuesIntBased, board, allCellsValuesScores);

		} /* No other option */
	}

	free(sol);
	sol = NULL;

	return retVal;
}

solveBoardUsingLinearProgrammingErrorCode solveBoardUsingLinearProgramming(solveBoardUsingLinearProgrammingSolvingMode solvingMode, Board* board, Board* boardSolution, double*** allCellsValuesScores) {
	solveBoardUsingLinearProgrammingErrorCode retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SUCCESS;

	int*** cellLegalValuesIntBased = NULL;
	int numVars = 0;
	GRBenv* env = NULL;

	if (solvingMode == SOLVE_BOARD_USING_LINEAR_PROGRAMMING_SOLVING_MODE_ILP) { /* Mode: ILP */
		if (!copyBoard(board, boardSolution)) {
			retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE;
			return retVal;
		}
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
			bool shouldContinue = false;
			switch (addVariablesAndObjectiveFunctionToModel(env, model, numVars, board, solvingMode)) {
			case ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_SUCCESS:
				shouldContinue = true;
				break;
			case ADD_VARIABLES_AND_OBJECTIVE_FUNCTION_TO_MODEL_MEMORY_ALLOCATION_FAILURE:
				retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE;
				break;
			default:
				retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_ADDING_VARS_AND_OBJECTIVE_FUNC;
				break;
			}
			if (shouldContinue) {
				shouldContinue = false;
				switch (addSudokuConstraints(env, model, board, numVars, cellLegalValuesIntBased, solvingMode)) {
				case ADD_CONSTRAINTS_FUNCS_SUCCESS:
					shouldContinue = true;
					break;
				case ADD_CONSTRAINTS_FUNCS_MEMORY_ALLOCATION_FAILURE:
					retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_MEMORY_ALLOCATION_FAILURE;
					break;
				case ADD_CONSTRAINTS_FUNCS_GRB_COULD_NOT_ADD_CONSTRAINT:
					retVal = SOLVE_BOARD_USING_LINEAR_PROGRAMMING_GRB_ERROR_FAILURE_ADDING_CONSTRAINTS;
				}
				if (shouldContinue) {
					switch (solveModel(env, model)) {
					case SOLVE_MODEL_SUCCESS:
						switch (getSolution(env, model, numVars, cellLegalValuesIntBased, board, boardSolution, allCellsValuesScores, solvingMode)) {
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

void freeValuesScoresArr(double*** valuesScores, Board* board) {
	int MN = getBoardBlockSize_MN(board);

	if (valuesScores != NULL) {
		int row = 0;
		for (row = 0; row < MN; row++) {
			if (valuesScores[row] != NULL) {
				int col = 0;
				for (col = 0; col < MN; col++) {
					if (valuesScores[row][col] != NULL) {
						free(valuesScores[row][col]);
						valuesScores[row][col] = NULL;
					}
				}
				free(valuesScores[row]);
				valuesScores[row] = NULL;
			}
		}
		free(valuesScores);
	}
}

bool allocateValuesScoresArr(double**** valuesScoresOut, Board* board) {
	int MN = getBoardBlockSize_MN(board);

	bool success = true;

	double*** valuesScores = NULL;
	valuesScores = calloc(MN, sizeof(double**));
	if (valuesScores == NULL)
		success = false;
	else {
		int row = 0;
		for (row = 0; row < MN; row++) {
			valuesScores[row] = calloc(MN, sizeof(double*));
			if (valuesScores[row] == NULL) {
				success = false;
				break;
			} else {
				int col = 0;
				for (col = 0; col < MN; col++) {
					if (isBoardCellEmpty(getBoardCellByRow(board, row, col))) {
						valuesScores[row][col] = calloc(MN + 1, sizeof(double));
						if (valuesScores[row][col] == NULL) {
							success = false;
							break;
						}
					}
					if (!success)
						break;
				}
			}

		}
	}

	if (!success) {
		freeValuesScoresArr(valuesScores, board);
		return false;
	}
	else {
		*valuesScoresOut = valuesScores;
		return true;
	}

}
