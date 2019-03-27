/**
 * PARSER Summary:
 *
 * A module designed to parse user commands for a sudoku game.
 *
 * cleanupCommand - cleans up any resources allocated for a previously parsed user command
 * parseCommand - parses a user command
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

bool parseIntArg(char* arg, int* dst);

bool parseBooleanIntArg(char* arg, bool* dst);

bool parseStringArg(char* arg, char** dst);

bool parseFloatArg(char* arg, float* dst);

/**
 * Reserved for future use.
 *
 * cleanupCommand frees memory allocated by parseCommand. In case one of the command types
 * will need to allocate additional internal memory, there will be a specific cleanup
 * implementation provided.
 *
 * @param command		the Command struct whose arguments are removed
 */
/*void cleanupCommand(Command* command); *//* TODO: is needed? */

char* getFirstToken(char* commandStr);

int splitArgumentsStringToTokens(char* commandStr, char* commandStrTokens[]);

#endif /* PARSER_H_ */
