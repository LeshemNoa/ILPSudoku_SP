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

bool parseIntArg(char* arg, int* dst);

bool parseIntArgOffset(char* arg, int* dst, int offset);

bool parseBooleanIntArg(char* arg, bool* dst);

bool parseStringArg(char* arg, char** dst);

bool parseDoubleArg(char* arg, double* dst);

char* getFirstToken(char* commandStr);

int splitArgumentsStringToTokens(char* commandStr, char* commandStrTokens[]);

#endif /* PARSER_H_ */
