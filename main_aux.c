#include "main_aux.h"

#define COMMAND_MAX_LENGTH (256) /* One extra character for the newline character */
#define INPUT_STRING_MAX_LENGTH (COMMAND_MAX_LENGTH + 1 + 1) /* One for COMMAND_END_MARKER, and one for the null terminator */
#define COMMAND_END_MARKER ('\n')

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

/*typedef enum ProcessCommandArgumentsErrorCode {
	PROCESS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILED = 1
	} ProcessCommandArgumentsErrorCode;*/

bool processCommandArguments(State* state, char* commandStrTokens[], Command* commandOut) {
	int i = 0;
	commandArgsParser parser = NULL;
	commandArgsRangeChecker rangeChecker = NULL;
	commandArgsValidator validator = NULL;

	/*commandOut->arguments = calloc(1, getSizeofCommandArgsStruct(commandOut->type));*/ /* TODO: could avoid this using static allocation...! */
	/*if (commandOut->arguments == NULL) {
		return PROCESS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILED;
	}*/

	parser = getCommandArgsParser(commandOut->type);
	rangeChecker = getCommandArgsRangeChecker(commandOut->type);
	validator = getCommandArgsValidator(commandOut->type);

	for (i = 0; i < commandOut->argumentsNum; i++) {
		if (!parser(commandStrTokens[i], i + 1, commandOut->arguments)) {
			printf("Error: failed to parse argument no. %d (due to wrong type)\n", i + 1);
			return false;
		}
		if (rangeChecker && !rangeChecker(commandOut->arguments, i + 1, state->gameState)) {
			printf("Error: Value of argument no. %d is out of exptected range\n", i + 1);
			return false;
		}
		if (validator && !validator(commandOut->arguments, i + 1, state->gameState)) {
			printf("Error: Value of argument no. %d does not agree with the current board state\n", i + 1);
			return false;
		}
	}
	return true;

}

bool processStringAsCommand(State* state, char* commandStr, Command* commandOut) {
	char* commandStrTokens[(COMMAND_MAX_LENGTH + 1)/2 + 1] = {0}; /* A definite upper-boundary on the number of possible tokens */
	char* commandType = NULL;

	commandType = getFirstToken(commandStr);
	if (!identifyCommandByType(commandType, commandOut)) {
		printf("Error: invalid command (unknown)\n");
		printf("Allowed commands in current mode are the following: %s\n", getAllowedCommandsString(state->gameMode));
		return false;
	}
	if (!isCommandAllowed(state->gameMode, commandOut->type)) {
		printf("Error: invalid command (not allowed in the current mode)\n");
		printf("Command '%s' is allowed in the following modes: %s\n", commandType, getAllowingModesString(commandOut->type));
		return false;
	}

	if (commandOut->type == COMMAND_TYPE_IGNORE) /* in such a case, looking for parameters is error-prone */
		return true;

	commandOut->argumentsNum = splitArgumentsStringToTokens(commandStr, commandStrTokens);
	if (!isCorrectArgumentsNum(commandOut)) {
		printf("Error: incorrect number of arguments\n");
		printf("Usage: %s\n", getCommandUsage(commandOut->type));
		return false;
	}

	if (!processCommandArguments(state, commandStrTokens, commandOut)) {
		return false;
	}

	return true;
}

void promptUserToInput(State* state) {
	printf("%s>", getCurModeString(state));
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
		GetInputStringErrorCode gisErrorCode = ERROR_SUCCESS;

		promptUserToInput(state);
		gisErrorCode = getInputString(inputStr, INPUT_STRING_MAX_LENGTH);
		if (gisErrorCode != ERROR_SUCCESS) {
			if (gisErrorCode == GET_INPUT_STRING_REACHED_EOF)
				loopHolds = false;
			continue;
		}

		if (processStringAsCommand(state, inputStr, &command)) {
			/* performCommand(...);*/
		}
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

	printf("Exiting...\n");
}
