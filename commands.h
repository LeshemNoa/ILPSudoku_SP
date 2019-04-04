#ifndef COMMANDS_H_
#define COMMANDS_H_

/* TODO: consider dividing this module to its different commands (one per command), with this one being their "governor" */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "game.h"


#define ERROR_SUCCESS (0)

#define COMMAND_MAX_LENGTH (256) /* One extra character for the newline character */
#define COMMAND_END_MARKER ('\n')

#define SOLVE_COMMAND_TYPE_STRING ("solve")
#define EDIT_COMMAND_TYPE_STRING ("edit")
#define MARK_ERRORS_COMMAND_TYPE_STRING ("mark_errors")
#define PRINT_BOARD_COMMAND_TYPE_STRING ("print_board")
#define SET_COMMAND_TYPE_STRING ("set")
#define VALIDATE_COMMAND_TYPE_STRING ("validate")
#define GUESS_COMMAND_TYPE_STRING ("guess")
#define GENERATE_COMMAND_TYPE_STRING ("generate")
#define UNDO_COMMAND_TYPE_STRING ("undo")
#define REDO_COMMAND_TYPE_STRING ("redo")
#define SAVE_COMMAND_TYPE_STRING ("save")
#define HINT_COMMAND_TYPE_STRING ("hint")
#define GUESS_HINT_COMMAND_TYPE_STRING ("guess_hint")
#define NUM_SOLUTIONS_COMMAND_TYPE_STRING ("num_solutions")
#define AUTOFILL_COMMAND_TYPE_STRING ("autofill")
#define RESET_COMMAND_TYPE_STRING ("reset")
#define EXIT_COMMAND_TYPE_STRING ("exit")
#define IGNORE_COMMAND_TYPE_STRING ("ignore")

/* TODO: consider replacing this with a function that builds these strings of the above Defines */
#define INIT_MODE_LIST_OF_ALLOWED_COMMANDS ("solve, edit, exit")
#define EDIT_MODE_LIST_OF_ALLOWED_COMMANDS ("solve, edit, print_board, set, validate, generate, undo, redo, save, num_solutions, reset, exit")
#define SOLVE_MODE_LIST_OF_ALLOWED_COMMANDS ("solve, edit, mark_errors, print_board, set, validate, guess, undo, redo, save, hint, guess_hint, num_solutions, autofill, reset, exit")

/* TODO: consider replacing this with a function that builds these strings of the above Defines
 * 		 Or maybe eliminate all the repetitions */
#define SOLVE_COMMAND_LIST_OF_ALLOWING_STATES ("Init, Edit, Solve")
#define EDIT_COMMAND_LIST_OF_ALLOWING_STATES ("Init, Edit, Solve")
#define MARK_ERRORS_COMMAND_LIST_OF_ALLOWING_STATES ("Solve")
#define PRINT_BOARD_COMMAND_LIST_OF_ALLOWING_STATES ("Edit, Solve")
#define SET_COMMAND_LIST_OF_ALLOWING_STATES ("Edit, Solve")
#define VALIDATE_COMMAND_LIST_OF_ALLOWING_STATES ("Edit, Solve")
#define GUESS_COMMAND_LIST_OF_ALLOWING_STATES ("Solve")
#define GENERATE_COMMAND_LIST_OF_ALLOWING_STATES ("Edit")
#define UNDO_COMMAND_LIST_OF_ALLOWING_STATES ("Edit, Solve")
#define REDO_COMMAND_LIST_OF_ALLOWING_STATES ("Edit, Solve")
#define SAVE_COMMAND_LIST_OF_ALLOWING_STATES ("Edit, Solve")
#define HINT_COMMAND_LIST_OF_ALLOWING_STATES ("Solve")
#define GUESS_HINT_COMMAND_LIST_OF_ALLOWING_STATES ("Solve")
#define NUM_SOLUTIONS_COMMAND_LIST_OF_ALLOWING_STATES ("Edit, Solve")
#define AUTOFILL_COMMAND_LIST_OF_ALLOWING_STATES ("Solve")
#define RESET_COMMAND_LIST_OF_ALLOWING_STATES ("Edit, Solve")
#define EXIT_COMMAND_LIST_OF_ALLOWING_STATES ("Init, Edit, Solve")

#define SOLVE_COMMAND_USAGE ("solve <path>")
#define EDIT_COMMAND_USAGE ("edit (<path>)")
#define MARK_ERRORS_COMMAND_USAGE ("mark_errors <0|1>")
#define PRINT_BOARD_COMMAND_USAGE ("print_board")
#define SET_COMMAND_USAGE ("set <column_no> <row_no> <value>")
#define VALIDATE_COMMAND_USAGE ("validate")
#define GUESS_COMMAND_USAGE ("guess <threshold>")
#define GENERATE_COMMAND_USAGE ("generate <num_cells_to_fill> <num_cells_to_clear>")
#define UNDO_COMMAND_USAGE ("undo")
#define REDO_COMMAND_USAGE ("redo")
#define SAVE_COMMAND_USAGE ("save <path>")
#define HINT_COMMAND_USAGE ("hint <column_no> <row_no>")
#define GUESS_HINT_COMMAND_USAGE ("guess_hint <column_no> <row_no>")
#define NUM_SOLUTIONS_COMMAND_USAGE ("num_solutions")
#define AUTOFILL_COMMAND_USAGE ("autofill")
#define RESET_COMMAND_USAGE ("reset")
#define EXIT_COMMAND_USAGE ("exit")


/**
 * commandType keeps all the possible values for the type attribute of a command struct,
 * and an additional IGNORE value, to be used when the command name the user provided
 * contained only whitespaces.
 */
typedef enum commandType {
	COMMAND_TYPE_SOLVE,
	COMMAND_TYPE_EDIT,
	COMMAND_TYPE_MARK_ERRORS,
	COMMAND_TYPE_PRINT_BOARD,
	COMMAND_TYPE_SET,
	COMMAND_TYPE_VALIDATE,
	COMMAND_TYPE_GUESS,
	COMMAND_TYPE_GENERATE,
	COMMAND_TYPE_UNDO,
	COMMAND_TYPE_REDO,
	COMMAND_TYPE_SAVE,
	COMMAND_TYPE_HINT,
	COMMAND_TYPE_GUESS_HINT,
	COMMAND_TYPE_NUM_SOLUTIONS,
	COMMAND_TYPE_AUTOFILL,
	COMMAND_TYPE_RESET,
	COMMAND_TYPE_EXIT,
	COMMAND_TYPE_IGNORE} CommandType;

typedef enum {
	IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS = 1,
	IS_BOARD_VALID_FOR_COMMAND_NO_MOVE_TO_UNDO, /* TODO: undo and redo probably shouldn't count here as "board invalid" for command */
	IS_BOARD_VALID_FOR_COMMAND_NO_MOVE_TO_REDO,
	IS_BOARD_VALID_FOR_COMMAND_BOARD_UNSOLVABLE,
	IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_ERRONEOUS_VALUE,
	IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_FIXED_VALUE,
	IS_BOARD_VALID_FOR_COMMAND_CELL_IS_NOT_EMPTY
} IsBoardValidForCommandErrorCode;

typedef enum ProcessStringAsCommandErrorCode {
	PROCESS_STRING_AS_COMMAND_UNKNOWN_COMMAND = 1,
	PROCESS_STRING_AS_COMMAND_COMMAND_NOT_ALLOWED_IN_CURRENT_MODE,
	PROCESS_STRING_AS_COMMAND_INCORRECT_ARGUMENTS_NUM,
	PROCESS_STRING_AS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILURE,
	PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_PARSED,
	PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_IN_RANGE,
	PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_AGREEING_WITH_BOARD
} ProcessStringAsCommandErrorCode;


/* function pointer to a concrete command type's ArgParser. There currently are 2 of these:
* setArgsParser and hintArgsParser.
* @params arg			[in] the string containing the specific argument currently
 * 						being parsed
* @params argGameState 		[in, out] a pointer to the concrete command argument struct
* @params argNo		 	[in] the parsed argument's index
*/
typedef bool (*commandArgsParser)(char* arg, int argNo, void* arguments);

typedef bool (*commandArgsRangeChecker)(void* arguments, int argNo, GameState* gameState);

typedef bool (*commandArgsValidator)(void* arguments, int argNo, GameState* gameState);

typedef void (*commandArgsCleaner)(void* arguments);


typedef struct {
	char* filePath;
} SolveCommandArguments, EditCommandArguments, SaveCommandArguments;

typedef struct {
	bool shouldMarkError;
} MarkErrorsCommandArguments;

/**
 * SetCommandArguments is a struct that contains the arguments the user provided
 * for a 'set' type command - the value to set, and the column and row number of the cell
 * to set this value in.
 */
typedef struct { /* Note: order of row and col is reverse to that provided by user */
	int row;
	int col;
	int value;
} SetCommandArguments;

typedef struct {
	float threshold; /* TODO: perhaps should be double? */
} GuessCommandArguments;

typedef struct {
	int numEmptyCellsToFill;
	int numCellsToClear;
} GenerateCommandArguments;

/**
 * HintCommandArguments is a struct that contains the arguments the user provided
 * for a 'hint' type command - the column and row number of the cell the user requests a
 * hint for.
 */
typedef struct { /* Note: order of row and col is reverse to that provided by user */
	int row;
	int col;
	int guessedValueOut;
} HintCommandArguments, GuessHintCommandArguments;

typedef struct {
	void* movesListOut;
} UndoCommandArguments, RedoCommandArguments;

typedef struct {
	bool isSolvableOut;
} ValidateCommandArguments;

typedef struct {
	int numSolutionsOut;
} NumSolutionsCommandArguments;

/**
 * For consistency, we use an minimal size struct to respresent the arguments of
 * commands with no arguments: 'validate', 'restart' and 'exit'
 */
typedef struct {
	char dummy;
} PrintBoardCommandArguments,
  AutofillCommandArguments,
  ResetCommandArguments,
  ExitCommandArguments,
  IgnoreCommandArguments;


#define COMMAND_ARGUMENTS_MAX_SIZE (sizeof(SolveCommandArguments) + \
									sizeof(EditCommandArguments) + \
									sizeof(MarkErrorsCommandArguments) + \
									sizeof(SetCommandArguments) + \
									sizeof(GuessCommandArguments) + \
									sizeof(GenerateCommandArguments) + \
									sizeof(SaveCommandArguments) + \
									sizeof(HintCommandArguments) + \
									sizeof(GuessHintCommandArguments) + \
									sizeof(PrintBoardCommandArguments) + \
									sizeof(ValidateCommandArguments) + \
									sizeof(UndoCommandArguments) + \
									sizeof(RedoCommandArguments) + \
									sizeof(NumSolutionsCommandArguments) + \
									sizeof(AutofillCommandArguments) + \
									sizeof(ResetCommandArguments) + \
									sizeof(ExitCommandArguments) + \
									sizeof(IgnoreCommandArguments))


/**
 * Command is a struct that represents the user's command. It has two attributes -
 * the command's type, matching one of the types defined in the commandType enum,
 * and the command's arguments, a generic pointer to the concrete arguments struct.
 */
typedef struct {
	CommandType type;
	int argumentsNum;
	void* arguments;
	/*har arguments[COMMAND_ARGUMENTS_MAX_SIZE];*/

} Command;

char* getAllowingModesString(CommandType commandType);

bool isCorrectArgumentsNum(Command* command);

bool isCommandAllowed(GameMode gameMode, CommandType commandType);

char* getAllowedCommandsString(GameMode gameMode);

char* getCommandUsage(CommandType commandType);

bool identifyCommandByType(char* commandName, Command* commandOut);

int getSizeofCommandArgsStruct(CommandType commandType);

commandArgsParser getCommandArgsParser(CommandType commandType);

commandArgsRangeChecker getCommandArgsRangeChecker(CommandType commandType);

commandArgsValidator getCommandArgsValidator(CommandType commandType);

IsBoardValidForCommandErrorCode isBoardValidForCommand(State* state, Command* command);

int performCommand(State* state, Command* command);

bool shouldPrintBoardPostCommand(CommandType commandType);

ProcessStringAsCommandErrorCode processStringAsCommand(State* state, char* commandStr, Command* commandOut, int* problematicArgNo);

void cleanupCommand(Command* command);

#endif /* COMMANDS_H_ */
