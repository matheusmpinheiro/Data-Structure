# Makefile for executable

# *****************************************************
# Parameters to control Makefile operation

CC = gcc
CFLAGS = -Wall

# ****************************************************
# Entries to bring the executable up to date

all: series clean

series: main.o 489700_489719_ED2_T02.o
	$(CC) $(CFLAGS) -o series main.o 489700_489719_ED2_T02.o

main.o: main.c 489700_489719_ED2_T02.h
	$(CC) $(CFLAGS) -c main.c

489700_489719_ED2_T02.o: 489700_489719_ED2_T02.c 489700_489719_ED2_T02.h
	$(CC) $(CFLAGS) -c 489700_489719_ED2_T02.c

clean:
	rm -f *.o

uninstall:
	rm -f series
	rm -f *.dat