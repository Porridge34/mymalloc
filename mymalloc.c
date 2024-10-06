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

void setChunkSize(int chunkStart, int chunkSize){
    *(int*)(&heap.bytes[chunkStart]) = chunkSize;
}

int getChunkSize(int chunkStart){
    // printf("chunk size: %d \n", *(int*)chunkStart); 
    return *(int*)(&heap.bytes[chunkStart]);
}

void setAllocated(int chunkStart, int allocated){
    *(int*)(&heap.bytes[chunkStart + 4]) = allocated;
}


bool isAllocated(int chunkStart){
    return *(int*)(&heap.bytes[chunkStart + 4]);
}

void leakDetector(){
    if(getChunkSize(heap.bytes) != MEMLENGTH || isAllocated(heap.bytes)){
        int chunkCounter = 0;
        int byteCounter = 0;
        int pointer = 0;

        while(pointer < MEMLENGTH){
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
    setChunkSize(0, MEMLENGTH);
    setAllocated(0, 0);
    initialized = 1;

    atexit(leakDetector);
}

void *mymalloc(size_t size, char *file, int line){
    // printf("allocating size %ld ", size);
    if(size == 0)
    {
        // add error?
        return NULL;
    }

    size = size + (8 - (size % 8));

    int result = 0;
    int current = 0; //start at 0th index of the heap
    int memoryEnd = MEMLENGTH;

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
            return ((void*) &heap.bytes[result]);
        }
        if(allocated){
            current = current + (8 + size);
        }
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s.c:%d)", size, file, line);
    return NULL;

}

bool isAdjacentAndFree(int current, int target) {
    if (current + getChunkSize(current) + 8 == target && isAllocated(current)) {
        return true;
    }
    return false;
}

void mergeBlocks(int first, int second) {
    setChunkSize(first, getChunkSize(first) + getChunkSize(second) + 8); //merges the chunk size
    setAllocated(first, 0); //deallocates
}

int nextBlock(int ptr) {
    int next = ptr + 8 + getChunkSize(ptr);
    return next;
}

int findPtr(void* ptr){
    int result = 0;
    while (result <= MEMLENGTH) {
        if (&heap.bytes[result + 8] == (char*) ptr){
            return result;
        }
        else {
            result += getChunkSize(result) + 8;
        }
    }
    return -1;
}

void myfree(void *ptr, char *file, int line) {
    printf("freeing");
    int current = 0;
    int toBeFreed = findPtr(ptr); //set toBeFreed to the metadata start
    if (toBeFreed == -1) {
        fprintf(stderr, "free: Unable to free bytes due to invalid address(%s.c:%d)", file, line);
        return;
    }
    while (current <= MEMLENGTH){
        if (isAdjacentAndFree(current, ptr)) {
            mergeBlocks(current,ptr);
            if (isAllocated(nextBlock(ptr))) {
                mergeBlocks(current, nextBlock(ptr));
            }
            return;
        }

        if (current == ptr) {
            if (isAllocated(nextBlock(ptr))) {
                mergeBlocks(ptr, nextBlock(ptr));
            }
            return;
        }
        current = nextBlock(current);
    }

    fprintf(stderr, "free: Unable to free bytes (%s.c:%d)", file, line);
}