CC = gcc
OBJS = main.o rec_stack.o linked_list.o undo_redo_list.o main_aux.o commands.o parser.o game.o board.o
EXEC = sudoku-console
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS) LP_solver_dummy.o
	$(CC) $(OBJS) LP_solver_dummy.o -o $@ -lm
$(EXEC)-nova: $(OBJS) LP_solver.o
	$(CC) $(OBJS) LP_solver.o $(GUROBI_LIB) -o $@ -lm
board.o: board.c board.h
	$(CC) $(COMP_FLAGS) -c $*.c
game.o: game.c game.h
	$(CC) $(COMP_FLAGS) -c $*.c
parser.o: parser.c parser.h
	$(CC) $(COMP_FLAGS) -c $*.c
LP_solver.o: LP_solver.c LP_solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
LP_solver_dummy.o: LP_solver_dummy.c LP_solver.h
	$(CC) $(COMP_FLAGS) -c $*.c
commands.o: commands.c commands.h
	$(CC) $(COMP_FLAGS) -c $*.c
main_aux.o: main_aux.c main_aux.h
	$(CC) $(COMP_FLAGS) -c $*.c
main.o: main.c
	$(CC) $(COMP_FLAGS) -c $*.c
linked_list.o: linked_list.c linked_list.h 
	$(CC) $(COMP_FLAGS) -c $*.c
undo_redo_list.o: undo_redo_list.c undo_redo_list.h
	$(CC) $(COMP_FLAGS) -c $*.c
rec_stack.o: rec_stack.c rec_stack.h
	$(CC) $(COMP_FLAGS) -c $*.c

clean:
	rm -f $(OBJS) LP_solver_dummy.o ILP_solver.o $(EXEC)
