#include "main_aux.h"

/* TODO: perhaps have errors printed to stderr, as discuseed earlier */

#define INPUT_STRING_MAX_LENGTH (COMMAND_MAX_LENGTH + sizeof(COMMAND_END_MARKER) + 1) /* One for COMMAND_END_MARKER, and one for the null terminator */

#define ERROR_SUCCESS (0)

typedef enum GetInputStringErrorCode {
	GET_INPUT_STRING_REACHED_EOF = 1,
	GET_INPUT_STRING_COMMAND_TOO_LONG} GetInputStringErrorCode;

/**
 * getCommandString reads a command string from stdin, and writes the input
 * into the provided string pointer.
 * 
 * @param commandStrOut		[out] a pointer to a string. 
 * @param commandMaxSize	[in] the maximum input size in bytes that should be read stdin
 * @return true 			iff a string was successfully fetched
 * @return false 			iff there had been an error and the input fetch had failed
 */
GetInputStringErrorCode getInputString(char* commandStrOut, int commandMaxSize) {
	bool foundNewline = false;
	int numIterations = 0;

	/* A command is identified by its ending char: a newline.
	 * So we read from the input buffer until it is obtained*/
	do {
		char* fgetsRes = fgets(commandStrOut, commandMaxSize, stdin);
		if (fgetsRes == NULL) { /* EOF was reached */
			printf("EOF was reached.\n");
			return GET_INPUT_STRING_REACHED_EOF;
		} else if (strchr(commandStrOut, COMMAND_END_MARKER) != NULL) {
			foundNewline = true;
		}
		numIterations++;
	} while(!foundNewline);

	if (numIterations > 1) {
		/* Since commandMaxSizes represents not only the maximum number of characters allowed to be read in one go, but also
		 * the maximal length of any given command, in case we needed more than one iteration of the above loop, the command
		 * is to be treated as too long.
		 */
		printf("Error: command's length exceeds the maximum of %d characters\n", COMMAND_MAX_LENGTH);
		return GET_INPUT_STRING_COMMAND_TOO_LONG;
	}

	return ERROR_SUCCESS;
}

void promptUserToInput(State* state) {
	printf("%s>", getCurModeString(state));
}

void printIsBoardValidForCommandError(IsBoardValidForCommandErrorCode errorCode) {
	printf("The board is not valid to perform desired command: ");
	switch (errorCode) {
	case IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS:
		printf("it is erroneous");
		break;
	case IS_BOARD_VALID_FOR_COMMAND_NO_MOVE_TO_UNDO:
		printf("there is no move to undo");
		break;
	case IS_BOARD_VALID_FOR_COMMAND_NO_MOVE_TO_REDO:
		printf("there is no move to redo");
		break;
	case IS_BOARD_VALID_FOR_COMMAND_BOARD_UNSOLVABLE:
		printf("it is unsolvable");
		break;
	case IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_ERRONEOUS_VALUE:
		printf("cell has erroneous value");
		break;
	case IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_FIXED_VALUE:
		printf("cell has a fixed value");
		break;
	case IS_BOARD_VALID_FOR_COMMAND_CELL_IS_NOT_EMPTY:
		printf("cell is not empty");
		break;
	}
	printf("\n");
}

void printProcessStringAsCommandError(ProcessStringAsCommandErrorCode errorCode, int problematicArgNo, State* state, Command* command, char* commandName) {
	switch (errorCode) {
	case PROCESS_STRING_AS_COMMAND_UNKNOWN_COMMAND:
		printf("Error: invalid command (unknown)\n");
		printf("Allowed commands in current mode are the following: %s\n", getAllowedCommandsString(state->gameMode));
		break;
	case PROCESS_STRING_AS_COMMAND_COMMAND_NOT_ALLOWED_IN_CURRENT_MODE:
		printf("Error: invalid command (not allowed in the current mode)\n");
		printf("Command '%s' is allowed in the following modes: %s\n", commandName, getAllowingModesString(command->type));
		break;
	case PROCESS_STRING_AS_COMMAND_INCORRECT_ARGUMENTS_NUM:
		printf("Error: incorrect number of arguments\n");
		printf("Usage: %s\n", getCommandUsage(command->type));
		break;
	case PROCESS_STRING_AS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILURE:
		printf("Error: Memory allocation for command's arguments' struct failed\n");
		break;
	case PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_PARSED:
		printf("Error: failed to parse argument no. %d (due to wrong type)\n", problematicArgNo);
		break;
	case PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_IN_RANGE:
		printf("Error: Value of argument no. %d is out of expected range\n", problematicArgNo);
		break;
	case PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_AGREEING_WITH_BOARD:
		printf("Error: Value of argument no. %d does not agree with the current board state\n", problematicArgNo);
		break;
	}
}

/**
 * performCommandLoop manages the user interface of the game. It takes commands from the user and
 * validates them, displaying an error message when the command is found invalid. The commands are
 * then performed and the game is updated accordingly. After each user turn it checks if the game
 * should be terminated, then it finishes.
 * 
 * @param GameState		[in, out] a pointer to the current GameState of the game
 * @return true 	iff the game should be terminated
 */
void performCommandLoop(State* state) {
	bool loopHolds = true;
	while (loopHolds) {
		Command command = {0};
		char inputStr[INPUT_STRING_MAX_LENGTH] = {0};
		int errorCode = ERROR_SUCCESS;
		int problematicArgNo = 0;

		promptUserToInput(state);
		errorCode = getInputString(inputStr, INPUT_STRING_MAX_LENGTH);
		if (errorCode != ERROR_SUCCESS) {
			if (errorCode == GET_INPUT_STRING_REACHED_EOF)
				loopHolds = false;
			continue;
		}

		errorCode = processStringAsCommand(state, inputStr, &command, &problematicArgNo); /* Note: mustn't 'continue' or 'break' from this point on - cleanupCommand needs to be called in the end */
		if (errorCode != ERROR_SUCCESS) {
			printProcessStringAsCommandError((ProcessStringAsCommandErrorCode)errorCode, problematicArgNo, state, &command, inputStr);
			if (errorCode == PROCESS_STRING_AS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILURE)
				loopHolds = false;
		} else {

			errorCode = isBoardValidForCommand(state, &command);
			if (errorCode != ERROR_SUCCESS) {
				printIsBoardValidForCommandError((IsBoardValidForCommandErrorCode)errorCode);
			} else {

				errorCode = performCommand(state, &command);
				if (errorCode != ERROR_SUCCESS) {
					/* TODO: call a printErrorMessage function, which should translate the error codes to string */
				} else {
					if (shouldPrintBoardPostCommand(command.type)) {
						/* TODO: call  getBoard from Game module and printBoard from this module */
					}

					/* TODO: in solve mode and in case command was Set, if all cells are now filled, check whether board is solved or not and let user know (in case of successful solution, immediately switch to INIT mode */

					if (command.type == COMMAND_TYPE_EXIT) {
						loopHolds = false;
					}
				}
			}
		}

		cleanupCommand(&command);
	}
}

/**
 * runGame starts by initializing the sudoku board and runs the game, exiting when it
 * is finished.
 * 
 * @return true 	iff the game is exited (Rather than: restarted)
 */
void runGame() {
	State state = {0}; /* properly initialised */
	/* TODO: even though, perhaps add an "initialiseState function" */

	printf("\t~~~ Let's play Sudoku! ~~~\t\n\n");

	performCommandLoop(&state);

	cleanupGameState(&state);

	printf("Exiting...\n");
}
