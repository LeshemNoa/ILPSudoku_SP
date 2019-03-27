#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SPBufferset.h"
#include "main_aux.h"

int main(int argc, char** argv) {
	SP_BUFF_SET();
	
	(void)argc; /* TODO: delete these */
	(void)argv;

	srand(time(NULL));

	runGame();

	return 0;
}
