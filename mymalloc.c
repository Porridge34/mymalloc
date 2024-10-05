#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mymalloc.h"

#define MEMLENGTH 4096

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

static int initialized = 0;

void setChunkSize(void *chunkStart, int chunkSize){
    *(int*)chunkStart = chunkSize;
}

int getChunkSize(void *chunkStart){
    return *(int*)chunkStart;
}

void setAllocated(void *chunkStart, int allocated){
    *(int*)(chunkStart + 4) = allocated;
}


bool isAllocated(void *chunkStart){
    return *(int*)(chunkStart + 4);
}

void leakDetector(){
    if(getChunkSize(heap.bytes) != MEMLENGTH || isAllocated(heap.bytes)){
        int chunkCounter = 0;
        int byteCounter = 0;
        void *pointer = heap.bytes;

        while(pointer < heap.bytes + MEMLENGTH){
            if(isAllocated(pointer)){
                chunkCounter++;
                byteCounter += getChunkSize(pointer);
            }
            pointer = pointer + 8 + getChunkSize(pointer);
        }

        fprintf(stderr, "mymalloc: %d bytes leaked in %d objects.", byteCounter, chunkCounter);
    }
    
}

void initializeHeap(){
    setChunkSize(heap.bytes, MEMLENGTH);
    setAllocated(heap.bytes, 0);
    initialized = 1;

    atexit(leakDetector);
}

void *mymalloc(size_t size, char *file, int line){
    if(size == 0)
    {
        // add error?
        return NULL;
    }

    size = size + (8 - (size % 8));

    void *result = NULL;
    void *current = heap.bytes;
    void *memoryEnd = heap.bytes + MEMLENGTH;

    while(current < memoryEnd){
        int chunkSize = getChunkSize(current);
        bool allocated = isAllocated(current);

        if(initialized == 0)
            initializeHeap();

        if(chunkSize >= (8 + size) && !allocated){
            setChunkSize(current, size + 8);
            setAllocated(current, 1);
            result = current + 8;
            if(chunkSize > (8 + size))
                setChunkSize(current + 8 + size, chunkSize - (8 + size)); 
            return result;
        }
        if(allocated){
            current = current + (8 + size);
        }
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s.c:%d)", size, file, line);
    return NULL;

}






