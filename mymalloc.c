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
    //printf("chunk size set: %d at chunk %d \n", *(int*)(&heap.bytes[chunkStart]), chunkStart); 
}

int getChunkSize(int chunkStart){
    //printf("chunk size: %d at chunk %d \n", *(int*)(&heap.bytes[chunkStart]), chunkStart); 
    return *(int*)(&heap.bytes[chunkStart]);
}

void setAllocated(int chunkStart, int allocated){
    *(int*)(&heap.bytes[chunkStart + 4]) = allocated;
    //printf("set allocated: %d at chunk %d \n", *(int*)(&heap.bytes[chunkStart + 4]), chunkStart + 4); 
}


bool isAllocated(int chunkStart){
    if(chunkStart + 4 < MEMLENGTH)
        return *(int*)(&heap.bytes[chunkStart + 4]);
    return 1;
}

void leakDetector(){
    if(getChunkSize(0) != MEMLENGTH || isAllocated(0)){
        int chunkCounter = 0;
        int byteCounter = 0;
        int pointer = 0;

        while(pointer < MEMLENGTH){
            if(isAllocated(pointer)){
                chunkCounter++;
                byteCounter += getChunkSize(pointer);
            }
            pointer = pointer + getChunkSize(pointer);
            if (pointer != MEMLENGTH && getChunkSize(pointer) == 0) {
                break;
            }
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
    if (size % 8 != 0) {
        size = size + (8 - (size % 8));
    }

    int result = 0;
    int current = 0; //start at 0th index of the heap
    int memoryEnd = MEMLENGTH;

    if(initialized == 0){
        initializeHeap();
    }

    while(current < memoryEnd){
        int chunkSize = getChunkSize(current);
        bool allocated = isAllocated(current);
            
        if(chunkSize >= (8 + size) && !allocated){
            if(chunkSize == 16 + size)
                setChunkSize(current, size + 16);
            else
                setChunkSize(current, size + 8);
            setAllocated(current, 1);
            result = current + 8;
            if(chunkSize > (8 + size))
                setChunkSize(current + 8 + size, chunkSize - (8 + size)); 
            return ((void*) &heap.bytes[result]);
        }
        current = current + chunkSize;
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s.c:%d)\n", size, file, line);
    return NULL;

}

void zeroed(int start, int end) {
    for (int i = start; i < end; i++) {
        heap.bytes[i] = 0;
    }
}

bool isAdjacentAndFree(int current, int target) {
    if (current + getChunkSize(current) == target && !isAllocated(current)) {
        return true;
    }
    return false;
}

void mergeBlocks(int first, int second) {
    setChunkSize(first, getChunkSize(first) + getChunkSize(second)); //merges the chunk size
    zeroed(first + 8, second);
    setAllocated(first, 0); //deallocates
}

int nextBlock(int ptr) {
    int next = ptr + getChunkSize(ptr);
    return next;
}

int findPtr(void* ptr){
    // int result = 0;
    // while (result < MEMLENGTH + 8) {
    //     if (&heap.bytes[result + 8] == (char*) ptr){
    //         return result;
    //     }
    //     else {
    //         result += getChunkSize(result);
    //         //printf("address found: %p to be freed: %p", ((void*) &heap.bytes[result]), ptr );
    //     }
    // }
    // return -1;
    int ptrVal = (char*)ptr - heap.bytes;
    if(ptrVal % 8 == 0)
        return ptrVal - 8;
    return -1;
}

void myfree(void *ptr, char *file, int line) {
    int current = 0;
    int toBeFreed = findPtr(ptr); //set toBeFreed to the metadata start
    //printf("to be freed: %d\n", toBeFreed);
    if (toBeFreed == -1) {
        fprintf(stderr, "free: Unable to free bytes due to invalid address(%s.c:%d)\n", file, line);
        return;
    }
    while (current < MEMLENGTH){
        if (isAdjacentAndFree(current, toBeFreed)) {
            mergeBlocks(current, toBeFreed);
            if (nextBlock(toBeFreed) < MEMLENGTH && !isAllocated(nextBlock(toBeFreed))) {
                mergeBlocks(current, nextBlock(toBeFreed));
            }
            return;
        }

        if (current == toBeFreed) {
            setAllocated(current, 0);
            if (!isAllocated(nextBlock(toBeFreed))) {
                mergeBlocks(toBeFreed, nextBlock(toBeFreed));
            }
            return;
        }
        current = nextBlock(current);
    }

    fprintf(stderr, "free: Unable to free bytes (%s.c:%d)\n", file, line);
}