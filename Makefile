CC = gcc
OBJS = main.o stack.o linked_list.o undo_redo_list.o main_aux.o commands.o parser.o game.o board.o move.o BT_solver.o
EXEC = sudoku-console
EXEC_LOCAL = $(EXEC)-local
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors

# Uncomment for Mac OS X support:
#UNAME_S = $(shell uname -s)
#ifeq ($(UNAME_S),Darwin) # if Mac OS X
#	GUROBI_COMP = -I/Library/gurobi563/mac64/include
#	GUROBI_LIB = -L/Library/gurobi563/mac64/lib -lgurobi56
#endif

$(EXEC): $(OBJS) LP_solver.o
	$(CC) $(OBJS) LP_solver.o $(GUROBI_LIB) -o $@ -lm
$(EXEC_LOCAL): $(OBJS) LP_solver_dummy.o
	$(CC) $(OBJS) LP_solver_dummy.o -o $@ -lm
move.o: move.c move.h linked_list.h
	$(CC) $(COMP_FLAGS) -c $*.c
board.o: board.c board.h LP_solver.h
	$(CC) $(COMP_FLAGS) -c $*.c
game.o: game.c game.h board.h undo_redo_list.h move.h
	$(CC) $(COMP_FLAGS) -c $*.c
parser.o: parser.c parser.h
	$(CC) $(COMP_FLAGS) -c $*.c
BT_solver.o: BT_solver.c BT_solver.h board.h
	$(CC) $(COMP_FLAGS) -c $*.c
LP_solver.o: LP_solver.c LP_solver.h board.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
LP_solver_dummy.o: LP_solver_dummy.c LP_solver.h board.h
	$(CC) $(COMP_FLAGS) -c $*.c
commands.o: commands.c commands.h game.h move.h board.h parser.h BT_solver.h LP_solver.h
	$(CC) $(COMP_FLAGS) -c $*.c
main_aux.o: main_aux.c main_aux.h commands.h
	$(CC) $(COMP_FLAGS) -c $*.c
main.o: main.c main_aux.h
	$(CC) $(COMP_FLAGS) -c $*.c
stack.o: stack.c stack.h linked_list.h
	$(CC) $(COMP_FLAGS) -c $*.c
linked_list.o: linked_list.c linked_list.h 
	$(CC) $(COMP_FLAGS) -c $*.c
undo_redo_list.o: undo_redo_list.c undo_redo_list.h move.h linked_list.h
	$(CC) $(COMP_FLAGS) -c $*.c

all: $(EXEC) $(EXEC_LOCAL)
clean:
	rm -f $(OBJS) LP_solver_dummy.o LP_solver.o $(EXEC) $(EXEC_LOCAL)
