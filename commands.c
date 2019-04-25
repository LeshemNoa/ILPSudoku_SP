#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "commands.h"

#include "board.h"
#include "parser.h"
#include "LP_solver.h"
#include "linked_list.h" /* CR: added this include so code will compile */

#define UNUSED(x) (void)(x)

#define DEFAULT_M (3)
#define DEFAULT_N (3)

#define GENERATE_COMMAND_MAX_NUM_TRIES (1000)

#define GUESS_THRESHOLD_MIN_VALUE (0.0)
#define GUESS_THRESHOLD_MAX_VALUE (1.0)

#define COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR ("memory allocation failure\n")

typedef char* (*commandArgsGetExpectedRangeStringFunc)(int argNo, GameState* gameState);
typedef char* (*getCommandArgsValidatorErrorStringFunc)(int error);
typedef char* (*getCommandErrorStringFunc)(int error);
typedef bool (*isCommandErrorRecoverableFunc)(int error);
typedef char* (*getCommandStrOutputFunc)(Command* command, GameState* gameState);

int getNumDecDigitsInNumber(int num) { /* Note: assumed to be non-negative */
	if (num == 0)
		return 1;
	return floor(log10(num)) + 1;
}

bool isCommandAllowed(GameMode gameMode, CommandType commandType) {
	switch (gameMode) {
	case GAME_MODE_INIT:
		switch (commandType) {
		case COMMAND_TYPE_SOLVE:
		case COMMAND_TYPE_EDIT:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE:
			return true;
		default:
			return false;
		}
		break;
	case GAME_MODE_EDIT:
		switch (commandType) {
		case COMMAND_TYPE_SOLVE:
		case COMMAND_TYPE_EDIT:
		case COMMAND_TYPE_PRINT_BOARD:
		case COMMAND_TYPE_SET:
		case COMMAND_TYPE_VALIDATE:
		case COMMAND_TYPE_GENERATE:
		case COMMAND_TYPE_UNDO:
		case COMMAND_TYPE_REDO:
		case COMMAND_TYPE_SAVE:
		case COMMAND_TYPE_NUM_SOLUTIONS:
		case COMMAND_TYPE_RESET:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE:
			return true;
		default:
			return false;
		}
		break;
	case GAME_MODE_SOLVE:
		switch (commandType) {
		case COMMAND_TYPE_SOLVE:
		case COMMAND_TYPE_EDIT:
		case COMMAND_TYPE_MARK_ERRORS:
		case COMMAND_TYPE_PRINT_BOARD:
		case COMMAND_TYPE_SET:
		case COMMAND_TYPE_VALIDATE:
		case COMMAND_TYPE_GUESS:
		case COMMAND_TYPE_UNDO:
		case COMMAND_TYPE_REDO:
		case COMMAND_TYPE_SAVE:
		case COMMAND_TYPE_HINT:
		case COMMAND_TYPE_GUESS_HINT:
		case COMMAND_TYPE_NUM_SOLUTIONS:
		case COMMAND_TYPE_AUTOFILL:
		case COMMAND_TYPE_RESET:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE:
			return true;
		default:
			return false;
		}
		break;
	}
	return false;
}

char* getAllowedCommandsString(GameMode gameMode) {
	switch (gameMode) {
	case GAME_MODE_INIT:
		return INIT_MODE_LIST_OF_ALLOWED_COMMANDS;
	case GAME_MODE_EDIT:
		return EDIT_MODE_LIST_OF_ALLOWED_COMMANDS;
	case GAME_MODE_SOLVE:
		return SOLVE_MODE_LIST_OF_ALLOWED_COMMANDS;
	}
	return NULL;
}

char* getAllowingModesString(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_SOLVE:
		return SOLVE_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_EDIT:
		return EDIT_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_MARK_ERRORS:
		return MARK_ERRORS_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_PRINT_BOARD:
		return PRINT_BOARD_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_SET:
		return SET_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_VALIDATE:
		return VALIDATE_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_GUESS:
		return GUESS_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_GENERATE:
		return GENERATE_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_UNDO:
		return UNDO_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_REDO:
		return REDO_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_SAVE:
		return SAVE_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_HINT:
		return HINT_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_GUESS_HINT:
		return GUESS_HINT_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_NUM_SOLUTIONS:
		return NUM_SOLUTIONS_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_AUTOFILL:
		return AUTOFILL_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_RESET:
		return RESET_COMMAND_LIST_OF_ALLOWING_STATES;
	case COMMAND_TYPE_EXIT:
		return EXIT_COMMAND_LIST_OF_ALLOWING_STATES;
	default:
		return NULL;
	}
}

bool isCorrectArgumentsNum(Command* command) {
	switch (command->type) {
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_VALIDATE:
	case COMMAND_TYPE_UNDO:
	case COMMAND_TYPE_REDO:
	case COMMAND_TYPE_NUM_SOLUTIONS:
	case COMMAND_TYPE_AUTOFILL:
	case COMMAND_TYPE_RESET:
	case COMMAND_TYPE_EXIT:
	case COMMAND_TYPE_IGNORE:
		if (command->argumentsNum == 0)
			return true;
		break;
	case COMMAND_TYPE_SOLVE:
	case COMMAND_TYPE_MARK_ERRORS:
	case COMMAND_TYPE_GUESS:
	case COMMAND_TYPE_SAVE:
		if (command->argumentsNum == 1)
			return true;
		break;
	case COMMAND_TYPE_GENERATE:
	case COMMAND_TYPE_HINT:
	case COMMAND_TYPE_GUESS_HINT:
		if (command->argumentsNum == 2)
			return true;
		break;
	case COMMAND_TYPE_SET:
		if (command->argumentsNum == 3)
			return true;
		break;
	case COMMAND_TYPE_EDIT:
		if (command->argumentsNum >= 0 && command->argumentsNum <= 1)
			return true;
		break;
	}
	return false;
}

char* getCommandUsage(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_SOLVE:
		return SOLVE_COMMAND_USAGE;
	case COMMAND_TYPE_EDIT:
		return EDIT_COMMAND_USAGE;
	case COMMAND_TYPE_MARK_ERRORS:
		return MARK_ERRORS_COMMAND_USAGE;
	case COMMAND_TYPE_PRINT_BOARD:
		return PRINT_BOARD_COMMAND_USAGE;
	case COMMAND_TYPE_SET:
		return SET_COMMAND_USAGE;
	case COMMAND_TYPE_VALIDATE:
		return VALIDATE_COMMAND_USAGE;
	case COMMAND_TYPE_GUESS:
		return GUESS_COMMAND_USAGE;
	case COMMAND_TYPE_GENERATE:
		return GENERATE_COMMAND_USAGE;
	case COMMAND_TYPE_UNDO:
		return UNDO_COMMAND_USAGE;
	case COMMAND_TYPE_REDO:
		return REDO_COMMAND_USAGE;
	case COMMAND_TYPE_SAVE:
		return SAVE_COMMAND_USAGE;
	case COMMAND_TYPE_HINT:
		return HINT_COMMAND_USAGE;
	case COMMAND_TYPE_GUESS_HINT:
		return GUESS_HINT_COMMAND_USAGE;
	case COMMAND_TYPE_NUM_SOLUTIONS:
		return NUM_SOLUTIONS_COMMAND_USAGE;
	case COMMAND_TYPE_AUTOFILL:
		return AUTOFILL_COMMAND_USAGE;
	case COMMAND_TYPE_RESET:
		return RESET_COMMAND_USAGE;
	case COMMAND_TYPE_EXIT:
		return EXIT_COMMAND_USAGE;
	default:
		return NULL;
	}
}

bool identifyCommandByType(char* commandType, Command* commandOut) {
	if (commandType == NULL) {
		commandOut->type = COMMAND_TYPE_IGNORE;
	} else if (strcmp(commandType, SOLVE_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_SOLVE;
	} else if (strcmp(commandType, EDIT_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_EDIT;
	} else if (strcmp(commandType, MARK_ERRORS_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_MARK_ERRORS;
	} else if (strcmp(commandType, PRINT_BOARD_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_PRINT_BOARD;
	} else if (strcmp(commandType, SET_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_SET;
	} else if (strcmp(commandType, VALIDATE_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_VALIDATE;
	} else if (strcmp(commandType, GUESS_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_GUESS;
	} else if (strcmp(commandType, GENERATE_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_GENERATE;
	} else if (strcmp(commandType, UNDO_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_UNDO;
	} else if (strcmp(commandType, REDO_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_REDO;
	} else if (strcmp(commandType, SAVE_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_SAVE;
	} else if (strcmp(commandType, HINT_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_HINT;
	} else if (strcmp(commandType, GUESS_HINT_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_GUESS_HINT;
	} else if (strcmp(commandType, NUM_SOLUTIONS_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_NUM_SOLUTIONS;
	} else if (strcmp(commandType, AUTOFILL_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_AUTOFILL;
	} else if (strcmp(commandType, RESET_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_RESET;
	} else if (strcmp(commandType, EXIT_COMMAND_TYPE_STRING) == 0) {
		commandOut->type = COMMAND_TYPE_EXIT;
	} else {
		return false;
	}
	return true;
}

int getSizeofCommandArgsStruct(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_SOLVE:
		return sizeof(SolveCommandArguments);
	case COMMAND_TYPE_EDIT:
		return sizeof(EditCommandArguments);
	case COMMAND_TYPE_MARK_ERRORS:
		return sizeof(MarkErrorsCommandArguments);
	case COMMAND_TYPE_PRINT_BOARD:
		return sizeof(PrintBoardCommandArguments);
	case COMMAND_TYPE_SET:
		return sizeof(SetCommandArguments);
	case COMMAND_TYPE_VALIDATE:
		return sizeof(ValidateCommandArguments);
	case COMMAND_TYPE_GUESS:
		return sizeof(GuessCommandArguments);
	case COMMAND_TYPE_GENERATE:
		return sizeof(GenerateCommandArguments);
	case COMMAND_TYPE_UNDO:
		return sizeof(UndoCommandArguments);
	case COMMAND_TYPE_REDO:
		return sizeof(RedoCommandArguments);
	case COMMAND_TYPE_SAVE:
		return sizeof(SaveCommandArguments);
	case COMMAND_TYPE_HINT:
		return sizeof(HintCommandArguments);
	case COMMAND_TYPE_GUESS_HINT:
		return sizeof(GuessHintCommandArguments);
	case COMMAND_TYPE_NUM_SOLUTIONS:
		return sizeof(NumSolutionsCommandArguments);
	case COMMAND_TYPE_AUTOFILL:
		return sizeof(AutofillCommandArguments);
	case COMMAND_TYPE_RESET:
		return sizeof(ResetCommandArguments);
	case COMMAND_TYPE_EXIT:
		return sizeof(ExitCommandArguments);
	case COMMAND_TYPE_IGNORE:
		return sizeof(IgnoreCommandArguments);
	}
	return 0;
}

bool solveArgsParser(char* arg, int argNo, void* arguments) {
	SolveCommandArguments* solveArguments = (SolveCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseStringArg(arg, &(solveArguments->filePath));
	}
	return false;
}

bool editArgsParser(char* arg, int argNo, void* arguments) {
	EditCommandArguments* editArguments = (EditCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseStringArg(arg, &(editArguments->filePath)); /* TODO: perhaps document that we were instructed that paths do not contains spaces */
	}
	return false;
}

bool markErrorsArgsParser(char* arg, int argNo, void* arguments) {
	MarkErrorsCommandArguments* markErrorsArguments = (MarkErrorsCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseBooleanIntArg(arg, &(markErrorsArguments->shouldMarkError));
	}
	return false;
}

/**
 * setArgsParser concretely implements an argument parser for the 'set' command.
 *
 * @param arg			[in] the string containing the specific argument currently
 * 						being parsed and assigned to the appropriate attribute
 * @param argsGameState		[in, out] a generic pointer to a command argument struct, casted
 * 						to be a SetCommandArgument struct containing the arguments
 * @param argNo 		[in] the parsed argument's index: argument 1 is the number of
 * 						column of the cell for which the user requested a hint, argument
 * 						2 is the number of the row, and argument 3 is the value to be set
 * 						in the cell with those indices
 * @return true 		iff parseIntArg successfully parsed and set a valid integer
 * @return false 		iff the parsing failed
 */

bool setArgsParser(char* arg, int argNo, void* arguments) {
	SetCommandArguments* setArguments = (SetCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseIntArgOffset(arg, &(setArguments->col), -1);
	case 2:
		return parseIntArgOffset(arg, &(setArguments->row), -1);
	case 3:
		return parseIntArg(arg, &(setArguments->value));
	}
	return false;
}

bool setArgsRangeChecker(void* arguments, int argNo, GameState* gameState) {
	SetCommandArguments* setArguments = (SetCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return isIndexInRange(gameState, setArguments->col);
	case 2:
		return isIndexInRange(gameState, setArguments->row);
	case 3:
		return isCellValueInRange(gameState, setArguments->value);
	}
	return false;
}

#define INT_RANGE_FORMAT ("%c%-2d, %2d%c")
#define INT_RANGE_FORMAT_SIZE (sizeof(INT_RANGE_FORMAT)) /* Note: this is an upper boundary in current implementation */

#define REAL_RANGE_FORMAT ("%c%-4.1f, %4.1f%c")
#define REAL_RANGE_FORMAT_SIZE (sizeof(REAL_RANGE_FORMAT)) /* Note: this is an upper boundary in current implementation */

#define SET_EMPTY_VALUE_STRING_FOR_EXPECTED_RANGE (", or %2d to clear")
#define SET_EMPTY_VALUE_STRING_FOR_EXPECTED_RANGE_SIZE (sizeof(SET_EMPTY_VALUE_STRING_FOR_EXPECTED_RANGE)) /* Note: this is an upper boundary in current implementation */

#define INCLUSIVE_OPENER ('[')
#define INCLUSIVE_CLOSER (']')
#define EXCLUSIVE_OPENER ('(')
#define EXCLUSIVE_CLOSER ('(')

char* setArgsGetExpectedRangeString(int argNo, GameState* gameState) {
	char* str = NULL;
	char* complexFormat = NULL;

	switch (argNo) {
	case 1:
	case 2:
		str = calloc(INT_RANGE_FORMAT_SIZE, sizeof(char));
		if (str != NULL)
			sprintf(str, INT_RANGE_FORMAT, INCLUSIVE_OPENER, 1, getBlockSize_MN(gameState), INCLUSIVE_CLOSER);
		break;
	case 3:
		complexFormat = calloc(INT_RANGE_FORMAT_SIZE + SET_EMPTY_VALUE_STRING_FOR_EXPECTED_RANGE_SIZE, sizeof(char));
		str = calloc(INT_RANGE_FORMAT_SIZE + SET_EMPTY_VALUE_STRING_FOR_EXPECTED_RANGE_SIZE, sizeof(char));
		if ((complexFormat != NULL) && (str != NULL)) {
			strcpy(complexFormat, INT_RANGE_FORMAT);
			strcat(complexFormat, SET_EMPTY_VALUE_STRING_FOR_EXPECTED_RANGE);
			sprintf(str, complexFormat, INCLUSIVE_OPENER, 1, getBlockSize_MN(gameState), INCLUSIVE_CLOSER, EMPTY_CELL_VALUE);
		}
		if (complexFormat != NULL)
			free(complexFormat);
		break;
	}

	return str;
}

bool guessArgsParser(char* arg, int argNo, void* arguments) {
	GuessCommandArguments* guessArguments = (GuessCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseDoubleArg(arg, &(guessArguments->threshold));
	}
	return false;
}

bool guessArgsRangeChecker(void* arguments, int argNo, GameState* gameState) {
	GuessCommandArguments* guessArguments = (GuessCommandArguments*)arguments;

	UNUSED(gameState);

	switch (argNo) {
	case 1:
		return (guessArguments->threshold > GUESS_THRESHOLD_MIN_VALUE) &&
			   (guessArguments->threshold <= GUESS_THRESHOLD_MAX_VALUE);
	}
	return false;
}

char* guessArgsGetExpectedRangeString(int argNo, GameState* gameState) {
	char* str = NULL;

	UNUSED(gameState);

	switch (argNo) {
	case 1:
		str = calloc(REAL_RANGE_FORMAT_SIZE, sizeof(char));
		if (str != NULL)
			sprintf(str, REAL_RANGE_FORMAT, EXCLUSIVE_OPENER, GUESS_THRESHOLD_MIN_VALUE, GUESS_THRESHOLD_MAX_VALUE, INCLUSIVE_CLOSER);
		break;
	}

	return str;
}

bool generateArgsParser(char* arg, int argNo, void* arguments) {
	GenerateCommandArguments* generateArguments = (GenerateCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseIntArg(arg, &(generateArguments->numEmptyCellsToFill));
	case 2:
		return parseIntArg(arg, &(generateArguments->numCellsToClear));
	}
	return false;
}

bool generateArgsRangeChecker(void* arguments, int argNo, GameState* gameState) {
	GenerateCommandArguments* generateArguments = (GenerateCommandArguments*)arguments;
		switch (argNo) {
		case 1:
			return generateArguments->numEmptyCellsToFill >= 0 && generateArguments->numEmptyCellsToFill <= getPuzzleBoardSize_MN2(gameState);
		case 2:
			return generateArguments->numCellsToClear >= 0 && generateArguments->numCellsToClear <= getPuzzleBoardSize_MN2(gameState);
		}
		return false;
}

char* generateArgsGetExpectedRangeString(int argNo, GameState* gameState) {
	char* str = NULL;

	switch (argNo) {
	case 1:
	case 2:
		str = calloc(INT_RANGE_FORMAT_SIZE, sizeof(char));
		if (str != NULL)
			sprintf(str, INT_RANGE_FORMAT, INCLUSIVE_OPENER, 0, getPuzzleBoardSize_MN2(gameState), INCLUSIVE_CLOSER);
		break;
	}

	return str;
}


typedef enum {
	GENERATE_ARGS_VALIDATOR_NOT_ENOUGH_EMPTY_CELLS
} GenerateArgsValidatorErrorCode;
bool generateArgsValidator(void* arguments, int argNo, GameState* gameState, int* errorOut) {
	GenerateCommandArguments* generateArguments = (GenerateCommandArguments*)arguments;
		switch (argNo) {
		case 1:
			if (generateArguments->numEmptyCellsToFill > getNumEmptyCells(gameState)) {
				*errorOut = GENERATE_ARGS_VALIDATOR_NOT_ENOUGH_EMPTY_CELLS;
				return false;
			}
			return true;
		case 2:
			return true;
		}
		return false;
}

#define GENERATE_COMMAND_ERROR_NOT_ENOUGH_EMPTY_BOARDS_IN_CELL ("there are not enough empty cells in board")

char* getGenerateArgsValidatorErrorString(int error) {
	GenerateArgsValidatorErrorCode errorCode = error;
	switch (errorCode) {
	case GENERATE_ARGS_VALIDATOR_NOT_ENOUGH_EMPTY_CELLS:
		return GENERATE_COMMAND_ERROR_NOT_ENOUGH_EMPTY_BOARDS_IN_CELL;
	}
	return NULL;
}

bool saveArgsParser(char* arg, int argNo, void* arguments) {
	SaveCommandArguments* saveArguments = (SaveCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseStringArg(arg, &(saveArguments->filePath));
	}
	return false;
}

/**
 * hintArgsParser concretely implements an argument parser for the 'hint' command.
 *
 * @param arg	 		[in] the string containing the specific argument currently
 * 						being parsed and assigned to the appropriate attribute
 * @param argsGameState		[in, out] a generic pointer to a command argument struct, casted
 * 						to be a HintCommandArgument struct containing the arguments
 * @param argNo 		[in] the parsed argument's index: argument 1 is the number of
 * 						column of the cell for which the user requested a hint, and
 * 						argument 2 is the number of the row
 * @return true			iff parseIntArg successfully parsed and set a valid integer
 * @return false 		iff the parsing failed
 */
bool hintArgsParser(char* arg, int argNo, void* arguments) {
	HintCommandArguments* hintArguments = (HintCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseIntArgOffset(arg, &(hintArguments->col), -1);
	case 2:
		return parseIntArgOffset(arg, &(hintArguments->row), -1);
	}
	return false;
}

bool hintArgsRangeChecker(void* arguments, int argNo, GameState* gameState) {
	HintCommandArguments* hintArguments = (HintCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return isIndexInRange(gameState, hintArguments->col);
	case 2:
		return isIndexInRange(gameState, hintArguments->row);
	}
	return false;
}

char* hintArgsGetExpectedRangeString(int argNo, GameState* gameState) {
	char* str = NULL;

	switch (argNo) {
	case 1:
	case 2:
		str = calloc(INT_RANGE_FORMAT_SIZE, sizeof(char));
		if (str != NULL)
			sprintf(str, INT_RANGE_FORMAT, INCLUSIVE_OPENER, 1, getBlockSize_MN(gameState), INCLUSIVE_CLOSER);
		break;
	}

	return str;
}

bool guessHintArgsParser(char* arg, int argNo, void* arguments) {
	GuessHintCommandArguments* guessHintArguments = (GuessHintCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseIntArgOffset(arg, &(guessHintArguments->col), -1);
	case 2:
		return parseIntArgOffset(arg, &(guessHintArguments->row), -1);
	}
	return false;
}

bool guessHintArgsRangeChecker(void* arguments, int argNo, GameState* gameState) {
	GuessHintCommandArguments* guessHintArguments = (GuessHintCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return isIndexInRange(gameState, guessHintArguments->col);
	case 2:
		return isIndexInRange(gameState, guessHintArguments->row);
	}
	return false;
}

char* guessHintArgsGetExpectedRangeString(int argNo, GameState* gameState) {
	char* str = NULL;

	switch (argNo) {
	case 1:
	case 2:
		str = calloc(INT_RANGE_FORMAT_SIZE, sizeof(char));
		if (str != NULL)
			sprintf(str, INT_RANGE_FORMAT, INCLUSIVE_OPENER, 1, getBlockSize_MN(gameState), INCLUSIVE_CLOSER);
		break;
	}

	return str;
}

commandArgsParser getCommandArgsParser(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_SOLVE:
		return solveArgsParser;
	case COMMAND_TYPE_EDIT:
		return editArgsParser;
	case COMMAND_TYPE_MARK_ERRORS:
		return markErrorsArgsParser;
	case COMMAND_TYPE_SET:
		return setArgsParser;
	case COMMAND_TYPE_GUESS:
		return guessArgsParser;
	case COMMAND_TYPE_GENERATE:
		return generateArgsParser;
	case COMMAND_TYPE_SAVE:
		return saveArgsParser;
	case COMMAND_TYPE_HINT:
		return hintArgsParser;
	case COMMAND_TYPE_GUESS_HINT:
		return guessHintArgsParser;
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_VALIDATE:
	case COMMAND_TYPE_UNDO:
	case COMMAND_TYPE_REDO:
	case COMMAND_TYPE_NUM_SOLUTIONS:
	case COMMAND_TYPE_AUTOFILL:
	case COMMAND_TYPE_RESET:
	case COMMAND_TYPE_EXIT:
	case COMMAND_TYPE_IGNORE:
		return NULL;
	}
	return NULL;
}

commandArgsRangeChecker getCommandArgsRangeChecker(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_SET:
		return setArgsRangeChecker;
	case COMMAND_TYPE_GENERATE:
		return generateArgsRangeChecker;
	case COMMAND_TYPE_HINT:
		return hintArgsRangeChecker;
	case COMMAND_TYPE_GUESS_HINT:
		return guessHintArgsRangeChecker;
	case COMMAND_TYPE_GUESS:
		return guessArgsRangeChecker;
	case COMMAND_TYPE_SOLVE:
	case COMMAND_TYPE_EDIT:
	case COMMAND_TYPE_MARK_ERRORS:
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_VALIDATE:
	case COMMAND_TYPE_UNDO:
	case COMMAND_TYPE_REDO:
	case COMMAND_TYPE_SAVE:
	case COMMAND_TYPE_NUM_SOLUTIONS:
	case COMMAND_TYPE_AUTOFILL:
	case COMMAND_TYPE_RESET:
	case COMMAND_TYPE_EXIT:
	case COMMAND_TYPE_IGNORE:
		return NULL;
	}
	return NULL;
}

commandArgsGetExpectedRangeStringFunc getGetCommandArgsExpectedRangeStringFunc(CommandType commandType) {
	switch (commandType) {
		case COMMAND_TYPE_SET:
			return setArgsGetExpectedRangeString;
		case COMMAND_TYPE_GENERATE:
			return generateArgsGetExpectedRangeString;
		case COMMAND_TYPE_HINT:
			return hintArgsGetExpectedRangeString;
		case COMMAND_TYPE_GUESS_HINT:
			return guessHintArgsGetExpectedRangeString;
		case COMMAND_TYPE_GUESS:
			return guessArgsGetExpectedRangeString;
		case COMMAND_TYPE_SOLVE:
		case COMMAND_TYPE_EDIT:
		case COMMAND_TYPE_MARK_ERRORS:
		case COMMAND_TYPE_PRINT_BOARD:
		case COMMAND_TYPE_VALIDATE:
		case COMMAND_TYPE_UNDO:
		case COMMAND_TYPE_REDO:
		case COMMAND_TYPE_SAVE:
		case COMMAND_TYPE_NUM_SOLUTIONS:
		case COMMAND_TYPE_AUTOFILL:
		case COMMAND_TYPE_RESET:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE:
			return NULL;
	}
	return NULL;
}

char* getCommandArgsExpectedRangeString(CommandType commandType, int argNo, GameState* gameState) {
	commandArgsGetExpectedRangeStringFunc func = getGetCommandArgsExpectedRangeStringFunc(commandType);
	return func(argNo, gameState);
}

commandArgsValidator getCommandArgsValidator(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_GENERATE:
		return generateArgsValidator;
	case COMMAND_TYPE_SET:
	case COMMAND_TYPE_SOLVE:
	case COMMAND_TYPE_EDIT:
	case COMMAND_TYPE_MARK_ERRORS:
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_VALIDATE:
	case COMMAND_TYPE_GUESS:
	case COMMAND_TYPE_UNDO:
	case COMMAND_TYPE_REDO:
	case COMMAND_TYPE_SAVE:
	case COMMAND_TYPE_HINT:
	case COMMAND_TYPE_GUESS_HINT:
	case COMMAND_TYPE_NUM_SOLUTIONS:
	case COMMAND_TYPE_AUTOFILL:
	case COMMAND_TYPE_RESET:
	case COMMAND_TYPE_EXIT:
	case COMMAND_TYPE_IGNORE:
		return NULL;
	}
	return NULL;
}

getCommandArgsValidatorErrorStringFunc getGetCommandArgsValidatorErrorString(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_GENERATE:
		return getGenerateArgsValidatorErrorString;
	case COMMAND_TYPE_SET:
	case COMMAND_TYPE_SOLVE:
	case COMMAND_TYPE_EDIT:
	case COMMAND_TYPE_MARK_ERRORS:
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_VALIDATE:
	case COMMAND_TYPE_GUESS:
	case COMMAND_TYPE_UNDO:
	case COMMAND_TYPE_REDO:
	case COMMAND_TYPE_SAVE:
	case COMMAND_TYPE_HINT:
	case COMMAND_TYPE_GUESS_HINT:
	case COMMAND_TYPE_NUM_SOLUTIONS:
	case COMMAND_TYPE_AUTOFILL:
	case COMMAND_TYPE_RESET:
	case COMMAND_TYPE_EXIT:
	case COMMAND_TYPE_IGNORE:
		return NULL;
	}
	return NULL;
}

IsBoardValidForCommandErrorCode isBoardValidForSetCommand(State* state, Command* command) {
	SetCommandArguments* args = (SetCommandArguments*)command->arguments;

	if (state->gameMode == GAME_MODE_SOLVE) {
		if (isCellFixed(state->gameState, args->row, args->col)) {
			return IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_FIXED_VALUE;
		}
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForValidateCommand(State* state, Command* command) {
	ValidateCommandArguments* args = (ValidateCommandArguments*)command->arguments;

	UNUSED(args);

	if (isBoardErroneous(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForGuessCommand(State* state, Command* command) {
	GuessCommandArguments* args = (GuessCommandArguments*)command->arguments;

	UNUSED(args);

	if (isBoardErroneous(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForGenerateCommand(State* state, Command* command) {
	GenerateCommandArguments* args = (GenerateCommandArguments*)command->arguments;

	UNUSED(args);

	if (isBoardErroneous(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForSaveCommand(State* state, Command* command) {
	SaveCommandArguments* args = (SaveCommandArguments*)command->arguments;

	UNUSED(args);

	if (state->gameMode == GAME_MODE_EDIT) {
		if (isBoardErroneous(state->gameState)) {
			return IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS;
		}
		if (isPuzzleSolvable(state->gameState) == IS_PUZZLE_SOLVABLE_BOARD_UNSOLVABLE)
			return IS_BOARD_VALID_FOR_COMMAND_BOARD_UNSOLVABLE;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForHintCommand(State* state, Command* command) {
	HintCommandArguments* args = (HintCommandArguments*)command->arguments;

	if (isBoardErroneous(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS;
	}
	if (isCellFixed(state->gameState, args->row, args->col)) {
		return IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_FIXED_VALUE;
	}
	if (!isCellEmpty(state->gameState, args->row, args->col)) {
		return IS_BOARD_VALID_FOR_COMMAND_CELL_IS_NOT_EMPTY;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForGuessHintCommand(State* state, Command* command) {
	GuessHintCommandArguments* args = (GuessHintCommandArguments*)command->arguments;

	if (isBoardErroneous(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS;
	}
	if (isCellFixed(state->gameState, args->row, args->col)) {
		return IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_FIXED_VALUE;
	}
	if (!isCellEmpty(state->gameState, args->row, args->col)) {
		return IS_BOARD_VALID_FOR_COMMAND_CELL_IS_NOT_EMPTY;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForNumSolutionsCommand(State* state, Command* command) {
	NumSolutionsCommandArguments* args = (NumSolutionsCommandArguments*)command->arguments;

	UNUSED(args);

	if (isBoardErroneous(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForAutofillCommand(State* state, Command* command) {
	AutofillCommandArguments* args = (AutofillCommandArguments*)command->arguments;

	UNUSED(args);

	if (isBoardErroneous(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForCommand(State* state, Command* command) {
	switch (command->type) {
		case COMMAND_TYPE_SET:
			return isBoardValidForSetCommand(state, command);
		case COMMAND_TYPE_VALIDATE:
			return isBoardValidForValidateCommand(state, command);
		case COMMAND_TYPE_GUESS:
			return isBoardValidForGuessCommand(state, command);
		case COMMAND_TYPE_GENERATE:
			return isBoardValidForGenerateCommand(state, command);
		case COMMAND_TYPE_SAVE:
			return isBoardValidForSaveCommand(state, command);
		case COMMAND_TYPE_HINT:
			return isBoardValidForHintCommand(state, command);
		case COMMAND_TYPE_GUESS_HINT:
			return isBoardValidForGuessHintCommand(state, command);
		case COMMAND_TYPE_NUM_SOLUTIONS:
			return isBoardValidForNumSolutionsCommand(state, command);
		case COMMAND_TYPE_AUTOFILL:
			return isBoardValidForAutofillCommand(state, command);
		case COMMAND_TYPE_SOLVE:
		case COMMAND_TYPE_EDIT:
		case COMMAND_TYPE_MARK_ERRORS:
		case COMMAND_TYPE_PRINT_BOARD:
		case COMMAND_TYPE_UNDO:
		case COMMAND_TYPE_REDO:
		case COMMAND_TYPE_RESET:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE:
			return ERROR_SUCCESS;
		}
	return ERROR_SUCCESS;
}

#define BOARD_INVALID_FOR_COMMAND_FORMAT_STR ("the board is not valid to perform desired command (%s)\n")
#define BOARD_INVALID_FOR_COMMAND_BOARD_ERRONEOUS ("it is erroneous")
#define BOARD_INVALID_FOR_COMMAND_BOARD_UNSOLVABLE ("it is unsolvable")
#define BOARD_INVALID_FOR_COMMAND_CELL_ERRONEOUS ("cell has erroneous value")
#define BOARD_INVALID_FOR_COMMAND_CELL_FIXED ("cell has a fixed value")
#define BOARD_INVALID_FOR_COMMAND_CELL_NOT_EMPTY ("cell is not empty")

char* getIsBoardValidForCommandErrorString(IsBoardValidForCommandErrorCode errorCode) {
	char* str = NULL;
	size_t requiredSize = 0;

	switch (errorCode) {
	case IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS:
		requiredSize = sizeof(BOARD_INVALID_FOR_COMMAND_FORMAT_STR) + sizeof(BOARD_INVALID_FOR_COMMAND_BOARD_ERRONEOUS);
		str = calloc(requiredSize, sizeof(char));
		if (str != NULL)
			sprintf(str, BOARD_INVALID_FOR_COMMAND_FORMAT_STR, BOARD_INVALID_FOR_COMMAND_BOARD_ERRONEOUS);
		break;
	case IS_BOARD_VALID_FOR_COMMAND_BOARD_UNSOLVABLE:
		requiredSize = sizeof(BOARD_INVALID_FOR_COMMAND_FORMAT_STR) + sizeof(BOARD_INVALID_FOR_COMMAND_BOARD_UNSOLVABLE);
		str = calloc(requiredSize, sizeof(char));
		if (str != NULL)
			sprintf(str, BOARD_INVALID_FOR_COMMAND_FORMAT_STR, BOARD_INVALID_FOR_COMMAND_BOARD_UNSOLVABLE);
		break;
	case IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_FIXED_VALUE:
		requiredSize = sizeof(BOARD_INVALID_FOR_COMMAND_FORMAT_STR) + sizeof(BOARD_INVALID_FOR_COMMAND_CELL_FIXED);
		str = calloc(requiredSize, sizeof(char));
		if (str != NULL)
			sprintf(str, BOARD_INVALID_FOR_COMMAND_FORMAT_STR, BOARD_INVALID_FOR_COMMAND_CELL_FIXED);
		break;
	case IS_BOARD_VALID_FOR_COMMAND_CELL_IS_NOT_EMPTY:
		requiredSize = sizeof(BOARD_INVALID_FOR_COMMAND_FORMAT_STR) + sizeof(BOARD_INVALID_FOR_COMMAND_CELL_NOT_EMPTY);
		str = calloc(requiredSize, sizeof(char));
		if (str != NULL)
			sprintf(str, BOARD_INVALID_FOR_COMMAND_FORMAT_STR, BOARD_INVALID_FOR_COMMAND_CELL_NOT_EMPTY);
		break;
	}

	return str;
}

typedef enum {
	PERFORM_EDIT_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_EDIT_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE
} PerformEditCommandErrorCode;

#define FILES_COMMANDS_ERROR_FILE_COULD_NOT_BE_OPENED_STR ("file could not be opened\n");

#define FILES_COMMANDS_ERROR_BAD_FORMAT_COULD_NOT_PARSE_DIMENSIONS_FROM_FILE_STR ("could not parse board dimensions\n")
#define FILES_COMMANDS_ERROR_BAD_FORMAT_DIMENSIONS_NOT_POSITIVE_STR ("board dimensions aren't positive numbers\n")
#define FILES_COMMANDS_ERROR_BAD_FORMAT_CELL_NOT_PARSED_STR ("could not parse one of the cells\n")
#define FILES_COMMANDS_ERROR_BAD_FORMAT_TOO_MUCH_CONTENT_STR ("file contains too much content\n")
#define FILES_COMMANDS_ERROR_BAD_FORMAT_CELL_VALUE_NOT_IN_RANGE_STR ("the value of one of the cells is not in the appropriate range\n")

/* TODO: perhaps these can be specific to saveCommand? */
#define FILES_COMMANDS_ERROR_WRITING_DIMENSIONS_ERROR_STR ("could not write board dimensions\n")
#define FILES_COMMANDS_ERROR_WRITING_CELL_ERROR_STR ("could not write a cell\n")

char* getEditCommandErrorString(int error) {
	PerformEditCommandErrorCode errorCode = (PerformEditCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_EDIT_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	default:
	{
		LoadBoardFromFileErrorCode errorCode = (LoadBoardFromFileErrorCode)(error - PERFORM_EDIT_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE);
		switch (errorCode) {
		case LOAD_BOARD_FROM_FILE_FILE_COULD_NOT_BE_OPENED:
			return FILES_COMMANDS_ERROR_FILE_COULD_NOT_BE_OPENED_STR;
		case LOAD_BOARD_FROM_FILE_COULD_NOT_PARSE_BOARD_DIMENSIONS:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_COULD_NOT_PARSE_DIMENSIONS_FROM_FILE_STR;
		case LOAD_BOARD_FROM_FILE_DIMENSION_ARE_NOT_POSITIVE:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_DIMENSIONS_NOT_POSITIVE_STR;
		case LOAD_BOARD_FROM_FILE_BAD_FORMAT_FAILED_TO_READ_A_CELL:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_CELL_NOT_PARSED_STR;
		case LOAD_BOARD_FROM_FILE_BAD_FORMAT_FILE_CONTAINS_TOO_MUCH_CONTENT:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_TOO_MUCH_CONTENT_STR;
		case LOAD_BOARD_FROM_FILE_CELL_VALUE_NOT_IN_RANGE:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_CELL_VALUE_NOT_IN_RANGE_STR;
		case LOAD_BOARD_FROM_FILE_MEMORY_ALLOCATION_FAILURE:
			return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
		}
	}
	}

	return NULL;
}

bool isEditCommandErrorRecoverable(int error) {
	PerformEditCommandErrorCode errorCode = (PerformEditCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_EDIT_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
	{
		LoadBoardFromFileErrorCode errorCode = (LoadBoardFromFileErrorCode)(error - PERFORM_EDIT_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE);
		switch (errorCode) {
		case LOAD_BOARD_FROM_FILE_MEMORY_ALLOCATION_FAILURE:
			return false;
		default:
			return true;
		}
	}
	}
}

PerformEditCommandErrorCode performEditCommand(State* state, Command* command) {
	EditCommandArguments* editArguments = (EditCommandArguments*)(command->arguments);
	Board board = {0};
	GameState* newGameState = NULL;

	if (command->argumentsNum == 0) { /* Start editing an empty 9x9 board */
		board.numRowsInBlock_M = DEFAULT_M;
		board.numColumnsInBlock_N = DEFAULT_N;
	} else { 						  /* Open from file */
		PerformEditCommandErrorCode retVal = ERROR_SUCCESS;
		retVal = (PerformEditCommandErrorCode)loadBoardFromFile(editArguments->filePath, &board);
		if (retVal != ERROR_SUCCESS)
			return PERFORM_EDIT_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE + retVal;
	}

	newGameState = createGameState(&board, GAME_MODE_EDIT);
	if (newGameState == NULL) {
		cleanupBoard(&board);
		return PERFORM_EDIT_COMMAND_MEMORY_ALLOCATION_FAILURE;
	 }
	cleanupBoard(&board);

	cleanupGameState(state->gameState); state->gameState = NULL;

	state->gameState = newGameState;
	state->gameMode = GAME_MODE_EDIT;
	return ERROR_SUCCESS;
}

char* getEditCommandStrOutput(Command* command, GameState* gameState) {
	EditCommandArguments* editArguments = (EditCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(editArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

typedef enum {
	PERFORM_SOLVE_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_SOLVE_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE
} PerformSolveCommandErrorCode;

char* getSolveCommandErrorString(int error) {
	PerformSolveCommandErrorCode errorCode = (PerformSolveCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_SOLVE_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	default:
	{
		LoadBoardFromFileErrorCode errorCode = (LoadBoardFromFileErrorCode)(error - PERFORM_SOLVE_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE);
		switch (errorCode) {
		case LOAD_BOARD_FROM_FILE_FILE_COULD_NOT_BE_OPENED:
			return FILES_COMMANDS_ERROR_FILE_COULD_NOT_BE_OPENED_STR;
		case LOAD_BOARD_FROM_FILE_COULD_NOT_PARSE_BOARD_DIMENSIONS:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_COULD_NOT_PARSE_DIMENSIONS_FROM_FILE_STR;
		case LOAD_BOARD_FROM_FILE_DIMENSION_ARE_NOT_POSITIVE:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_DIMENSIONS_NOT_POSITIVE_STR;
		case LOAD_BOARD_FROM_FILE_BAD_FORMAT_FAILED_TO_READ_A_CELL:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_CELL_NOT_PARSED_STR;
		case LOAD_BOARD_FROM_FILE_BAD_FORMAT_FILE_CONTAINS_TOO_MUCH_CONTENT:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_TOO_MUCH_CONTENT_STR;
		case LOAD_BOARD_FROM_FILE_CELL_VALUE_NOT_IN_RANGE:
			return FILES_COMMANDS_ERROR_BAD_FORMAT_CELL_VALUE_NOT_IN_RANGE_STR;
		case LOAD_BOARD_FROM_FILE_MEMORY_ALLOCATION_FAILURE:
			return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
		}
	}
	}

	return NULL;
}

bool isSolveCommandErrorRecoverable(int error) {
	PerformSolveCommandErrorCode errorCode = (PerformSolveCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_SOLVE_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
	{
		LoadBoardFromFileErrorCode errorCode = (LoadBoardFromFileErrorCode)(error - PERFORM_SOLVE_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE);
		switch (errorCode) {
		case LOAD_BOARD_FROM_FILE_MEMORY_ALLOCATION_FAILURE:
			return false;
		default:
			return true;
		}
	}
	}
}

PerformSolveCommandErrorCode performSolveCommand(State* state, Command* command) { /* TODO: perhaps wrap solve and edit with some generic LOAD command */
	SolveCommandArguments* solveArguments = (SolveCommandArguments*)(command->arguments);
	GameState* newGameState = NULL;

	Board board = {0};
	PerformSolveCommandErrorCode retVal = ERROR_SUCCESS;
	retVal = (PerformSolveCommandErrorCode)loadBoardFromFile(solveArguments->filePath, &board); /* Note: any superficially legitimate board may be
		 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   loaded for solving, even one that's inherently
		 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   unsolvable (i.e.: two fixed cells that are
		 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   neighbours and share the same value). */
																	  /* TODO: move this note to function's documentation in due time */
	if (retVal != ERROR_SUCCESS)
		return PERFORM_SOLVE_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE + retVal;

	newGameState = createGameState(&board, GAME_MODE_SOLVE);
	if (newGameState == NULL) {
		cleanupBoard(&board);
		return PERFORM_SOLVE_COMMAND_MEMORY_ALLOCATION_FAILURE;
	}
	cleanupBoard(&board);

	cleanupGameState(state->gameState); state->gameState = NULL;

	state->gameState = newGameState;
	state->gameMode = GAME_MODE_SOLVE;
	return ERROR_SUCCESS;
}

char* getSolveCommandStrOutput(Command* command, GameState* gameState) {
	SolveCommandArguments* solveArguments = (SolveCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(solveArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

typedef enum {
	PERFORM_SAVE_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_SAVE_COMMAND_ERROR_IN_SAVE_BOARD_TO_FILE
} PerformSaveCommandErrorCode;

char* getSaveCommandErrorString(int error) {
	PerformSaveCommandErrorCode errorCode = (PerformSaveCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_SAVE_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	default:
	{
		SaveBoardToFileErrorCode errorCode = (SaveBoardToFileErrorCode)(error - PERFORM_SAVE_COMMAND_ERROR_IN_SAVE_BOARD_TO_FILE);
		switch (errorCode) {
		case SAVE_BOARD_TO_FILE_FILE_COULD_NOT_BE_OPENED:
			return FILES_COMMANDS_ERROR_FILE_COULD_NOT_BE_OPENED_STR;
		case SAVE_BOARD_TO_FILE_DIMENSIONS_COULD_NOT_BE_WRITTEN:
			return FILES_COMMANDS_ERROR_WRITING_DIMENSIONS_ERROR_STR;
		case SAVE_BOARD_TO_FILE_FAILED_TO_WRITE_A_CELL:
			return FILES_COMMANDS_ERROR_WRITING_CELL_ERROR_STR;
		}
	}
	}

	return NULL;
}

bool isSaveCommandErrorRecoverable(int error) {
	PerformSaveCommandErrorCode errorCode = (PerformSaveCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_SAVE_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

PerformSaveCommandErrorCode performSaveCommand(State* state, Command* command) {
	SaveCommandArguments* saveArguments = (SaveCommandArguments*)(command->arguments);

	PerformSaveCommandErrorCode retVal = ERROR_SUCCESS;

	Board exportedBoard = {0};


	if (!exportBoard(state->gameState, &exportedBoard)) {
		return PERFORM_SAVE_COMMAND_MEMORY_ALLOCATION_FAILURE;
	}

	if (state->gameMode == GAME_MODE_EDIT) {
		markFilledCellsAsFixed(&exportedBoard);
	}

	retVal = (PerformSaveCommandErrorCode)saveBoardToFile(saveArguments->filePath, &exportedBoard);
	cleanupBoard(&exportedBoard);
	if (retVal != ERROR_SUCCESS)
		return PERFORM_SAVE_COMMAND_ERROR_IN_SAVE_BOARD_TO_FILE + retVal;

	return ERROR_SUCCESS;
}

char* getSaveCommandStrOutput(Command* command, GameState* gameState) {
	SaveCommandArguments* saveArguments = (SaveCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(saveArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

typedef enum {
	PERFORM_MARK_ERRORS_COMMAND_NO_CHANGE = 1
} PerformMarkErrorsCommandErrorCode;

#define MARK_ERRORS_ERROR_NO_CHANGE_STR ("no change has occurred\n")

char* getMarkErrorsCommandErrorString(int error) {
	PerformMarkErrorsCommandErrorCode errorCode = (PerformMarkErrorsCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_MARK_ERRORS_COMMAND_NO_CHANGE:
		return MARK_ERRORS_ERROR_NO_CHANGE_STR;
	}

	return NULL;
}

bool isMarkErrorsCommandErrorRecoverable(int error) {
	PerformMarkErrorsCommandErrorCode errorCode = (PerformMarkErrorsCommandErrorCode)error;

	switch (errorCode) {
	default:
		return true;
	}
}

PerformMarkErrorsCommandErrorCode performMarkErrorsCommand(State* state, Command* command) {
	MarkErrorsCommandArguments* markErrorsArguments = (MarkErrorsCommandArguments*)(command->arguments);

	if (shouldMarkErrors(state) == markErrorsArguments->shouldMarkError)
		return PERFORM_MARK_ERRORS_COMMAND_NO_CHANGE;

	setMarkErrors(state, markErrorsArguments->shouldMarkError);

	return ERROR_SUCCESS;
}

char* getMarkErrorsCommandStrOutput(Command* command, GameState* gameState) {
	MarkErrorsCommandArguments* markErrorsArguments = (MarkErrorsCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(markErrorsArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

typedef enum {
	PERFORM_VALIDATE_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_VALIDATE_COMMAND_FAILED_IN_VALIDATING
} PerformValidateCommandErrorCode;

#define VALIDATE_COMMAND_ERROR_FAILED_IN_VALIDATING ("failed in validating board\n")

char* getValidateCommandErrorString(int error) {
	PerformValidateCommandErrorCode errorCode = (PerformValidateCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_VALIDATE_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	case PERFORM_VALIDATE_COMMAND_FAILED_IN_VALIDATING:
		return VALIDATE_COMMAND_ERROR_FAILED_IN_VALIDATING;
	}

	return NULL;
}

bool isValidateCommandErrorRecoverable(int error) {
	PerformValidateCommandErrorCode errorCode = (PerformValidateCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_VALIDATE_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

PerformValidateCommandErrorCode performValidateCommand(State* state, Command* command) {
	ValidateCommandArguments* validateArguments = (ValidateCommandArguments*)(command->arguments);

	PerformValidateCommandErrorCode retVal = ERROR_SUCCESS;

	switch (isPuzzleSolvable(state->gameState)) {
	case IS_PUZZLE_SOLVABLE_BOARD_SOLVABLE:
		validateArguments->isSolvableOut = true;
		break;
	case IS_PUZZLE_SOLVABLE_MEMORY_ALLOCATION_FAILURE:
		retVal = PERFORM_VALIDATE_COMMAND_MEMORY_ALLOCATION_FAILURE;
		break;
	case IS_PUZZLE_SOLVABLE_BOARD_UNSOLVABLE:
		validateArguments->isSolvableOut = false;
		break;
	case IS_PUZZLE_SOLVABLE_FAILED_VALIDATING:
		retVal = PERFORM_VALIDATE_COMMAND_FAILED_IN_VALIDATING;
		break;
	}

	return retVal;
}

#define VALIDATE_COMMAND_OUTPUT_BOARD_SOLVABLE ("Board is solvable\n")
#define VALIDATE_COMMAND_OUTPUT_BOARD_NOT_SOLVABLE ("Board is not solvable\n")

char* getValidateCommandStrOutput(Command* command, GameState* gameState) {
	ValidateCommandArguments* validateArguments = (ValidateCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* strToCopy = NULL;

	UNUSED(gameState);

	if (validateArguments->isSolvableOut) {
		strToCopy = VALIDATE_COMMAND_OUTPUT_BOARD_SOLVABLE;
	} else {
		strToCopy = VALIDATE_COMMAND_OUTPUT_BOARD_NOT_SOLVABLE;
	}

	numCharsRequired = strlen(strToCopy) + 1;
	str = calloc(numCharsRequired, sizeof(char));
	if (str != NULL) {
		strcpy(str, strToCopy);
	}

	return str;
}

typedef enum {
	PERFORM_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_HINT_COMMAND_BOARD_UNSOLVABLE,
	PERFORM_HINT_COMMAND_COULD_NOT_SOLVE_BOARD
} PerformHintCommandErrorCode;

#define BOARD_SOLVING_COMMANDS_ERROR_BOARD_UNSOLVABLE_STR ("board is unsolvable\n")
#define BOARD_SOLVING_COMMANDS_ERROR_COULD_NOT_SOLVE_BOARD_STR ("failed to solve board\n")

char* getHintCommandErrorString(int error) {
	PerformHintCommandErrorCode errorCode = (PerformHintCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	case PERFORM_HINT_COMMAND_BOARD_UNSOLVABLE:
		return BOARD_SOLVING_COMMANDS_ERROR_BOARD_UNSOLVABLE_STR;
	case PERFORM_HINT_COMMAND_COULD_NOT_SOLVE_BOARD:
		return BOARD_SOLVING_COMMANDS_ERROR_COULD_NOT_SOLVE_BOARD_STR;
	}

	return NULL;
}

bool isHintCommandErrorRecoverable(int error) {
	PerformHintCommandErrorCode errorCode = (PerformHintCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

PerformHintCommandErrorCode performHintCommand(State* state, Command* command) {
	HintCommandArguments* hintArguments = (HintCommandArguments*)(command->arguments);

	PerformHintCommandErrorCode retVal = ERROR_SUCCESS;

	Board boardSolution = {0};

	switch (getPuzzleSolution(state->gameState, &boardSolution)) {
	case GET_PUZZLE_SOLUTION_SUCCESS:
		hintArguments->guessedValueOut = getBoardCellValue(getBoardCellByRow(&boardSolution, hintArguments->row, hintArguments->col));
		break;
	case GET_PUZZLE_SOLUTION_BOARD_UNSOLVABLE:
		retVal = PERFORM_HINT_COMMAND_BOARD_UNSOLVABLE;
		break;
	case GET_PUZZLE_SOLUTION_COULD_NOT_SOLVE_BOARD:
		retVal = PERFORM_HINT_COMMAND_COULD_NOT_SOLVE_BOARD;
		break;
	case GET_PUZZLE_SOLUTION_MEMORY_ALLOCATION_FAILURE:
		retVal = PERFORM_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE;
		break;
	}

	cleanupBoard(&boardSolution);

	return retVal;
}

#define HINT_COMMAND_OUTPUT_FORMAT ("value: %2d\n")

char* getHintCommandStrOutput(Command* command, GameState* gameState) {
	HintCommandArguments* hintArguments = (HintCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;

	UNUSED(gameState);

	numCharsRequired = sizeof(HINT_COMMAND_OUTPUT_FORMAT);

	str = calloc(numCharsRequired, sizeof(char));
	if (str != NULL) {
		sprintf(str, HINT_COMMAND_OUTPUT_FORMAT, hintArguments->guessedValueOut);
	}

	return str;
}

typedef enum {
	RANDOMLY_FILL_EMPTY_SUCCESS,
	RANDOMLY_FILL_EMPTY_CELL_MEMORY_ALLOCATION_FAILURE,
	RANDOMLY_FILL_EMPTY_CELL_NO_LEGAL_VALUE
} randomlyFillEmptyCellErrorCode;
randomlyFillEmptyCellErrorCode randomlyFillEmptyCell(Board* boardInOut, int row, int col) {
	randomlyFillEmptyCellErrorCode retVal = RANDOMLY_FILL_EMPTY_SUCCESS;
	CellLegalValues cellLegalValues;

	if (!fillBoardCellLegalValuesStruct(boardInOut, row, col, &cellLegalValues)) {
		retVal = RANDOMLY_FILL_EMPTY_CELL_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	if (cellLegalValues.numLegalValues == 0) {
		retVal = RANDOMLY_FILL_EMPTY_CELL_NO_LEGAL_VALUE;
	} else {
		while (true) {
			int MN = getBoardBlockSize_MN(boardInOut);
			int value = (rand() % MN) + 1;
			if (cellLegalValues.legalValues[value]) {
				setBoardCellValue(boardInOut, row, col, value);
				break;
			}
		}
	}

	cleanupCellLegalValuesStruct(&cellLegalValues);
	return retVal;
}

void findEmptyCellInBoard(Board* board, int* rowOut, int* colOut) { /* Note: we assume the board has at least one empty cell */
	int MN = getBoardBlockSize_MN(board);
	int row = 0, col = 0;
	while (true) {
		col = rand() % MN;
		row = rand() % MN;
		if (isBoardCellEmpty(getBoardCellByRow(board, row, col))) {
			*rowOut = row;
			*colOut = col;
			return;
		}
	}
}

typedef enum {
	RANDOMLY_FILL_X_EMPTY_CELLS_SUCCESS,
	RANDOMLY_FILL_X_EMPTY_CELLS_MEMORY_ALLOCATION_FAILURE,
	RANDOMLY_FILL_X_EMPTY_CELLS_CHOSEN_CELL_HAS_NO_LEGAL_VALUE
} randomlyFillXEmptyCellsErrorCode;
randomlyFillXEmptyCellsErrorCode randomlyFillXEmptyCells(Board* boardInOut, int numEmptyCellsToFill) { /* Note: we assume the board has at least numEmptyCellsToFill empty cells */
	int numEmptyCellsFilled = 0;
	for (numEmptyCellsFilled = 0; numEmptyCellsFilled < numEmptyCellsToFill; numEmptyCellsFilled++) {
		int row = 0, col = 0;

		findEmptyCellInBoard(boardInOut, &row, &col);

		switch (randomlyFillEmptyCell(boardInOut, row, col)) {
		case RANDOMLY_FILL_EMPTY_SUCCESS:
			continue;
		case RANDOMLY_FILL_EMPTY_CELL_MEMORY_ALLOCATION_FAILURE:
			return RANDOMLY_FILL_X_EMPTY_CELLS_MEMORY_ALLOCATION_FAILURE;
		case RANDOMLY_FILL_EMPTY_CELL_NO_LEGAL_VALUE:
			return RANDOMLY_FILL_X_EMPTY_CELLS_CHOSEN_CELL_HAS_NO_LEGAL_VALUE;
		}
	}
	return RANDOMLY_FILL_X_EMPTY_CELLS_SUCCESS;
}

void findNonEmptyCellInBoard(Board* board, int* rowOut, int* colOut) { /* Note: we assume the board has at least one non-empty cell */
	int MN = getBoardBlockSize_MN(board);
	int row = 0, col = 0;
	while (true) {
		col = rand() % MN;
		row = rand() % MN;
		if (!isBoardCellEmpty(getBoardCellByRow(board, row, col))) {
			*rowOut = row;
			*colOut = col;
			return;
		}
	}
}

void randomlyClearYCells(Board* board, int numCellsToClear) {
	int numCellsCleared = 0;
	for (numCellsCleared = 0; numCellsCleared < numCellsToClear; numCellsCleared++) {
		Cell* cell = NULL;

		int row = 0, col = 0;
		findNonEmptyCellInBoard(board, &row, &col);

		cell = getBoardCellByRow(board, row, col);
		emptyBoardCell(cell);
	}
}

typedef enum {
	PERFORM_GENERATE_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_GENERATE_COMMAND_COULD_NOT_GENERATE_REQUESTED_BOARD
} PerformGenerateCommandErrorCode;

#define GENERATE_COMMAND_ERROR_FAILED_TO_GENERATE_BOARD_STR ("failed to generate requested board\n")

char* getGenerateCommandErrorString(int error) {
	PerformGenerateCommandErrorCode errorCode = (PerformGenerateCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_GENERATE_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	case PERFORM_GENERATE_COMMAND_COULD_NOT_GENERATE_REQUESTED_BOARD:
		return GENERATE_COMMAND_ERROR_FAILED_TO_GENERATE_BOARD_STR;
	}

	return NULL;
}

bool isGenerateCommandErrorRecoverable(int error) {
	PerformGenerateCommandErrorCode errorCode = (PerformGenerateCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_GENERATE_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

PerformGenerateCommandErrorCode performGenerateCommand(State* state, Command* command) {
	GenerateCommandArguments* generateArguments = (GenerateCommandArguments*)(command->arguments);

	PerformGenerateCommandErrorCode retVal = ERROR_SUCCESS;
	bool severeErrorOccurred = false;
	bool succeeded = false;

	int numTries = 0;
	Board board = {0};
	Board boardSolution = {0};

	UNUSED(retVal);

	for (numTries = 0; (numTries < GENERATE_COMMAND_MAX_NUM_TRIES) && (!severeErrorOccurred) && (!succeeded); numTries++) {
		cleanupBoard(&board);
		cleanupBoard(&boardSolution);

		if (!exportBoard(state->gameState, &board)) {
			retVal = PERFORM_GENERATE_COMMAND_MEMORY_ALLOCATION_FAILURE;
			break;
		}

		switch (randomlyFillXEmptyCells(&board, generateArguments->numEmptyCellsToFill)) {
			case RANDOMLY_FILL_X_EMPTY_CELLS_SUCCESS:
				break;
			case RANDOMLY_FILL_X_EMPTY_CELLS_MEMORY_ALLOCATION_FAILURE:
				severeErrorOccurred = true;
				retVal = PERFORM_GENERATE_COMMAND_MEMORY_ALLOCATION_FAILURE;
				continue;
			case RANDOMLY_FILL_X_EMPTY_CELLS_CHOSEN_CELL_HAS_NO_LEGAL_VALUE:
				continue;
		}

		switch (getBoardSolution(&board, &boardSolution)) {
		case GET_BOARD_SOLUTION_SUCCESS:
			randomlyClearYCells(&boardSolution, generateArguments->numCellsToClear);

			if (makeMultiCellMove(state, &boardSolution)) { /* CR: this should have been here boardSolution, as I wrote in the original TODO that was here. Are you sure you have checked the code you've written? */
				retVal = ERROR_SUCCESS;
				succeeded = true;
			} else {
				retVal = PERFORM_GENERATE_COMMAND_MEMORY_ALLOCATION_FAILURE;
				severeErrorOccurred = true;
			}

			break;
		case GET_BOARD_SOLUTION_BOARD_UNSOLVABLE:
		case GET_BOARD_SOLUTION_COULD_NOT_SOLVE_BOARD:
			continue;
		case GET_BOARD_SOLUTION_MEMORY_ALLOCATION_FAILURE:
			severeErrorOccurred = true;
			continue;
		}
	}
	cleanupBoard(&board);
	cleanupBoard(&boardSolution);

	if (!succeeded)
		retVal = PERFORM_GENERATE_COMMAND_COULD_NOT_GENERATE_REQUESTED_BOARD;

	return retVal;
}

char* getGenerateCommandStrOutput(Command* command, GameState* gameState) {
	GenerateCommandArguments* generateArguments = (GenerateCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(generateArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

typedef enum {
	PERFORM_GUESS_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_GUESS_HINT_COMMAND_BOARD_NOT_SOLVABLE,
	PERFORM_GUESS_HINT_COMMAND_COULD_NOT_SOLVE_BOARD
} PerformGuessHintCommandErrorCode;

char* getGuessHintCommandErrorString(int error) {
	PerformGuessHintCommandErrorCode errorCode = (PerformGuessHintCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_GUESS_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	case PERFORM_GUESS_HINT_COMMAND_BOARD_NOT_SOLVABLE:
		return BOARD_SOLVING_COMMANDS_ERROR_BOARD_UNSOLVABLE_STR;
	case PERFORM_GUESS_HINT_COMMAND_COULD_NOT_SOLVE_BOARD:
		return BOARD_SOLVING_COMMANDS_ERROR_COULD_NOT_SOLVE_BOARD_STR;
	}

	return NULL;
}

bool isGuessHintCommandErrorRecoverable(int error) {
	PerformGuessHintCommandErrorCode errorCode = (PerformGuessHintCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_GUESS_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

PerformGuessHintCommandErrorCode performGuessHintCommand(State* state, Command* command) {
	GuessHintCommandArguments* guessHintArguments = (GuessHintCommandArguments*)(command->arguments);

	PerformGuessHintCommandErrorCode retVal = ERROR_SUCCESS;

	bool isBoardSolved = false;

	Board board = {0};
	double*** valuesScores = NULL;
	int MN = 0;

	if (!exportBoard(state->gameState, &board)) {
		retVal = PERFORM_GUESS_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	MN = getBoardBlockSize_MN(&board);

	switch (guessValuesForAllPuzzleCells(&board, &valuesScores)) {
	case GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_SOLVED:
		isBoardSolved = true;
		break;
	case GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_MEMORY_ALLOCATION_FAILURE:
		retVal = PERFORM_GUESS_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE;
		break;
	case GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_NOT_SOLVABLE:
		retVal = PERFORM_GUESS_HINT_COMMAND_BOARD_NOT_SOLVABLE;
		break;
	case GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_COULD_NOT_SOLVE_BOARD:
		retVal = PERFORM_GUESS_HINT_COMMAND_COULD_NOT_SOLVE_BOARD;
		break;
	}

	if (isBoardSolved) {
		guessHintArguments->valuesScoresOut = calloc(MN + 1, sizeof(double));
		if (guessHintArguments->valuesScoresOut == NULL)
			retVal = PERFORM_GUESS_HINT_COMMAND_MEMORY_ALLOCATION_FAILURE;
		else {
			int value = 1;
			for (value = 1; value <= MN; value++)
				guessHintArguments->valuesScoresOut[value] = valuesScores[guessHintArguments->row][guessHintArguments->col][value];
		}
	}

	freeValuesScoresArr(valuesScores, &board);
	cleanupBoard(&board);

	return retVal;
}

#define GUESS_HINT_COMMAND_OUTPUT_FORMAT_PER_VALUE ("value: %2d, score: %.4f\n")
#define GUESS_HINT_COMMAND_OUTPUT_NO_VALUE_HAS_POSITIVE_SCORE ("no value achieved positive score\n")

char* getGuessHintCommandStrOutput(Command* command, GameState* gameState) {
	GuessHintCommandArguments* guessHintArguments = (GuessHintCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;

	int actualNumValues = 0;

	int value = 0;
	for (value = 1; value <= getBlockSize_MN(gameState); value++) {
		if (guessHintArguments->valuesScoresOut[value] > 0.0)
			actualNumValues++;
	}

	if (actualNumValues > 0) {
		numCharsRequired = actualNumValues * sizeof(GUESS_HINT_COMMAND_OUTPUT_FORMAT_PER_VALUE); /* Note: upper boundary */
		str = calloc(numCharsRequired, sizeof(char));
		if (str != NULL) {
			int numCharsAlreadyWritten = 0;
			for (value = 1; value <= getBlockSize_MN(gameState); value++) {
				if (guessHintArguments->valuesScoresOut[value] > 0.0)
					numCharsAlreadyWritten += sprintf(str + numCharsAlreadyWritten,
													  GUESS_HINT_COMMAND_OUTPUT_FORMAT_PER_VALUE,
													  value,
													  guessHintArguments->valuesScoresOut[value]);
				}
		}
	} else {
		char* strToCopy = GUESS_HINT_COMMAND_OUTPUT_NO_VALUE_HAS_POSITIVE_SCORE;
		numCharsRequired = strlen(strToCopy) + 1;
		str = calloc(numCharsRequired, sizeof(char));
		if (str != NULL) {
			strcpy(str, strToCopy);
		}
	}

	return str;
}

double getRealRand(double min, double max) {
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

typedef enum {
	PERFORM_GUESS_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_GUESS_COMMAND_BOARD_NOT_SOLVABLE,
	PERFORM_GUESS_COMMAND_COULD_NOT_SOLVE_BOARD
} PerformGuessCommandErrorCode;

char* getGuessCommandErrorString(int error) {
	PerformGuessCommandErrorCode errorCode = (PerformGuessCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_GUESS_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	case PERFORM_GUESS_COMMAND_BOARD_NOT_SOLVABLE:
		return BOARD_SOLVING_COMMANDS_ERROR_BOARD_UNSOLVABLE_STR;
	case PERFORM_GUESS_COMMAND_COULD_NOT_SOLVE_BOARD:
		return BOARD_SOLVING_COMMANDS_ERROR_COULD_NOT_SOLVE_BOARD_STR;
	}

	return NULL;
}

bool isGuessCommandErrorRecoverable(int error) {
	PerformGuessCommandErrorCode errorCode = (PerformGuessCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_GUESS_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

int chooseGuessedValueForCell(Board* board, int row, int col, double* valuesScores, double threshold) { /* Note: array is changed */
	int value = 1;
	double legalValuesScoresSum = 0.0;
	double incrementalNormalisedScore = 0.0;
	double randNum = 0;
	double minimum = 1.0;
	int chosenValue = -1;

	for (value = 1; value <= getBoardBlockSize_MN(board); value++) {
		if ((isValueLegalForBoardCell(board, row, col, value)) &&
			(valuesScores[value] >= threshold)) {
				legalValuesScoresSum += valuesScores[value];
		} else
				valuesScores[value] = 0;
	}

	if (legalValuesScoresSum > 0) {
		/* Normalise relevant scores: */
		for (value = 1; value <= getBoardBlockSize_MN(board); value++) {
			if (valuesScores[value] > 0) {
				incrementalNormalisedScore += valuesScores[value] / legalValuesScoresSum;
				valuesScores[value] = incrementalNormalisedScore;
			}
		}

		/* Randomise one of the legal values: */
		randNum = getRealRand(0.0, 1.0);
		for (value = 1; value <= getBoardBlockSize_MN(board); value++) {
			if (valuesScores[value] >= randNum)
				if (valuesScores[value] <= minimum) {
					minimum = valuesScores[value];
					chosenValue = value;
				}
		}
	}

	return chosenValue;
}

PerformGuessCommandErrorCode performGuessCommand(State* state, Command* command) {
	GuessCommandArguments* guessArguments = (GuessCommandArguments*)(command->arguments);

	PerformGuessCommandErrorCode retVal = ERROR_SUCCESS;

	bool isBoardSolved = false;

	Board board = {0};
	double*** valuesScores = NULL;
	int MN = 0;

	if (!exportBoard(state->gameState, &board)) {
		retVal = PERFORM_GUESS_COMMAND_MEMORY_ALLOCATION_FAILURE;
		return retVal;
	}

	MN = getBoardBlockSize_MN(&board);

	switch (guessValuesForAllPuzzleCells(&board, &valuesScores)) {
	case GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_SOLVED:
		isBoardSolved = true;
		break;
	case GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_MEMORY_ALLOCATION_FAILURE:
		retVal = PERFORM_GUESS_COMMAND_MEMORY_ALLOCATION_FAILURE;
		break;
	case GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_BOARD_NOT_SOLVABLE:
		retVal = PERFORM_GUESS_COMMAND_BOARD_NOT_SOLVABLE;
		break;
	case GUESS_VALUES_FOR_ALL_PUZZLE_CELLS_COULD_NOT_SOLVE_BOARD:
		retVal = PERFORM_GUESS_COMMAND_COULD_NOT_SOLVE_BOARD;
		break;
	}

	if (isBoardSolved) {
		int row = 0;
		for (row = 0; row < MN; row++) {
			int col = 0;
			for (col = 0; col < MN; col++)
				if (isBoardCellEmpty(getBoardCellByRow(&board, row, col))) {
					int chosenValue = chooseGuessedValueForCell(&board, row, col, valuesScores[row][col], guessArguments->threshold);
					if (chosenValue != -1)
						setBoardCellValue(&board, row, col, chosenValue);
				}
		}

		if (!makeMultiCellMove(state, &board)) { /* CR: actually here, in guess, we CAN (as you see) document each change. If you feel like writing this code (hopefully it would be easy) for improved efficiency - by all means, do it */
			retVal = PERFORM_GUESS_COMMAND_MEMORY_ALLOCATION_FAILURE;
		}
	}


	freeValuesScoresArr(valuesScores, &board);
	cleanupBoard(&board);

	return retVal;
}

char* getGuessCommandStrOutput(Command* command, GameState* gameState) {
	GuessCommandArguments* guessArguments = (GuessCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(guessArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

typedef enum {
	PERFORM_SET_COMMAND_MEMORY_ALLOCATION_FAILURE = 1
} PerformSetCommandErrorCode;

char* getSetCommandErrorString(int error) {
	PerformSetCommandErrorCode errorCode = (PerformSetCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_SET_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	}

	return NULL;
}

bool isSetCommandErrorRecoverable(int error) {
	PerformSetCommandErrorCode errorCode = (PerformSetCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_SET_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

/* Assuming that upon call to this functions all conditions have been
checked - in solve mode fixed cells cannot be set etc. This is consistent
with the command loop flow */
PerformSetCommandErrorCode performSetCommand(State* state, Command* command) {
	SetCommandArguments* setArguments = (SetCommandArguments*)(command->arguments);
	if(!makeSingleCellMove(state, setArguments->value, setArguments->row, setArguments->col)){
		return PERFORM_SET_COMMAND_MEMORY_ALLOCATION_FAILURE;
	}
	else {return ERROR_SUCCESS; }
}

char* getSetCommandStrOutput(Command* command, GameState* gameState) {
	SetCommandArguments* setArguments = (SetCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(setArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

#define UNDO_COMMAND_NOTHING_TO_UNDO_FAILURE_STR ("No move to undo\n")

typedef enum {
	PERFORM_UNDO_COMMAND_NOTHING_TO_UNDO = 1
} PerformUndoCommandErrorCode;

char* getUndoCommandErrorString(int error) {
	PerformUndoCommandErrorCode errorCode = (PerformUndoCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_UNDO_COMMAND_NOTHING_TO_UNDO:
		return UNDO_COMMAND_NOTHING_TO_UNDO_FAILURE_STR;
	}
	return NULL;
}

bool isUndoCommandErrorRecoverable(int error) {
	PerformUndoCommandErrorCode errorCode = (PerformUndoCommandErrorCode)error;

	/* all undo errors are recoverable */
	switch (errorCode) {
	default:
		return true;
	}
}

PerformUndoCommandErrorCode performUndoCommand(State* state, Command* command) {
	UndoCommandArguments* undoArguments = (UndoCommandArguments*)(command->arguments);
	undoArguments->movesListOut = undoMove(state);
	if (undoArguments->movesListOut == NULL) {
		return PERFORM_UNDO_COMMAND_NOTHING_TO_UNDO;
	}
	return ERROR_SUCCESS;
}

#define SINGLE_CELL_MOVE_OUTPUT_FORMAT ("[%d,%d] = %d->%d\n") /* CR: not a CR comment!: I would love it if you used a reversed arrow for undo...: [0,3] = 1 <- 2 (undo), [0,3] = 1 -> 2 (redo); also, if you could feel like changing the squared brackets to regular ones, that's be lovely (the squared ones makes me think of ranges, rather than coordinates); last thing, how about having an empty cell be an empty string (for example, (1, 3) =   -> 1). These are all merely suggestions, again :) */

size_t getMoveStrOutputSize(GameState* gameState, Move* move) {
	int MN = getBlockSize_MN(gameState);

	size_t numCharsRequired = 
		1 +	(move->singleCellMoves.size * /* CR: access size through a wrapper function (also singleCellMoves...) */
			(sizeof(SINGLE_CELL_MOVE_OUTPUT_FORMAT) + (4 * getNumDecDigitsInNumber(MN + 1)))); /* CR: 4 should be be a define constant */ /* CR: since we were instucted to use format %2d for each cell's value when outputting a board to the screen, you could use the same here, and refrain from using the getNumDecDigits func */
	
	return numCharsRequired;
}

/* Returns the total number of characters written. 
This count does not include the additional null-character 
automatically appended at the end of the string. */
size_t sprintMoveStrOutput(char* outStr, Move* move, bool undo) { /* CR: if the move is empty (aka an autofill that changed nothing, perhaps have some indicative output?) */
	char* start = outStr;
	Node* curr = getHead(&(move->singleCellMoves)); /* CR: access singleCellMoves through a wrapper function */
	while (curr != NULL) {
		singleCellMove* scMove = (singleCellMove*)curr->data; /* CR: access data through a wrapper function */
		outStr += sprintf(outStr,
						  SINGLE_CELL_MOVE_OUTPUT_FORMAT,
						  scMove->row + 1, /* CR: note the reason for the offset */
						  scMove->col + 1,
						  undo ? scMove->newVal : scMove->prevVal,
						  undo ? scMove->prevVal : scMove->newVal);
		curr = getNext(curr);
	}

	return outStr - start;
}

char* getUndoCommandStrOutput(Command* command, GameState* gameState) {
	UndoCommandArguments* undoArguments = (UndoCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	if (undoArguments->movesListOut != NULL) {
		numCharsRequired = getMoveStrOutputSize(gameState, undoArguments->movesListOut);
		str = calloc(numCharsRequired, sizeof(char)); /* CR: check returned value of calloc */
		sprintMoveStrOutput(str, undoArguments->movesListOut, true);
	} else {
		numCharsRequired = strlen(emptyString) + 1; /* TODO: call here a call to a generic function that returns a dynamically allocated empty string */
		str = calloc(numCharsRequired, sizeof(char));
	}

	return str;
}

#define REDO_COMMAND_NOTHING_TO_REDO_FAILURE_STR ("No move to redo\n")

typedef enum {
	PERFORM_REDO_COMMAND_NOTHING_TO_REDO = 1
} PerformRedoCommandErrorCode;

char* getRedoCommandErrorString(int error) {
	PerformRedoCommandErrorCode errorCode = (PerformRedoCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_REDO_COMMAND_NOTHING_TO_REDO:
		return REDO_COMMAND_NOTHING_TO_REDO_FAILURE_STR;
	}
	return NULL;
}

bool isRedoCommandErrorRecoverable(int error) {
	PerformRedoCommandErrorCode errorCode = (PerformRedoCommandErrorCode)error;

	/* all redo errors are recoverable */
	switch (errorCode) {
	default:
		return true;
	}
}

PerformRedoCommandErrorCode performRedoCommand(State* state, Command* command) {
	RedoCommandArguments* redoArguments = (RedoCommandArguments*)(command->arguments);
	redoArguments->movesListOut = redoMove(state);
	if (redoArguments->movesListOut == NULL) {
		return PERFORM_REDO_COMMAND_NOTHING_TO_REDO;
	}
	return ERROR_SUCCESS;
}

char* getRedoCommandStrOutput(Command* command, GameState* gameState) { /* CR: similar to getUndoCommandStrOutput */
	RedoCommandArguments* redoArguments = (RedoCommandArguments*)(command->arguments);
	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	if (redoArguments->movesListOut != NULL) {
		numCharsRequired = getMoveStrOutputSize(gameState, redoArguments->movesListOut);
		str = calloc(numCharsRequired, sizeof(char));
		sprintMoveStrOutput(str, redoArguments->movesListOut, false);
	} else {
		numCharsRequired = strlen(emptyString) + 1;
		str = calloc(numCharsRequired, sizeof(char));
	}

	return str;
}

typedef enum {
	PERFORM_NUM_SOLUTIONS_COMMAND_MEMORY_ALLOCATION_FAILURE = 1
} PerformNumSoltionsCommandErrorCode;

char* getNumSolutionsCommandErrorString(int error) {
	PerformNumSoltionsCommandErrorCode errorCode = (PerformNumSoltionsCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_NUM_SOLUTIONS_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	}

	return NULL;
}

bool isNumSolutionsCommandErrorRecoverable(int error) {
	PerformNumSoltionsCommandErrorCode errorCode = (PerformNumSoltionsCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_NUM_SOLUTIONS_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

typedef enum {
	PERFORM_AUTO_FILL_COMMAND_MEMORY_ALLOCATION_FAILURE = 1
} PerformAutofillCommandErrorCode;

char* getAutoFillCommandErrorString(int error) {
	PerformAutofillCommandErrorCode errorCode = (PerformAutofillCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_AUTO_FILL_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return COMMAND_ERROR_MEMORY_ALLOCATION_FAILURE_STR;
	}

	return NULL;
}

bool isAutofillCommandErrorRecoverable(int error) {
	PerformAutofillCommandErrorCode errorCode = (PerformAutofillCommandErrorCode)error;

	switch (errorCode) {
	case PERFORM_AUTO_FILL_COMMAND_MEMORY_ALLOCATION_FAILURE:
		return false;
	default:
		return true;
	}
}

typedef enum {
	PERFORM_RESET_COMMAND_NO_CHANGES
} PerformResetCommandErrorCode;

bool isResetCommandErrorRecoverable(int error) {
	PerformResetCommandErrorCode errorCode = (PerformResetCommandErrorCode)error;

	/* all reset errors are recoverable */
	switch (errorCode) {
	default:
		return true;
	}
}

PerformNumSoltionsCommandErrorCode performNumSolutionsCommand(State* state, Command* command) {	
	NumSolutionsCommandArguments* args = (NumSolutionsCommandArguments*) command->arguments;
	int numSolutions = 0;

	if (!calculateNumSolutions(state->gameState, &numSolutions)) {
		return PERFORM_NUM_SOLUTIONS_COMMAND_MEMORY_ALLOCATION_FAILURE;
	}

	args->numSolutionsOut = numSolutions;
	return ERROR_SUCCESS;
}

PerformAutofillCommandErrorCode performAutofillCommand(State* state, Command* command) {
	AutofillCommandArguments* autofillArguments = (AutofillCommandArguments*)(command->arguments);
	Move* move = NULL;

	if (!autofill(state, &move)) {
		return PERFORM_AUTO_FILL_COMMAND_MEMORY_ALLOCATION_FAILURE;
	}

	autofillArguments->movesListOut = move;
	return ERROR_SUCCESS;
}

PerformResetCommandErrorCode performResetCommand(State* state, Command* command) {
	ResetCommandArguments* resetfillArguments = (ResetCommandArguments*)(command->arguments);

	UNUSED(resetfillArguments);

	if (!resetMoves(state)) {
		return PERFORM_RESET_COMMAND_NO_CHANGES;
	}
	return ERROR_SUCCESS;
}

#define NUM_SOLUTIONS_OUTPUT_FORMAT ("Number of solutions: %d\n")

char* getNumSolutionsCommandStrOutput(Command* command, GameState* gameState) {
	NumSolutionsCommandArguments* numSolutionsArguments = (NumSolutionsCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;

	UNUSED(gameState);

	numCharsRequired = sizeof(NUM_SOLUTIONS_OUTPUT_FORMAT) + getNumDecDigitsInNumber(numSolutionsArguments->numSolutionsOut); /* Note: conservative upper boundary */

	str = calloc(numCharsRequired, sizeof(char));
	if (str != NULL) {
		sprintf(str, NUM_SOLUTIONS_OUTPUT_FORMAT, numSolutionsArguments->numSolutionsOut);
	}

	return str;
}

int performCommand(State* state, Command* command) {
	switch (command->type) {
		case COMMAND_TYPE_SOLVE:
			return performSolveCommand(state, command);
		case COMMAND_TYPE_EDIT:
			return performEditCommand(state, command);
		case COMMAND_TYPE_MARK_ERRORS:
			return performMarkErrorsCommand(state, command);
		case COMMAND_TYPE_SET:
			return performSetCommand(state, command);
		case COMMAND_TYPE_VALIDATE:
			return performValidateCommand(state, command);
		case COMMAND_TYPE_GUESS:
			return performGuessCommand(state, command);
		case COMMAND_TYPE_GENERATE:
			return performGenerateCommand(state, command);
		case COMMAND_TYPE_UNDO:
			return performUndoCommand(state, command);
		case COMMAND_TYPE_REDO:
			return performRedoCommand(state, command);
		case COMMAND_TYPE_SAVE:
			return performSaveCommand(state, command);
		case COMMAND_TYPE_HINT:
			return performHintCommand(state, command);
		case COMMAND_TYPE_GUESS_HINT:
			return performGuessHintCommand(state, command);
		case COMMAND_TYPE_NUM_SOLUTIONS:
			return performNumSolutionsCommand(state, command);
		case COMMAND_TYPE_AUTOFILL:
			return performAutofillCommand(state, command);
		case COMMAND_TYPE_RESET:
			return performResetCommand(state, command);
		case COMMAND_TYPE_PRINT_BOARD:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE:
			return ERROR_SUCCESS;
		}

	return ERROR_SUCCESS;
}

getCommandErrorStringFunc getGetCommandErrorStringFunc(CommandType type) {
	switch (type) {
		case COMMAND_TYPE_SOLVE:
			return getSolveCommandErrorString;
		case COMMAND_TYPE_EDIT:
			return getEditCommandErrorString;
		case COMMAND_TYPE_MARK_ERRORS:
			return getMarkErrorsCommandErrorString;
		case COMMAND_TYPE_SET:
			return getSetCommandErrorString;
		case COMMAND_TYPE_VALIDATE:
			return getValidateCommandErrorString;
		case COMMAND_TYPE_GUESS:
			return getGuessCommandErrorString;
		case COMMAND_TYPE_GENERATE:
			return getGenerateCommandErrorString;
		case COMMAND_TYPE_UNDO:
			return getUndoCommandErrorString;
		case COMMAND_TYPE_REDO:
			return getRedoCommandErrorString;
		case COMMAND_TYPE_SAVE:
			return getSaveCommandErrorString;
		case COMMAND_TYPE_HINT:
			return getHintCommandErrorString;
		case COMMAND_TYPE_GUESS_HINT:
			return getGuessHintCommandErrorString;
		case COMMAND_TYPE_NUM_SOLUTIONS:
			return getNumSolutionsCommandErrorString;
		case COMMAND_TYPE_AUTOFILL:
			return getAutoFillCommandErrorString;
		case COMMAND_TYPE_RESET:
		case COMMAND_TYPE_PRINT_BOARD:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE:
			return NULL;
		}

	return NULL;
}

char* getCommandErrorString(CommandType type, int error) {
	getCommandErrorStringFunc func = getGetCommandErrorStringFunc(type);
	return func(error);
}

isCommandErrorRecoverableFunc getIsCommandErrorRecoverableFunc(CommandType type) {
	switch (type) {
		case COMMAND_TYPE_SOLVE:
			return isSolveCommandErrorRecoverable;
		case COMMAND_TYPE_EDIT:
			return isEditCommandErrorRecoverable;
		case COMMAND_TYPE_MARK_ERRORS:
			return isMarkErrorsCommandErrorRecoverable;
		case COMMAND_TYPE_SET:
			return isSetCommandErrorRecoverable;
		case COMMAND_TYPE_VALIDATE:
			return isValidateCommandErrorRecoverable;
		case COMMAND_TYPE_GUESS:
			return isGuessCommandErrorRecoverable;
		case COMMAND_TYPE_GENERATE:
			return isGenerateCommandErrorRecoverable;
		case COMMAND_TYPE_UNDO:
			return isUndoCommandErrorRecoverable;
		case COMMAND_TYPE_REDO:
			return isRedoCommandErrorRecoverable;
		case COMMAND_TYPE_SAVE:
			return isSaveCommandErrorRecoverable;
		case COMMAND_TYPE_HINT:
			return isHintCommandErrorRecoverable;
		case COMMAND_TYPE_GUESS_HINT:
			return isGuessHintCommandErrorRecoverable;
		case COMMAND_TYPE_NUM_SOLUTIONS:
			return isNumSolutionsCommandErrorRecoverable;
		case COMMAND_TYPE_AUTOFILL:
			return isAutofillCommandErrorRecoverable;
		case COMMAND_TYPE_RESET:
			return isResetCommandErrorRecoverable;
		case COMMAND_TYPE_PRINT_BOARD:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE:
			return NULL;
		}
	return NULL;
}

bool isCommandErrorRecoverable(CommandType type, int error) {
	isCommandErrorRecoverableFunc func = getIsCommandErrorRecoverableFunc(type);
	return func(error);
}

char* getAutofillCommandStrOutput(Command* command, GameState* gameState) { /* CR: autofill shouldn't have any output */
	AutofillCommandArguments* autofillArguments = (AutofillCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	if (autofillArguments->movesListOut != NULL) {
		numCharsRequired = getMoveStrOutputSize(gameState, autofillArguments->movesListOut);
		str = calloc(numCharsRequired, sizeof(char));
		sprintMoveStrOutput(str, autofillArguments->movesListOut, false);
	} else {
		numCharsRequired = strlen(emptyString) + 1;
		str = calloc(numCharsRequired, sizeof(char));
	}

	return str;
}

char* getResetCommandStrOutput(Command* command, GameState* gameState) {
	ResetCommandArguments* resetArguments = (ResetCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(resetArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

char* getPrintBoardCommandStrOutput(Command* command, GameState* gameState) {
	PrintBoardCommandArguments* printBoardArguments = (PrintBoardCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(printBoardArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

char* getExitCommandStrOutput(Command* command, GameState* gameState) {
	ExitCommandArguments* exitArguments = (ExitCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(exitArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

char* getIgnoreCommandStrOutput(Command* command, GameState* gameState) { /* Note: function is not really necessary (will never be called) */
	IgnoreCommandArguments* ignoreArguments = (IgnoreCommandArguments*)(command->arguments);

	char* str = NULL;
	size_t numCharsRequired = 0;
	char* emptyString = "";

	UNUSED(ignoreArguments);
	UNUSED(gameState);

	numCharsRequired = strlen(emptyString) + 1;
	str = calloc(numCharsRequired, sizeof(char));

	return str;
}

getCommandStrOutputFunc getGetCommandStrOutputFunc(CommandType type) {
	switch (type) {
		case COMMAND_TYPE_SOLVE:
			return getSolveCommandStrOutput;
		case COMMAND_TYPE_EDIT:
			return getEditCommandStrOutput;
		case COMMAND_TYPE_MARK_ERRORS:
			return getMarkErrorsCommandStrOutput;
		case COMMAND_TYPE_SET:
			return getSetCommandStrOutput;
		case COMMAND_TYPE_VALIDATE:
			return getValidateCommandStrOutput;
		case COMMAND_TYPE_GUESS:
			return getGuessCommandStrOutput;
		case COMMAND_TYPE_GENERATE:
			return getGenerateCommandStrOutput;
		case COMMAND_TYPE_UNDO:
			return getUndoCommandStrOutput;
		case COMMAND_TYPE_REDO:
			return getRedoCommandStrOutput;
		case COMMAND_TYPE_SAVE:
			return getSaveCommandStrOutput;
		case COMMAND_TYPE_HINT:
			return getHintCommandStrOutput;
		case COMMAND_TYPE_GUESS_HINT:
			return getGuessHintCommandStrOutput;
		case COMMAND_TYPE_NUM_SOLUTIONS:
			return getNumSolutionsCommandStrOutput;
		case COMMAND_TYPE_AUTOFILL:
			return getAutofillCommandStrOutput;
		case COMMAND_TYPE_RESET:
			return getResetCommandStrOutput;
		case COMMAND_TYPE_PRINT_BOARD:
			return getPrintBoardCommandStrOutput;
		case COMMAND_TYPE_EXIT:
			return getExitCommandStrOutput;
		case COMMAND_TYPE_IGNORE: /* Note: effectively unreachable */
			return getIgnoreCommandStrOutput;
		}
	return NULL;
}

char* getCommandStrOutput(Command* command, GameState* gameState) {
	getCommandStrOutputFunc func = getGetCommandStrOutputFunc(command->type);
	return func(command, gameState);
}

bool shouldPrintBoardPostCommand(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_SOLVE:
	case COMMAND_TYPE_EDIT:
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_SET:
	case COMMAND_TYPE_GUESS:
	case COMMAND_TYPE_GENERATE:
	case COMMAND_TYPE_UNDO:
	case COMMAND_TYPE_REDO:
	case COMMAND_TYPE_AUTOFILL:
	case COMMAND_TYPE_RESET:
		return true;
	case COMMAND_TYPE_MARK_ERRORS:
	case COMMAND_TYPE_VALIDATE:
	case COMMAND_TYPE_SAVE:
	case COMMAND_TYPE_HINT:
	case COMMAND_TYPE_GUESS_HINT:
	case COMMAND_TYPE_NUM_SOLUTIONS:
	case COMMAND_TYPE_EXIT:
	case COMMAND_TYPE_IGNORE:
		return false;
	}
	return false;
}

ProcessStringAsCommandErrorCode processCommandArguments(State* state, char* commandStrTokens[], Command* commandOut, int* problematicArgNo, int* argsValidatorError) {
	int i = 0;
	commandArgsParser parser = NULL;
	commandArgsRangeChecker rangeChecker = NULL;
	commandArgsValidator validator = NULL;

	commandOut->arguments = calloc(1, getSizeofCommandArgsStruct(commandOut->type));
	if (commandOut->arguments == NULL) {
		return PROCESS_STRING_AS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILURE;
	}

	parser = getCommandArgsParser(commandOut->type);
	rangeChecker = getCommandArgsRangeChecker(commandOut->type);
	validator = getCommandArgsValidator(commandOut->type);

	for (i = 0; i < commandOut->argumentsNum; i++) {
		*problematicArgNo = i + 1;

		if (!parser(commandStrTokens[i], i + 1, commandOut->arguments)) {
			return PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_PARSED;
		}
		if (rangeChecker && !rangeChecker(commandOut->arguments, i + 1, state->gameState)) {
			return PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_IN_RANGE;
		}
		if (validator && !validator(commandOut->arguments, i + 1, state->gameState, argsValidatorError)) {
			return PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_AGREEING_WITH_BOARD;
		}
	}
	return ERROR_SUCCESS;

}

ProcessStringAsCommandErrorCode processStringAsCommand(State* state, char* commandStr, Command* commandOut, int* problematicArgNo, int* argsValidatorError) {
	char* commandStrTokens[(COMMAND_MAX_LENGTH + 1)/2 + 1] = {0}; /* A definite upper-boundary on the number of possible tokens */
	char* commandType = NULL;

	commandType = getFirstToken(commandStr);
	if (!identifyCommandByType(commandType, commandOut)) {
		return PROCESS_STRING_AS_COMMAND_UNKNOWN_COMMAND;
	}
	if (!isCommandAllowed(state->gameMode, commandOut->type)) {
		return PROCESS_STRING_AS_COMMAND_COMMAND_NOT_ALLOWED_IN_CURRENT_MODE;
	}

	if (commandOut->type == COMMAND_TYPE_IGNORE) /* in such a case, looking for parameters is error-prone, and in any case there is no need */
		return ERROR_SUCCESS;

	commandOut->argumentsNum = splitArgumentsStringToTokens(commandStr, commandStrTokens);
	if (!isCorrectArgumentsNum(commandOut)) {
		return PROCESS_STRING_AS_COMMAND_INCORRECT_ARGUMENTS_NUM;
	}

	return processCommandArguments(state, commandStrTokens, commandOut, problematicArgNo, argsValidatorError);
}

#define INVALID_COMMAND_STR ("invalid command (%s)\n")
#define INVALID_COMMAND_UNKNOWN_STR ("unknown")
#define INVALID_COMMAND_NOT_ALLOWED_STR ("not allowed in the current mode")
#define ALLOWED_COMMANDS_IN_CUR_MODE_STR ("Allowed commands in current mode are the following: %s\n")
#define COMMAND_ALLOWED_IN_FOLLOWING_MODES_STR ("Command '%s' is allowed in the following modes: %s\n")
#define INCORRECT_NUM_OF_ARGS ("incorrect number of arguments\n")
#define USAGE_PREFIX_STR ("Usage: %s\n")
#define MEMORY_ALLOCATION_FAILURE_STR ("Memory allocation failure\n")
#define ARG_PARSING_ERROR_STR ("failed to parse argument no. %2d (due to wrong type)\n")
#define ARG_VALUE_NOT_IN_RANGE_STR ("Value of argument no. %2d is out of expected range (%s)\n")
#define ARG_VALUE_NOT_AGREEING_WITH_BOARD ("Value of argument no. %2d does not agree with the current board state (%s)\n")

char* getProcessStringAsCommandErrorString(ProcessStringAsCommandErrorCode errorCode, int problematicArgNo, State* state, Command* command, char* commandName, int argsValidatorError) {
	char* str = NULL;
	char* format = NULL;
	size_t requiredSize = 0;
	char* expectedRangeString = NULL;

	switch (errorCode) {
	case PROCESS_STRING_AS_COMMAND_UNKNOWN_COMMAND:
		requiredSize = sizeof(INVALID_COMMAND_STR) + sizeof(INVALID_COMMAND_UNKNOWN_STR) +
					   sizeof(ALLOWED_COMMANDS_IN_CUR_MODE_STR) + (strlen(getAllowedCommandsString(state->gameMode)) + 1);
		format = calloc(requiredSize, sizeof(char));
		str = calloc(requiredSize, sizeof(char));
		if ((format != NULL) && (str != NULL)) {
			strcpy(format, INVALID_COMMAND_STR);
			strcat(format, ALLOWED_COMMANDS_IN_CUR_MODE_STR);
			sprintf(str, format, INVALID_COMMAND_UNKNOWN_STR, getAllowedCommandsString(state->gameMode));
		}
		break;
	case PROCESS_STRING_AS_COMMAND_COMMAND_NOT_ALLOWED_IN_CURRENT_MODE:
		requiredSize = sizeof(INVALID_COMMAND_STR) + sizeof(INVALID_COMMAND_NOT_ALLOWED_STR) +
					   sizeof(COMMAND_ALLOWED_IN_FOLLOWING_MODES_STR) + (strlen(commandName) + 1) + (strlen(getAllowingModesString(command->type)) + 1);
		format = calloc(requiredSize, sizeof(char));
		str = calloc(requiredSize, sizeof(char));
		if ((format != NULL) && (str != NULL)) {
			strcpy(format, INVALID_COMMAND_STR);
			strcat(format, COMMAND_ALLOWED_IN_FOLLOWING_MODES_STR);
			sprintf(str, format, INVALID_COMMAND_NOT_ALLOWED_STR, commandName, getAllowingModesString(command->type));
		}
		break;
	case PROCESS_STRING_AS_COMMAND_INCORRECT_ARGUMENTS_NUM:
		requiredSize = sizeof(INCORRECT_NUM_OF_ARGS) +
					   sizeof(USAGE_PREFIX_STR) + (strlen(getCommandUsage(command->type)) + 1);
		format = calloc(requiredSize, sizeof(char));
		str = calloc(requiredSize, sizeof(char));
		if ((format != NULL) && (str != NULL)) {
			strcpy(format, INCORRECT_NUM_OF_ARGS);
			strcat(format, USAGE_PREFIX_STR);
			sprintf(str, format, getCommandUsage(command->type));
		}
		break;
	case PROCESS_STRING_AS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILURE:
		requiredSize = sizeof(MEMORY_ALLOCATION_FAILURE_STR);
		str = calloc(requiredSize, sizeof(char));
		if (str != NULL) {
			strcpy(str, MEMORY_ALLOCATION_FAILURE_STR);
		}
		break;
	case PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_PARSED:
		requiredSize = sizeof(ARG_PARSING_ERROR_STR) +
					   sizeof(USAGE_PREFIX_STR) + (strlen(getCommandUsage(command->type)) + 1);
		format = calloc(requiredSize, sizeof(char));
		str = calloc(requiredSize, sizeof(char));
		if ((format != NULL) && (str != NULL)) {
			strcpy(format, ARG_PARSING_ERROR_STR);
			strcat(format, USAGE_PREFIX_STR);
			sprintf(str, format, problematicArgNo, getCommandUsage(command->type));
		}
		break;
	case PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_IN_RANGE:
		expectedRangeString = getCommandArgsExpectedRangeString(command->type, problematicArgNo, state->gameState);
		if (expectedRangeString != NULL) {
			requiredSize = sizeof(ARG_VALUE_NOT_IN_RANGE_STR) + (strlen(expectedRangeString) + 1);
			format = calloc(requiredSize, sizeof(char));
			str = calloc(requiredSize, sizeof(char));
			if ((format != NULL) && (str != NULL)) {
				strcpy(format, ARG_VALUE_NOT_IN_RANGE_STR);
				sprintf(str, format, problematicArgNo, expectedRangeString);
			}
			free(expectedRangeString);
		}
		break;
	case PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_AGREEING_WITH_BOARD:
		requiredSize = sizeof(ARG_VALUE_NOT_AGREEING_WITH_BOARD) + (strlen(getGetCommandArgsValidatorErrorString(command->type)(argsValidatorError)) + 1);
		format = calloc(requiredSize, sizeof(char));
		str = calloc(requiredSize, sizeof(char));
		if ((format != NULL) && (str != NULL)) {
			strcpy(format, ARG_VALUE_NOT_AGREEING_WITH_BOARD);
			sprintf(str, format, problematicArgNo, getGetCommandArgsValidatorErrorString(command->type)(argsValidatorError));
		}
		break;
	}

	if (format != NULL)
		free(format);
	return str;
}

void guessHintCommandArgsCleaner(void* arguments) {
	GuessHintCommandArguments* guessHintArguments = (GuessHintCommandArguments*)(arguments);
	if (guessHintArguments->valuesScoresOut != NULL) {
		free(guessHintArguments->valuesScoresOut);
		guessHintArguments->valuesScoresOut = NULL;
	}
}

commandArgsCleaner getCommandArgsCleaner(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_GUESS_HINT:
		return guessHintCommandArgsCleaner;
	case COMMAND_TYPE_SOLVE:
	case COMMAND_TYPE_EDIT:
	case COMMAND_TYPE_MARK_ERRORS:
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_SET:
	case COMMAND_TYPE_VALIDATE:
	case COMMAND_TYPE_GUESS:
	case COMMAND_TYPE_GENERATE:
	case COMMAND_TYPE_UNDO:
	case COMMAND_TYPE_REDO:
	case COMMAND_TYPE_SAVE:
	case COMMAND_TYPE_HINT:
	case COMMAND_TYPE_NUM_SOLUTIONS:
	case COMMAND_TYPE_AUTOFILL:
	case COMMAND_TYPE_RESET:
	case COMMAND_TYPE_EXIT:
	case COMMAND_TYPE_IGNORE:
		return NULL;
	}
	return NULL;
}

void cleanupCommand(Command* command) {
	commandArgsCleaner cleaner = NULL;

	if (command == NULL || command->arguments == NULL) {
		return;
	}

	cleaner = getCommandArgsCleaner(command->type);

	if (cleaner != NULL) {
		cleaner(command->arguments);
	}

	free(command->arguments);
	command->arguments = NULL;
}
