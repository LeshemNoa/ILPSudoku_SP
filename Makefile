CC = gcc
OBJS = linked_list.o undo_redo_list.o main_aux.o commands.o parser.o game.o
EXEC = game
COMP_FLAG = -ansi -Wall -Wextra -pedantic-errors -g -c

$(EXEC): main.o $(OBJS)
	$(CC) main.o $(OBJS) -o $@
test: test.o $(OBJS)
	$(CC) test.o $(OBJS) -o test

test.o: test.c 
	$(CC) $(COMP_FLAG) $*.c
game.o: game.c game.h
	$(CC) $(COMP_FLAG) $*.c
parser.o: parser.c parser.h
	$(CC) $(COMP_FLAG) $*.c
commands.o: commands.c commands.h
	$(CC) $(COMP_FLAG) $*.c
main_aux.o: main_aux.c main_aux.h game.h parser.h commands.h
	$(CC) $(COMP_FLAG) $*.c
main.o: main.c main_aux.h
	$(CC) $(COMP_FLAG) $*.c
linked_list.o: linked_list.h linked_list.c
	$(CC) $(COMP_FLAG) $*.c
undo_redo_list.o: linked_list.h undo_redo_list.c undo_redo_list.h
	$(CC) $(COMP_FLAG) $*.c

clean:
	rm -f $(OBJS) $(EXEC) test test.o