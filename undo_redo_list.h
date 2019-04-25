#ifndef UNDO_REDO_LIST_H
#define UNDO_REDO_LIST_H

#include <stdbool.h>

#include "linked_list.h"

typedef struct {
    List list; 
    Node* current;
    int numUndos;
} UndoRedoList;

typedef struct {
	int row;
	int col;
	int prevVal;
	int newVal;
} singleCellMove;

typedef struct {
	List singleCellMoves;
} Move;

singleCellMove* createSingleCellMove(int prevVal, int newVal, int row, int col);
bool addSingleCellMoveToMove(Move* move, int prevVal, int newVal, int row, int col);
void initUndoRedo(UndoRedoList* move_list);
bool addNewMoveToList(UndoRedoList* move_list, Move* new_move);
Move* undoInList(UndoRedoList* moveList); /* should this pass out move to undo? */
Move* redoInList(UndoRedoList* moveList);
bool reset(UndoRedoList* move_list);
Move* getCurrent(UndoRedoList* moveList);
void destroyUndoRedo(UndoRedoList* move_list);

#endif /* UNDO_REDO_LIST_H */
