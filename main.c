/**
 * MAIN Summary:
 *
 * A simple module, running the Sudoku game
 */

#include <stdlib.h>
#include <time.h>

#include "SPBufferset.h"
#include "main_aux.h"

#define UNUSED(x) (void)(x)

int main(int argc, char** argv) {
	SP_BUFF_SET();
	
	UNUSED(argc);
	UNUSED(argv);

	srand(time(NULL));

	runGame();

	return 0;
}
