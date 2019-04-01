#include "parser.h"

#define UNUSED(x) (void)(x)

#define COMMAND_DELIMITERS " \t\r\n"

/* 
* Reserved for future use. 
*
* function pointer to a concrete implementation of a command cleaner of a specific
* command type.
* 
* @params argGameState	a generic pointer to the concrete command argument struct
* 
*
*/
typedef void (*commandArgsCleaner)(void* argsGameState);

/**
 * parseIntArg parses a string representing an integer into an integer, then
 * assigns it to the provided integer pointer. 
 * 
 * @param arg		[in] the string to be parsed 
 * @param dst 		[in, out] a pointer to an integer to be assigned with
 * 					the parse result
 * @return true		iff parsing was successful 
 * @return false 	iff provided input was not entirely composed of digits
 */
bool parseIntArg(char* arg, int* dst) {
	int sscanfRetVal = sscanf(arg, "%d", dst);
	return (sscanfRetVal == 1);
}

bool parseBooleanIntArg(char* arg, bool* dst) {
	if (strlen(arg) != 1)
		return false;

	if (arg[0] == '0')
		*dst = false;
	else if (arg[0] == '1')
		*dst = true;
	else
		return false;

	return true;
}

bool parseStringArg(char* arg, char** dst) {
	char* p;

	if (strlen(arg) == 0)
		return false;

	/* validate that arg is all digits */
	for (p = arg; *p != '\0'; p++)
		if (isprint(*p) == 0)
			return false;

	*dst = arg; /* Note: not copied! (this is OK) */
	return true;
}

bool parseFloatArg(char* arg, float* dst) {
	int sscanfRetVal = sscanf(arg, "%f", dst);
	return (sscanfRetVal == 1);
}

/*void cleanupCommand(Command* command) {
	commandArgsCleaner cleaners[] = {NULL, NULL, NULL, NULL, NULL, NULL}; *//* No cleanup is necessary for any command yet */

	/*if (command == NULL || command->arguments == NULL) {
		return;
	}

	if (cleaners[command->type] != NULL) {
		cleaners[command->type](command->arguments);
	}

	free(command->arguments);
}*/

char* getFirstToken(char* commandStr) {
	return strtok(commandStr, COMMAND_DELIMITERS);
}

int splitArgumentsStringToTokens(char* commandStr, char* commandStrTokens[]) {
	int argsNum = 0;
	char* arg = NULL;

	UNUSED(commandStr); /* It seems bizarre not to have this parameter, so we keep it even
		if it isn't used explicitly (it's used implicitly by strtok) */

	while (true) {
		arg = strtok(NULL, COMMAND_DELIMITERS);
		if (arg == NULL)
			break;

		commandStrTokens[argsNum] = arg;
		argsNum++;
	}

	return argsNum;
}