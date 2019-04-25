#ifndef MOVE_H
#define MOVE_H

/* CR: this should simply be a part of undo_redo_list module (is there any reason for it being independent of it? */

#include "linked_list.h"

typedef struct {
	int row;
	int col;
	int prevVal;
	int newVal;
} singleCellMove; /* CR: since you use the word Move in many contexts, I advise this to be changed to cellChange (this isn't ambiguous, as far as I can tell) */

typedef struct {
	List singleCellMoves;
} Move;

#endif /* MOVE_H */
