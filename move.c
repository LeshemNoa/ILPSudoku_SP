#include "move.h"
#include <stdlib.h>

Move* createMove() {
    Move* move = (Move*) calloc(1, sizeof(Move));
	if (move == NULL) { return NULL; }

    initList(&(move->cellChanges));
    return move;
}

void destroyMove(Move* move) {
    if (move == NULL) { return; }
    while (!isListEmpty(&(move->cellChanges))) {
        CellChange* change = (CellChange*) popList(&(move->cellChanges));
        free(change);
    }
    free(move);
}

CellChange* createCellChange(int prevVal, int newVal, int row, int col){
    CellChange* change = (CellChange*) calloc(1, sizeof(CellChange));
    if (change == NULL) { return NULL; }
    change->col = col;
    change->row = row;
    change->prevVal = prevVal;
    change->newVal = newVal;
    return change;
}

/* false is returned on memory allocation error */
bool addCellChangeToMove(Move* move, int prevVal, int newVal, int row, int col) {
    CellChange* change = createCellChange(prevVal, newVal, row, col);
    if (change == NULL) { return false; }
    if (!pushList(&(move->cellChanges), change)) {
        free(change);
        return false;
    }
    return true;
}

int getMoveSize(const Move* move) {
    return getListSize(&(move->cellChanges));
}

const Node* getFirstCellChange(const Move* move) {
    return getListHead(&(move->cellChanges));
}
