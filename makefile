CC = gcc
CFLAGS = -std=c99 -Wall -Wvla -fsanitize=address,undefined
memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) memtest.o mymalloc.o -o memtest
memtest.o: memtest.c
	$(CC) $(CFLAGS) -c memtest.c
mymalloc.o: mymalloc.c
	$(CC) $(CFLAGS) -c mymalloc.c