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
    if (!pushList(&(move->cellChanges), change)) { /* CR: this should be a call to some clearList function, I feel */
        free(change);
        return false;
    }
    return true;
}

int getCellChangesSize(const Move* move) {
    return getListSize(&(move->cellChanges));
}

Node* getFirstCellChange(const Move* move) { /* CR: seems like the returned value should be a const pointer, too... (and also perhaps in all those functions in list which return nodes (aka getListHead...) */
    return getListHead(&(move->cellChanges));
}
