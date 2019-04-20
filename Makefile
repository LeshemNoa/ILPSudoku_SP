CC = gcc
OBJS = main.o main_aux.o commands.o parser.o game.o ILP_solver.o

EXEC = sudoku-console
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
game.o: game.c game.h
	$(CC) $(COMP_FLAGS) -c $*.c
parser.o: parser.c parser.h
	$(CC) $(COMP_FLAGS) -c $*.c
ILP_solver.o: ILP_solver.c ILP_solver.h game.o
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
commands.o: commands.c commands.h parser.o game.o ILP_solver.o
	$(CC) $(COMP_FLAGS) -c $*.c
main_aux.o: main_aux.c main_aux.h game.o parser.o commands.o
	$(CC) $(COMP_FLAGS) -c $*.c
main.o: main.c main_aux.o
	$(CC) $(COMP_FLAGS) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
