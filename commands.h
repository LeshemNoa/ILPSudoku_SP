#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <stdbool.h>

#include "game.h"
#include "move.h"

/**
 * The Commands module is designed as an interface for the game. It's responsible for taking
 * user input, parsing it, managing commands, presenting output to the user and take care of
 * errors that may occur at any stage of the game.
 * 
 * Note: Were we given permission to use directories in this project to have its files better
 * organized, we would have split this file so that each command would have been in a file of
 * its own. For now we leave it as it is, mainly to avoid excessive inflation of the already
 * large number of files our project holds.
 * 
 */

#define ERROR_SUCCESS (0)

#define COMMAND_MAX_LENGTH (256)
#define COMMAND_END_MARKER ('\n')

/**
 * Command type strings, according to the specified format.
 * 
 */
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

/**
 * Map between the three game modes and the commands each of them allows.
 * 
 */
#define INIT_MODE_LIST_OF_ALLOWED_COMMANDS ("solve, edit, exit")
#define EDIT_MODE_LIST_OF_ALLOWED_COMMANDS ("solve, edit, print_board, set, validate, generate, undo, redo, save, num_solutions, reset, exit")
#define SOLVE_MODE_LIST_OF_ALLOWED_COMMANDS ("solve, edit, mark_errors, print_board, set, validate, guess, undo, redo, save, hint, guess_hint, num_solutions, autofill, reset, exit")

/**
 * Map between the commands and the game modes they are allowed in.
 * 
 */
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

/**
 * Usage string to output in case commands are input in wrong format. 
 * 
 */
#define SOLVE_COMMAND_USAGE ("solve <path (str)>")
#define EDIT_COMMAND_USAGE ("edit (<path (str)>)")
#define MARK_ERRORS_COMMAND_USAGE ("mark_errors <0|1 (int)>")
#define PRINT_BOARD_COMMAND_USAGE ("print_board")
#define SET_COMMAND_USAGE ("set <column_no (int)> <row_no (int)> <value (int)>")
#define VALIDATE_COMMAND_USAGE ("validate")
#define GUESS_COMMAND_USAGE ("guess <threshold (real)>")
#define GENERATE_COMMAND_USAGE ("generate <num_cells_to_fill (int)> <num_cells_to_clear (int)>")
#define UNDO_COMMAND_USAGE ("undo")
#define REDO_COMMAND_USAGE ("redo")
#define SAVE_COMMAND_USAGE ("save <path (str)>")
#define HINT_COMMAND_USAGE ("hint <column_no (int)> <row_no (int)>")
#define GUESS_HINT_COMMAND_USAGE ("guess_hint <column_no (int)> <row_no (int)>")
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

/**
 * General errors that may occur when trying to perform a command.
 * 
 */
typedef enum {
	IS_BOARD_VALID_FOR_COMMAND_BOARD_ERRONEOUS = 1,
	IS_BOARD_VALID_FOR_COMMAND_BOARD_UNSOLVABLE,
	IS_BOARD_VALID_FOR_COMMAND_CELL_HAS_FIXED_VALUE,
	IS_BOARD_VALID_FOR_COMMAND_CELL_IS_NOT_EMPTY
} IsBoardValidForCommandErrorCode;

/**
 * Errors that may occur while taking user input and processing it as a command.
 * 
 */
typedef enum ProcessStringAsCommandErrorCode {
	PROCESS_STRING_AS_COMMAND_UNKNOWN_COMMAND = 1,
	PROCESS_STRING_AS_COMMAND_COMMAND_NOT_ALLOWED_IN_CURRENT_MODE,
	PROCESS_STRING_AS_COMMAND_INCORRECT_ARGUMENTS_NUM,
	PROCESS_STRING_AS_COMMAND_ARGUMENTS_MEMORY_ALLOCATION_FAILURE,
	PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_PARSED,
	PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_IN_RANGE,
	PROCESS_STRING_AS_COMMAND_ARGUMENT_NOT_AGREEING_WITH_BOARD
} ProcessStringAsCommandErrorCode;


/* function pointer to a concrete command type's ArgParser. There is one for each command.
* @params arg				[in] the string containing the specific argument currently
* 							being parsed
* @params argGameState 		[in, out] a pointer to the concrete command argument struct
* @params argNo		 		[in] the parsed argument's index
*/
typedef bool (*commandArgsParser)(char* arg, int argNo, void* arguments);

/**
 * Function pointer to a concrete command type's RangeChecker.
 * There is one for the following commands: Set, Guess, Generate, Hint and Guess-Hint.
 * 
 */
typedef bool (*commandArgsRangeChecker)(void* arguments, int argNo, GameState* gameState);

/**
 * Function pointer to a concrete command type's ArgsValidator.
 * There currently exists only one, for Generate.
 */
typedef bool (*commandArgsValidator)(void* arguments, int argNo, GameState* gameState, int* errorCodeOut);

/**
 * Function pointer to a concrete command type's ArgsValidator.
 * The currently exists only one, for Guess-Hint.
 */
typedef void (*commandArgsCleaner)(void* arguments);


/**
 * SolveCommandArguments, EditCommandArguments, SaveCommandArguments are structs
 * that contain the argument the user provided for each of those types of commands:
 * a path to a file to save a board to or load from. 
 * 
 */
typedef struct {
	char* filePath;
} SolveCommandArguments, EditCommandArguments, SaveCommandArguments;

/**
 *  MarkErrorsCommandArguments is a struct that contains the arguments the user provided
 *  for a 'mark_errors' type command: 1 corresponds to true, 0 to false.
 */
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

/**
 * GuessCommandArguments is a struct that contains the arguments the user provided
 * for a 'guess' type command - the threshold that'll serve as the minimum score for
 * a guessed value.
 */
typedef struct {
	double threshold;
} GuessCommandArguments;

/**
 * GenerateCommandArguments is a struct that contains the arguments the user provided
 * for a 'generate' type command - the number of cells to randomly fill and the number
 * of cells to clear afterwards.
 */
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
} HintCommandArguments;

/**
 * GuessHintCommandArguments is a struct that contains the arguments the user provided
 * for a 'guess_hint' type command - the indices for the cell for which a guess is
 * requsted, along with a pointer to the score of that guess that'll determine if
 * the guess is valid or not. 
 * 
 */
typedef struct { /* Note: order of row and col is reverse to that provided by user */
	int row;
	int col;
	double* valuesScoresOut;
} GuessHintCommandArguments;

/**
 * UndoCommandArguments, RedoCommandArguments, AutofillCommandArguments are
 * structs that contain the output of these types of commands - a read-only
 * pointer to a Move struct, for further processing necessary according to 
 * the command type.
 * 
 */
typedef struct {
	const Move* movesListOut;
} UndoCommandArguments, RedoCommandArguments, AutofillCommandArguments;

/**
 * ValidateCommandArguments is a struct that contains the output of a 'validate'
 * command - true if exists a solution to the board, otherwise false.
 * 
 */
typedef struct {
	bool isSolvableOut;
} ValidateCommandArguments;

/**
 * ValidateCommandArguments is a struct that contains the output of a 'validate'
 * command - true if exists a solution to the board, otherwise false. 
 * 
 */
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
} Command;

/**
 * In case the user tries to use a command in a game mode where it's unavailable,
 * this function is used to get the string to output to them, stating the modes
 * in which this command is available.
 * 
 * @param commandType	[in] Command type for which to get the output string
 * @return char* 		String stating the command's allowing modes
 */
char* getAllowingModesString(CommandType commandType);

/**
 * Checks whether the number of arguments the user supplied is as required
 * for this type of command.
 * 
 * @param command 		[in] The command struct to be examined
 * @return true 		iff the number of arguments suit the command type
 * @return false 		otherwise
 */
bool isCorrectArgumentsNum(Command* command);

/**
 * Checks whether the command the user requested to perform is allowed in the
 * provided GameMode.
 * 
 * @param gameMode 			[in] GameMode to check command availability
 * @param commandType 		[in] Command type to check availability
 * @return true 			iff the command is available in that mode
 * @return false 			otherwise
 */
bool isCommandAllowed(GameMode gameMode, CommandType commandType);

/**
 * Get the string stating all the allowed commands in the provided GameMode
 * 
 * @param gameMode 		[in] GameMode for which allowed command string is requested
 * @return char* 		Matching allowed command string
 */
char* getAllowedCommandsString(GameMode gameMode);

/**
 * In case the user tries to use a command incorrectly, this function is used to 
 * get the string to output to them, stating the correct usage of this command.
 *  
 * @param commandType	[in] Command type for which to get the output string
 * @return char* 		String stating the command's correct usage
 */
char* getCommandUsage(CommandType commandType);

/**
 * Parses the name of the command from the user input string to create a command
 * struct with a matching type for further processing. 
 * 
 * @param commandName 	[in] The command name string as it appears in the input
 * @param commandOut 	[in, out] A pointer to be assigned with the command struct
 * 						created according with that command name
 * @return true 		iff command name identified successfully
 * @return false 		iff command name is invalid
 */
bool identifyCommandByType(char* commandName, Command* commandOut);

/**
 * Get the size in bytes of the CommandArgs struct matching the provided command type.
 * 
 * @param commandType 		[in] Command type whose Args struct size is requested
 * @return int 				size of CommandArgs struct
 */
int getSizeofCommandArgsStruct(CommandType commandType);

/**
 * Get the ArgsParser struct matching the provided command type.
 * 
 * @param commandType 			[in] Command type whose ArgsParser is requested
 * @return commandArgsParser  	ArgsParser for command or NULL if there exists none
 * 								for this command type
 */
commandArgsParser getCommandArgsParser(CommandType commandType);

/**
 * Get the RangeChecker struct matching the provided command type.
 * 
 * @param commandType 				[in] Command type whose RangeChecker is requested
 * @return commandArgsRangeChecker	RangeChecker for command or NULL if there exists none
 * 									for this command type
 */
commandArgsRangeChecker getCommandArgsRangeChecker(CommandType commandType);

/**
 * In case the user tries to input out of range command arguments, this function is
 * used to get the string to output to them, stating the correct range. 
 * 
 * @param commandType 			[in] Command type whose expected range strimg
 * 								 is requested
 * @param argNo 				[in] number of out-of-range argument
 * @param gameState 			[in] GameState context for the command
 * @return char* 				Instructive string for error to output
 */
char* getCommandArgsExpectedRangeString(CommandType commandType, int argNo, GameState* gameState);

/**
 * Get the ArgsValidtor struct matching the provided command type.
 * 
 * @param commandType 				[in] Command type whose ArgsValidtor is requested
 * @return commandArgsValidator 	ArgsValidtor for command or NULL if there exists none
 * 									for this command type
 */
commandArgsValidator getCommandArgsValidator(CommandType commandType);

/**
 * Checks whether board is valid to perform the requested command. The conditions vary
 * between commands, and they are as described in the instructions.
 * 
 * @param state 		[in] State for which the command's validity is checked
 * @param command 		[in] The command whose validity is checked
 * @return IsBoardValidForCommandErrorCode 	The result of that check, for further processing or output
 */
IsBoardValidForCommandErrorCode isBoardValidForCommand(State* state, Command* command);

/**
 * Get the string description to match the provided IsBoardValidForCommandErrorCode.
 * 
 * @param errorCode 	[in] Error code for which matching string it fetched
 * @return char* 		Output string
 */
char* getIsBoardValidForCommandErrorString(IsBoardValidForCommandErrorCode errorCode);

/**
 * Backbone function for performing commands. Takes a Command struct and redirects it
 * appropriately for execution.
 * 
 * @param state 		[in, out] State to which the command will be applied
 * @param command 		[in] Command struct to perform 
 * @return int 			The error code returned from each specific PerformCommand
 */
int performCommand(State* state, Command* command);

/**
 * Get the string description to match the provided command error according to its number and type.
 * 
 * @param type 		[in] Command type for which the error matches
 * @param error 	[in] The error number
 * @return char* 	String description of the error to output
 */
char* getCommandErrorString(CommandType type, int error);

/**
 * Checks, according to the command type and the error number, if the error that has occurred
 * is too severe to recove from and continue the game. The errors that are not recoverable are
 * memory allocation errors.
 * 
 * @param type 		[in] Command type for which the error matches
 * @param error 	[in] The error number 	
 * @return true 	iff the error is recoverable
 * @return false 	iff the error is fatal
 */
bool isCommandErrorRecoverable(CommandType type, int error);

/**
 * Get the command's string output, according to the instructions for each type of command.
 * This excludes errors - this command only gets output that is part of a valid command 
 * performance flow.
 * 
 * @param command 		[in] Command for which the output string corresonds
 * @param gameState 	[in] GameState context for the command
 * @return char* 		The appropriate output string
 */
char* getCommandStrOutput(Command* command, GameState* gameState);

/** 
 * Checks whether the board should be printed following a particular command. Command types
 * after which the board is printed are specified in the instructions.
 * 
 * @param commandType 	[in] Command type checked
 * @return true 		iff the board should be printed
 * @return false 		otherwise
 */
bool shouldPrintBoardPostCommand(CommandType commandType);

/**
 * Backbone function to process input string into a Command struct. 
 * 
 * @param state 		[in] State in which command is provided as user input
 * @param commandStr 	[in] The user input string
 * @param commandOut 	[in, out] A pointer to be assigned with the Command struct constructed
 * 						according to the input
 * @param problematicArgNo 	[in, out] Integer pointer to be assigned with the number of argument that
 * 							is found to be incorrectly supplied
 * @param argsValidatorError 	[in, out] Integer pointer to an argsValidatorError number, 
 * 								corresponding to an error that may have occurred
 * @return ProcessStringAsCommandErrorCode 	Error code
 */
ProcessStringAsCommandErrorCode processStringAsCommand(State* state, char* commandStr, Command* commandOut, int* problematicArgNo, int* argsValidatorError);

/**
 * Get the string description to match the provided ProcessStringAsCommandErrorCode.
 * 
 * @param errorCode    			[in] Error code for which matching string it fetched
 * @param problematicArgNo 		[in] Number of problematic argument in case error is in regards
 * 								to invalid input arguments
 * @param state 				[in] State in which the command was attempted
 * @param command 				[in] The command that caused the error
 * @param commandName 			[in] Command name string
 * @param argsValidatorError 	[in] argsValidatorError number, corresponding to an error that may
 * 								 have occurred
 * @return char* 				Output string
 */
char* getProcessStringAsCommandErrorString(ProcessStringAsCommandErrorCode errorCode, int problematicArgNo, State* state, Command* command, char* commandName, int argsValidatorError);

/**
 * Frees memory allocated to the provided command struct. 
 * 
 * @param command 		[in] pointer to the command to be freed
 */
void cleanupCommand(Command* command);

#endif /* COMMANDS_H_ */
