#ifndef UNDO_REDO_LIST_H
#define UNDO_REDO_LIST_H
#include "linked_list.h"
#include "game.h"

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

singleCellMove* createSingleCellMove(int prevVal, int newVal, int col, int row);
bool addSingleCellMoveToMove(Move* move, int prevVal, int newVal, int col, int row);
void initUndoRedo(UndoRedoList* move_list);
bool makeMove(UndoRedoList* move_list, Move* new_move);
bool undo(UndoRedoList* move_list); /* should this pass out move to undo? */
bool redo(UndoRedoList* move_list);
bool reset(UndoRedoList* move_list);
void* getCurrent(UndoRedoList* move_list);
void destroyUndoRedo(UndoRedoList* move_list);

#endif /* UNDO_REDO_LIST_H */
