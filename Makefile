NAME=main
OBJFILES=$(NAME).o stack.o astack.o ial.o instrlist.o buildin.o interpreter.o prec.o scanner.o parser.o whattoken.o garbage.o strangen.o

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -W -g

# univerzální pravidlo pro generování všech objektových souborů
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# Startovací pravidlo
all: $(NAME)

# Slinkování všech objektových souborů do jednoho spustitelného programu.
$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

clean:
	rm -f *.o *.out $(PROGS)
