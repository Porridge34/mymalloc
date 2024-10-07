#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#include "mymalloc.h"
#endif

// Compile with -DLEAK to leak memory
#ifndef LEAK
#define LEAK 0
#endif

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

int method1() {
	char *obj[OBJECTS];
	int i, j, errors = 0;
	
	// fill memory with objects
	for (i = 0; i < OBJECTS; i++) {
		// printf("allocating int: %d", i);
		obj[i] = malloc(OBJSIZE);
		if (obj[i] == NULL) {
		    printf("Unable to allocate object %d\n", i);
		    exit(1);
		}
	}
	
	// fill each object with distinct bytes
	for (i = 0; i < OBJECTS; i++) {
		memset(obj[i], i, OBJSIZE);
	}
	
	// check that all objects contain the correct bytes
	for (i = 0; i < OBJECTS; i++) {
		for (j = 0; j < OBJSIZE; j++) {
			if (obj[i][j] != i) {
				errors++;
				printf("Object %d byte %d incorrect: %d\n", i, j, obj[i][j]);
			}
		}
	}

	// free all objects
	if (!LEAK) {
	    for (i = 0; i < OBJECTS; i++) {
		free(obj[i]);
	    }
	}
	return errors;
}

int method2() {
	int errors = 0;
	//allocate and deallocate things randomly
	int* testArray[100];
	for (int i = 0; i < 100; i++) {
		testArray[i] = malloc(rand() % 24 + 1);
		if (!testArray[i]) {
			errors++;
		}
		if (rand() % 10 && i > 2) {
			int random = rand() % i;
			if (testArray[random]) {
				free(testArray[random]);
				testArray[random] = NULL;
			}
		}
	}

	//free the rest of the elements
	if (!LEAK) {
		for (int i = 0; i < 100; i++) {
			if (testArray[i]) {
				free(testArray[i]);
			}
		}
	}
	return errors;
}

int main(int argc, char **argv)
{
	int errors = method1();
	errors += method2();
	for (int i = 0; i < 20; i++) {
		errors += method2();
	}
	
	//memory should be empty, so I should  be able to allocate 1 very big object
	char* big = malloc(MEMSIZE - HEADERSIZE);
	free(big);

	//should cause errors
	char* tooBig = malloc(3345435543);
	if (tooBig != NULL) {
		errors++;
	}

	char* none;
	free(none);

	printf("%d incorrect bytes\n", errors);
	
	return EXIT_SUCCESS;
}
