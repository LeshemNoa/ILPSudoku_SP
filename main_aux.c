/**
 * MAIN_AUX Summary:
 *
 * A module designed to help main run a proper sudoku game
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main_aux.h"

#include "commands.h"

#define INPUT_STRING_MAX_LENGTH (COMMAND_MAX_LENGTH + sizeof(COMMAND_END_MARKER) + 1) /* Note: One for COMMAND_END_MARKER, and one for the null terminator */

#define ERROR_PREFIX_STR ("Error: ")

#define EOF_WAS_REACHED_ERROR_STR ("EOF was reached.\n")
#define COMMAND_IS_TOO_LONG_ERROR_STR ("command's length exceeds expectations (i.e., too many characters were entered in a single line)\n")

#define USER_PROMPT_MARKER_STR (">")

#define BOARD_SUCCESSFULLY_SOLVED_STR ("The board was successfully solved!\n")
#define BOARD_REMAINS_UNSOLVED_STR ("The board is erroneous and hence remains unsolved...\n")

#define GAME_HEADER_STR ("\t~~~ Let's play Sudoku! ~~~\t\n\n")
#define EXIT_STR ("Exiting...\n")

#define ERROR_SUCCESS (0)

/**
 * Prints input string to stdout.
 * 
 * @param str 	[in] input string
 */
void printString(char* str) {
	if (str != NULL)
		printf("%s", str);
}

/**
 * Prints a dynamically allocated string to stdout and free it afterwards.
 * 
 * @param str 		[in] input string
 * @return true 	iff the provided pointer was not NULL
 * @return false 	otherwise
 */
bool printAllocatedString(char* str) {
	printString(str);
	if (str != NULL) {
		free(str);
		return true;
	}
	return false;
}

/**
 * Print the error prefix string to stdout.
 * 
 */
void printErrorPrefix() {
	printString(ERROR_PREFIX_STR);
}

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
			printErrorPrefix(); printString(EOF_WAS_REACHED_ERROR_STR);
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
		printErrorPrefix(); printString(COMMAND_IS_TOO_LONG_ERROR_STR);
		return GET_INPUT_STRING_COMMAND_TOO_LONG;
	}

	return ERROR_SUCCESS;
}

/**
 * Print the prompt to the user to enter a command.	
 * 
 * @param state 	[in] Current state of the game
 */
void promptUserToInput(State* state) {
	printString(getCurModeString(state)); printString(USER_PROMPT_MARKER_STR);
}

/**
 * Print an announcement that the board is successfully solved. 
 */
void announceBoardSolved() {
	printString(BOARD_SUCCESSFULLY_SOLVED_STR);
}

/**
 * Print an announcement state that the board it still erroneous.
 * 
 */
void announceBoardErroneous() {
	printString(BOARD_REMAINS_UNSOLVED_STR);
}

/**
 * Clean up all existing data in the provided state, and switch back to init mode,
 * allowing the state of a new game.
 * 
 * @param state 	[in] State to be cleaned and reset
 */
void switchToInitMode(State* state) {
	cleanupGameState(state->gameState); state->gameState = NULL;
	state->gameMode = GAME_MODE_INIT;
}

/**
 * Prints the board conrained in the provided State struct, in format according to the
 * instructions.
 * 
 * @param state 		[in] State whose board is to be printed
 * @return true 		iff procedure was successful
 * @return false 		iff a memory error occurred
 */
bool printBoard(State* state) {
	return printAllocatedString(getPuzzleAsString(state));
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
		int argsValidatorError = 0;

		promptUserToInput(state);
		errorCode = getInputString(inputStr, INPUT_STRING_MAX_LENGTH);
		if (errorCode != ERROR_SUCCESS) {
			if (errorCode == GET_INPUT_STRING_REACHED_EOF)
				loopHolds = false;
			continue;
		}

		errorCode = processStringAsCommand(state, inputStr, &command, &problematicArgNo, &argsValidatorError); /* Note: mustn't 'continue' or 'break' from this point on - cleanupCommand needs to be called in the end */
		if (errorCode != ERROR_SUCCESS) {
			printErrorPrefix();
			if (!printAllocatedString(getProcessStringAsCommandErrorString((ProcessStringAsCommandErrorCode)errorCode, problematicArgNo, state, &command, inputStr, argsValidatorError)))
				loopHolds = false;
			if (errorCode == PROCESS_STRING_AS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILURE)
				loopHolds = false;
		} else {

			errorCode = isBoardValidForCommand(state, &command);
			if (errorCode != ERROR_SUCCESS) {
				printErrorPrefix();
				if (!printAllocatedString(getIsBoardValidForCommandErrorString((IsBoardValidForCommandErrorCode)errorCode)))
					loopHolds = false;
			} else {

				errorCode = performCommand(state, &command);
				if (errorCode != ERROR_SUCCESS) {
					printErrorPrefix();
					printString(getCommandErrorString(command.type, errorCode));
					if (!isCommandErrorRecoverable(command.type, errorCode))
						loopHolds = false;
				} else {
					if (!printAllocatedString(getCommandStrOutput(&command, state->gameState)))
						loopHolds = false;

					if (shouldPrintBoardPostCommand(command.type)) {
						if (!printBoard(state))
							loopHolds = false;
					}

					if (state->gameMode == GAME_MODE_SOLVE) {
						if (isSolutionSuccessful(state->gameState)) {
							announceBoardSolved();
							switchToInitMode(state);
						} else if (isSolutionFailing(state->gameState)) {
							announceBoardErroneous();
						}
					}

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
	State state = {0}; /* Note: this is properly initialised */

	printString(GAME_HEADER_STR);

	performCommandLoop(&state);

	cleanupGameState(state.gameState); state.gameState = NULL;

	printString(EXIT_STR);
}
