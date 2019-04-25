#ifndef MOVE_H
#define MOVE_H

#include "linked_list.h"

typedef struct {
	int row;
	int col;
	int prevVal;
	int newVal;
} singleCellMove;

typedef struct {
	List singleCellMoves;
} Move;

#endif /* MOVE_H */
