#ifndef BT_SOLVER_H
#define BT_SOLVER_H

#include <stdbool.h>
#include "game.h" /* CR: this module shouldn't know game. It should be the other way around (see LP_solver) */

bool calculateNumSolutions(GameState* gameState, int* numSolutions);

#endif /* BT_SOLVER_H */
