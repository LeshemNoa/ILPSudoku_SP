CC = gcc
OBJS = main.o rec_stack.o linked_list.o undo_redo_list.o main_aux.o commands.o parser.o game.o
EXEC = sudoku-console
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors -c
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS) ILP_solver_dummy.o
	$(CC) $(OBJS) ILP_solver_dummy.o -o $@ -lm	
$(EXEC)-nova: $(OBJS) ILP_solver.o
	$(CC) $(OBJS) ILP_solver.o $(GUROBI_LIB) -o $@ -lm
game.o: game.c game.h
	$(CC) $(COMP_FLAGS) $*.c
parser.o: parser.c parser.h
	$(CC) $(COMP_FLAGS) $*.c
ILP_solver.o: ILP_solver.c ILP_solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) $*.c
ILP_solver_dummy.o: ILP_solver_dummy.c ILP_solver.h
	$(CC) $(COMP_FLAGS) $*.c
commands.o: commands.c commands.h
	$(CC) $(COMP_FLAGS) $*.c
main_aux.o: main_aux.c main_aux.h
	$(CC) $(COMP_FLAGS) $*.c
main.o: main.c
	$(CC) $(COMP_FLAGS) $*.c
linked_list.o: linked_list.c linked_list.h 
	$(CC) $(COMP_FLAGS) $*.c
undo_redo_list.o: undo_redo_list.c undo_redo_list.h
	$(CC) $(COMP_FLAGS) $*.c
rec_stack.o: rec_stack.c rec_stack.h
	$(CC) $(COMP_FLAGS) $*.c

clean:
	rm -f $(OBJS) $(EXEC)
