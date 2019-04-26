#ifndef MOVE_H
#define MOVE_H

#include "linked_list.h"

typedef struct {
	int row;
	int col;
	int prevVal;
	int newVal;
} CellChange;

typedef struct {
	List cellChanges;
} Move;

Move* createMove();
void destroyMove(Move* move);
CellChange* createCellChange(int prevVal, int newVal, int row, int col);
/* false is returned on memory allocation error */
bool addCellChangeToMove(Move* move, int prevVal, int newVal, int row, int col);
int getMoveSize(const Move* move);
const Node* getFirstCellChange(const Move* move);
#endif /* MOVE_H */
