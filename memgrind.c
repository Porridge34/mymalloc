#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>


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

void firstTest(){
    char *obj;
    for(int i = 0; i < 120; i++){
        obj = malloc(1);
        free(obj);
    }
}

void secondTest(){
    char *arr[120];
    for(int i = 0; i < 120; i++){
        arr[i] = malloc(1);
    }
    for(int i = 0; i < 120; i++){
        free(arr[i]);
    }
}

void thirdTest(){
    char *arr[120];
    int c = 0;
    for (int i = 0; i < 120; i++){
        arr[i] = NULL;
    }
        while(c < 120){
            if(rand() % 2 == 0 || c == 0){
                arr[c] = malloc(1);
                c++;
            }
            else{
                for(int j = 0; j < c; j++){
                    if(arr[j] != NULL){
                        free(arr[j]);
                        arr[j] = NULL;
                        break;
                    }
                }
            }
        }
    
    for (int i = 0; i < 120; i++){
        if(arr[i] != NULL){
            free(arr[i]);
        }
    }
}

void fourthTest(){
    char *arr[10];
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 10; j++){
            arr[j] = malloc(1);
        }
        for(int j = 0; j < 10; j++){
            free(arr[j]);
            arr[j] = NULL;
        }
    }

}

void fifthTest(){
    char *arr[120];
    for(int i = 0; i < 120; i++){
        arr[i] = malloc(8);
        if(i % 4 == 1){
            free(arr[i]);
            arr[i] = NULL;
        }
    }
    for (int i = 0; i < 120; i++){
        if(arr[i] != NULL){
            free(arr[i]);
        }
    }
}

int main(){
    
    struct timeval tvStart;
    struct timeval tvEnd;

    gettimeofday(&tvStart, NULL);
    for(int i = 0; i < 50; i++){
        firstTest();
        secondTest();
        thirdTest();
        fourthTest();
        fifthTest();
    }
    gettimeofday(&tvEnd, NULL);
    if(tvStart.tv_sec==tvEnd.tv_sec)
        printf("Elapsed Time: %ld microseconds\n",(tvEnd.tv_usec-tvStart.tv_usec));
    else
        printf("Elapsed Time: %ld microseconds\n",(tvEnd.tv_sec-tvStart.tv_sec-1)*1000000+(1000000-tvStart.tv_usec)+tvEnd.tv_usec);
    return EXIT_SUCCESS;
}
