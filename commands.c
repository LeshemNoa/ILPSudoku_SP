#include "commands.h"

#define UNUSED(x) (void)(x)

#define DEFAULT_M (3)
#define DEFAULT_N (3)

#define FIXED_CELL_MARKER_IN_FILE ('.')
#define EMPTY_CELL_VALUE_IN_FILE (0)

/* TODO: check necessity of IGNORE in all of the below */

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
	case COMMAND_TYPE_IGNORE: /* TODO: is needed? */
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
		return parseStringArg(arg, &(editArguments->filePath)); /* TODO: what happens in case of quotes sign? (i.e., folder name with a space) */
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
		return parseIntArg(arg - 1, &(setArguments->value));
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

bool guessArgsParser(char* arg, int argNo, void* arguments) {
	GuessCommandArguments* guessArguments = (GuessCommandArguments*)arguments;
	switch (argNo) {
	case 1:
		return parseFloatArg(arg, &(guessArguments->threshold));
	}
	return false;
}

/* TODO: is guess's threshold limited in its range? */

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
			return generateArguments->numEmptyCellsToFill >= 0 && generateArguments->numEmptyCellsToFill <= getBoardSize_MN2(gameState);
		case 2:
			return generateArguments->numCellsToClear >= 0 && generateArguments->numCellsToClear <= getBoardSize_MN2(gameState);
		}
		return false;
}

bool generateArgsValidator(void* arguments, int argNo, GameState* gameState) {
	GenerateCommandArguments* generateArguments = (GenerateCommandArguments*)arguments;
		switch (argNo) {
		case 1:
			return (generateArguments->numEmptyCellsToFill >= 0) && (generateArguments->numEmptyCellsToFill <= getNumEmptyCells(gameState)); 
		case 2:
			return true;
		}
		return false;
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
	case COMMAND_TYPE_IGNORE: /* TODO: is needed? */
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
	case COMMAND_TYPE_SOLVE:
	case COMMAND_TYPE_EDIT:
	case COMMAND_TYPE_MARK_ERRORS:
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_VALIDATE:
	case COMMAND_TYPE_GUESS:
	case COMMAND_TYPE_UNDO:
	case COMMAND_TYPE_REDO:
	case COMMAND_TYPE_SAVE:
	case COMMAND_TYPE_NUM_SOLUTIONS:
	case COMMAND_TYPE_AUTOFILL:
	case COMMAND_TYPE_RESET:
	case COMMAND_TYPE_EXIT:
	case COMMAND_TYPE_IGNORE: /* TODO: is needed? */
		return NULL;
	}
	return NULL;
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
	case COMMAND_TYPE_IGNORE: /* TODO: is needed? */
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

IsBoardValidForCommandErrorCode isBoardValidForUndoCommand(State* state, Command* command) {
	UndoCommandArguments* args = (UndoCommandArguments*)command->arguments;

	UNUSED(args);

	if (isThereMoveToUndo(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_NO_MOVE_TO_UNDO;
	}

	return ERROR_SUCCESS;
}

IsBoardValidForCommandErrorCode isBoardValidForRedoCommand(State* state, Command* command) {
	RedoCommandArguments* args = (RedoCommandArguments*)command->arguments;

	UNUSED(args);

	if (isThereMoveToRedo(state->gameState)) {
		return IS_BOARD_VALID_FOR_COMMAND_NO_MOVE_TO_REDO;
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
		if (!isBoardSolvable(state->gameState)) {
			return IS_BOARD_VALID_FOR_COMMAND_BOARD_UNSOLVABLE;
		}
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
		case COMMAND_TYPE_UNDO:
			return isBoardValidForUndoCommand(state, command);
		case COMMAND_TYPE_REDO:
			return isBoardValidForRedoCommand(state, command);
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
		case COMMAND_TYPE_GENERATE:
		case COMMAND_TYPE_RESET:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE: /* TODO: is needed? */
			return ERROR_SUCCESS;
		}
	return ERROR_SUCCESS;
}

typedef enum {
	PERFORM_EDIT_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_EDIT_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE
} PerformEditCommandErrorCode;

typedef enum {
	LOAD_BOARD_FROM_FILE_FILE_COULD_NOT_BE_OPENED = 1,
	LOAD_BOARD_FROM_FILE_COULD_NOT_PARSE_BOARD_DIMENSIONS,
	LOAD_BOARD_FROM_FILE_DIMENSION_ARE_NOT_POSITIVE,
	LOAD_BOARD_FROM_FILE_BAD_FORMAT_FAILED_TO_READ_A_CELL,
	LOAD_BOARD_FROM_FILE_BAD_FORMAT_FILE_CONTAINS_TOO_MUCH_CONTENT,
	LOAD_BOARD_FROM_FILE_CELL_VALUE_NOT_IN_RANGE
} LoadBoardFromFileErrorCode;

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
	int nm = boardInOut->numColumnsInBlock_N * boardInOut->numRowsInBlock_M;

	int row = 0, col = 0;
	for (row = 0; row < nm; row++)
		for (col = 0; col < nm; col++)
			if (!readCellFromFileToBoard(file, &(boardInOut->cells[row][col]), (row + 1 == nm) && (col + 1 == nm)))
				return false;

	return true;
}

bool areCellValuesInRange(Board* board) {
	int nm = board->numColumnsInBlock_N * board->numRowsInBlock_M;
	int row = 0;
	int col = 0;
	for (row = 0; row < nm; row++)
		for (col = 0; col < nm; col++) {
			int value = board->cells[row][col].value;
			if ((value != EMPTY_CELL_VALUE_IN_FILE) && ((value < 1) || (value > nm)))
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
				if (createBoard(&tempBoard, m, n) == NULL) {
					retVal = PERFORM_EDIT_COMMAND_MEMORY_ALLOCATION_FAILURE;
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
								findErroneousCells(&tempBoard);
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

PerformEditCommandErrorCode performEditCommand(State* state, Command* command) {
	EditCommandArguments* editArguments = (EditCommandArguments*)(command->arguments);

	if (command->argumentsNum == 0) { /* Start editing an empty 9x9 board */
		cleanupGameState(state);

		 if (createGameState(state, DEFAULT_M, DEFAULT_N, NULL) == NULL) {
			 return PERFORM_EDIT_COMMAND_MEMORY_ALLOCATION_FAILURE;
		 }

	} else { /* Open from file */
		Board board;
		PerformEditCommandErrorCode retVal = ERROR_SUCCESS;
		retVal = loadBoardFromFile(editArguments->filePath, &board);
		if (retVal != ERROR_SUCCESS)
			return PERFORM_EDIT_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE + retVal;
		if (createGameState(state, 0, 0, &board) == NULL) {
			cleanupBoard(&board);
			return PERFORM_EDIT_COMMAND_MEMORY_ALLOCATION_FAILURE;
		}
		markAllCellsAsNotFixed(state->gameState);
	}

	state->gameMode = GAME_MODE_EDIT;
	return ERROR_SUCCESS;
}

typedef enum {
	PERFORM_SOLVE_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_SOLVE_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE
} PerformSolveCommandErrorCode;

PerformSolveCommandErrorCode performSolveCommand(State* state, Command* command) {
	SolveCommandArguments* solveArguments = (SolveCommandArguments*)(command->arguments);

	Board board;
	PerformSolveCommandErrorCode retVal = ERROR_SUCCESS;
	retVal = loadBoardFromFile(solveArguments->filePath, &board);
	if (retVal != ERROR_SUCCESS)
		return PERFORM_SOLVE_COMMAND_ERROR_IN_LOAD_BOARD_FROM_FILE + retVal;
	if (createGameState(state, 0, 0, &board) == NULL) {
		cleanupBoard(&board);
		return PERFORM_SOLVE_COMMAND_MEMORY_ALLOCATION_FAILURE;
	}

	state->gameMode = GAME_MODE_SOLVE;
	return ERROR_SUCCESS;
}

typedef enum {
	PERFORM_SAVE_COMMAND_MEMORY_ALLOCATION_FAILURE = 1,
	PERFORM_SAVE_COMMAND_ERROR_IN_SAVE_BOARD_TO_FILE
} PerformSaveCommandErrorCode;

typedef enum {
	SAVE_BOARD_TO_FILE_FILE_COULD_NOT_BE_OPENED = 1,
	SAVE_BOARD_TO_FILE_DIMENSIONS_COULD_NOT_BE_WRITTEN,
	SAVE_BOARD_TO_FILE_FAILED_TO_WRITE_A_CELL
} SaveBoardToFileErrorCode;

typedef enum {
	WRITE_CELL_TO_FILE_FIXEDNESS_CRITERION_CELL_IS_FIXED,
	WRITE_CELL_TO_FILE_FIXEDNESS_CRITERION_CELL_IS_NOT_EMPTY
} writeCellFromBoardToFileFixednessCriterion;



bool writeCellFromBoardToFile(FILE* file, Cell* cell, writeCellFromBoardToFileFixednessCriterion fixednessCriterion, bool isLastInRow) {
	int fprintfRetVal = 0;
	fprintfRetVal = fprintf(file, "%d", cell->value);
	if (fprintfRetVal <= 0)
		return false;

	if (((fixednessCriterion == WRITE_CELL_TO_FILE_FIXEDNESS_CRITERION_CELL_IS_FIXED) && isBoardCellFixed(cell)) ||
		((fixednessCriterion == WRITE_CELL_TO_FILE_FIXEDNESS_CRITERION_CELL_IS_NOT_EMPTY) && !isBoardCellEmpty(cell))) {
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

bool writeCellsFromBoardToFile(FILE* file, Board* boardInOut, writeCellFromBoardToFileFixednessCriterion fixednessCriterion) {
	int nm = boardInOut->numColumnsInBlock_N * boardInOut->numRowsInBlock_M;

	int row = 0, col = 0;
	for (row = 0; row < nm; row++) {
		for (col = 0; col < nm; col++)
			if (!writeCellFromBoardToFile(file, &(boardInOut->cells[row][col]), fixednessCriterion, (col + 1 == nm)))
				return false;
		fprintf(file, "\n");
	}

	return true;
}

SaveBoardToFileErrorCode saveBoardToFile(char* filePath, Board* board, writeCellFromBoardToFileFixednessCriterion fixednessCriterion) {
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
			if (!writeCellsFromBoardToFile(file, board, fixednessCriterion)) {
				retVal = SAVE_BOARD_TO_FILE_FAILED_TO_WRITE_A_CELL;
			}
		}
		fclose(file);
	}
	return retVal;
}

PerformSaveCommandErrorCode performSaveCommand(State* state, Command* command) {
	SaveCommandArguments* saveArguments = (SaveCommandArguments*)(command->arguments);

	PerformSaveCommandErrorCode retVal = ERROR_SUCCESS;

	Board exportedBoard = {0};

	writeCellFromBoardToFileFixednessCriterion fixednessCriteria = WRITE_CELL_TO_FILE_FIXEDNESS_CRITERION_CELL_IS_FIXED;
	if (state->gameMode == GAME_MODE_EDIT) {
		fixednessCriteria = WRITE_CELL_TO_FILE_FIXEDNESS_CRITERION_CELL_IS_NOT_EMPTY;
	}

	if (!exportBoard(state->gameState, &exportedBoard)) {
		return PERFORM_SAVE_COMMAND_MEMORY_ALLOCATION_FAILURE;
	}

	retVal = saveBoardToFile(saveArguments->filePath, &exportedBoard, fixednessCriteria);
	cleanupBoard(&exportedBoard);
	if (retVal != ERROR_SUCCESS)
		return PERFORM_SAVE_COMMAND_ERROR_IN_SAVE_BOARD_TO_FILE + retVal;

	return ERROR_SUCCESS;
}

typedef enum {
	PERFORM_MARK_ERRORS_COMMAND_NO_CHANGE = 1
} PerformMarkErrorsCommandErrorCode;

PerformMarkErrorsCommandErrorCode performMarkErrorsCommand(State* state, Command* command) {
	MarkErrorsCommandArguments* markErrorsArguments = (MarkErrorsCommandArguments*)(command->arguments);

	if (shouldMarkErrors(state->gameState) == markErrorsArguments->shouldMarkError)
		return PERFORM_MARK_ERRORS_COMMAND_NO_CHANGE;

	setMarkErrors(state->gameState, markErrorsArguments->shouldMarkError);

	return ERROR_SUCCESS;
}

typedef enum {
	VALUE_FIXED = 1,
	VALUE_INVALID /* is this necessary? */
} PerformSetCommandErrorCode;

PerformSetCommandErrorCode performSetCommand(State* state, Command* command) {
	SetCommandArguments* setArguments = (SetCommandArguments*)(command->arguments);

	switch(state->gameMode) {
		case (GAME_MODE_EDIT):
			
		case (GAME_MODE_SOLVE):
	}
}

 

int performCommand(State* state, Command* command) {
	int errorCode = ERROR_SUCCESS;

	UNUSED(state);
	UNUSED(command);

	switch (command->type) {
		case COMMAND_TYPE_SOLVE:
			return performSolveCommand(state, command);
		case COMMAND_TYPE_EDIT:
			return performEditCommand(state, command);
		case COMMAND_TYPE_MARK_ERRORS:
			return performMarkErrorsCommand(state, command);
		/*case COMMAND_TYPE_SET:
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
			return performRedoCommand(state, command);*/
		case COMMAND_TYPE_SAVE:
			return performSaveCommand(state, command);
		/*case COMMAND_TYPE_HINT:
			return performHintCommand(state, command);
		case COMMAND_TYPE_GUESS_HINT:
			return performGuessHintCommand(state, command);
		case COMMAND_TYPE_NUM_SOLUTIONS:
			return performNumSolutionsCommand(state, command);
		case COMMAND_TYPE_AUTOFILL:
			return performAutofillCommand(state, command);
		case COMMAND_TYPE_RESET:
			return performResetCommand(state, command);*/
		case COMMAND_TYPE_PRINT_BOARD:
		case COMMAND_TYPE_EXIT:
		case COMMAND_TYPE_IGNORE: /* TODO: is needed? */
			return ERROR_SUCCESS;
		default: /* TODO: get rid of this */
			break;
		}

	return errorCode;
}

bool shouldPrintBoardPostCommand(CommandType commandType) {
	switch (commandType) {
	case COMMAND_TYPE_SOLVE:
	case COMMAND_TYPE_EDIT:
	case COMMAND_TYPE_PRINT_BOARD:
	case COMMAND_TYPE_SET:
	case COMMAND_TYPE_GUESS: /* TODO: check! this is contrary to instructions, but seems legit (?) */
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

ProcessStringAsCommandErrorCode processCommandArguments(State* state, char* commandStrTokens[], Command* commandOut, int* problematicArgNo) {
	int i = 0;
	commandArgsParser parser = NULL;
	commandArgsRangeChecker rangeChecker = NULL;
	commandArgsValidator validator = NULL;

	commandOut->arguments = calloc(1, getSizeofCommandArgsStruct(commandOut->type)); /* TODO: could avoid this using static allocation...! */
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
		if (validator && !validator(commandOut->arguments, i + 1, state->gameState)) {
			return PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_AGREEING_WITH_BOARD;
		}
	}
	return ERROR_SUCCESS;

}

ProcessStringAsCommandErrorCode processStringAsCommand(State* state, char* commandStr, Command* commandOut, int* problematicArgNo) {
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

	return processCommandArguments(state, commandStrTokens, commandOut, problematicArgNo);
}

commandArgsCleaner getCommandArgsCleaner(CommandType commandType) {
	switch (commandType) {
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
