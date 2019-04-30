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
/* The parser module contains some general parsing functions for different types of data
types. */

/**
 * Parse provided string to an integer.
 * 
 * @param arg       [in] String to parse
 * @param dst       [in, out] Pointer to an integer, to be assigned with the parse result
 * @return true     iff an integer was successfully parsed
 * @return false    iff the parse was unsuccessful
 */
bool parseIntArg(char* arg, int* dst);

/**
 * Parse provided string to an integer, with provided offset.
 * For example, if the input string is "x" and the offset is -y, the parse result is x-y.
 * 
 * @param arg       [in] String to parse
 * @param dst       [in, out] Pointer to an integer, to be assigned with the parse result
 * @param offset    [in] How much to offset the parse result
 * @return true     iff an integer was successfully parsed
 * @return false    iff the parse was unsuccessful
 */
bool parseIntArgOffset(char* arg, int* dst, int offset);

/**
 * Parse provided string, containg the integer 1 or the integer 0, to a boolean.
 * 
 * @param arg       [in] String to parse
 * @param dst       [in, out] Pointer to a boolean, to be assigned with the parse result
 * @return true     iff a boolean was successfully parsed
 * @return false    iff the parse was unsuccessful
 */
bool parseBooleanIntArg(char* arg, bool* dst);

/**
 * Validates the provided string argument is comprised entirely of printable chracters,
 * and assigns it to the provided pointer. 
 * 
 * @param arg       [in]  String to be parsed
 * @param dst       [in, out] Pointer to a string to be assigned with the parsed string
 * @return true     iff input string only contains printable characters
 * @return false    otherwise
 */
bool parseStringArg(char* arg, char** dst);

/**
 * Parse provided string to a double.
 * 
 * @param arg       [in] String to parse
 * @param dst       [in, out] Pointer to a double, to be assigned with the parse result
 * @return true     iff a double was successfully parsed
 * @return false    iff the parse was unsuccessful
 */
bool parseDoubleArg(char* arg, double* dst);

/**
 * Get the first token in the provided string, where tokens are separated by spaces.
 *  
 * @param arg       [in] String to parse
 * @return char*    The first token in the string
 */
char* getFirstToken(char* commandStr);

int splitArgumentsStringToTokens(char* commandStr, char* commandStrTokens[]);

#endif /* PARSER_H_ */
