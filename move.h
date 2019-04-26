#ifndef MOVE_H
#define MOVE_H

/* CR+: this should simply be a part of undo_redo_list module (is there any reason for it being independent of it? */
/* CR Response: Commands module needs to know Move but doesn't need to know the undo list maintained by game */
/* CR: sounds legit! */
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
int getCellChangesSize(const Move* move); /* CR: or perhaps getMoveSize? */
Node* getFirstCellChange(const Move* move);
#endif /* MOVE_H */
