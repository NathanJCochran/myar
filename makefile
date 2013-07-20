CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall 
PROGS=myar

all: $(PROGS)

myar: myar.o
	$(CC) $(CFLAGS) -o $@ $^

myar.o: myar.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f $(PROGS) *.o *~

