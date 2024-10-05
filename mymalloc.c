#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mymalloc.h"

#define MEMLENGTH 4096

static union {
char bytes[MEMLENGTH];
double not_used;
} heap;

void *mymalloc(size_t size, char *file, int line){
    if(size == 0)
    {
        //add error
        return NULL;
    }

    size = size + (8 - (size % 8));

    void *result = NULL;
    void *current = heap.bytes;
    void *memoryEnd = heap.bytes + MEMLENGTH;

    while(current < memoryEnd){
        int chunkSize = getChunkSize(current);
        bool allocated = isAllocated(current);

        if(chunkSize == 0 && !allocated){
            setChunkSize(current, size + 8);
            setAsAllocated(current);
            result = current + 8;
            setChunkSize(current + 8 + size, memoryEnd - (current + 8 + size)); //next chunk size
            return result;
        }
        if(chunkSize >= (8 + size) && !allocated){
            setChunkSize(current, size + 8);
            setAsAllocated(current);
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

void setChunkSize(void *chunkStart, int chunkSize){
    *(int*)chunkStart = chunkSize;
}

int getChunkSize(void *chunkStart){
    return *(int*)chunkStart;
}

void setAsAllocated(void *chunkStart){
    *(int*)(chunkStart + 4) = 1;
}

bool isAllocated(void *chunkStart){
    return *(int*)(chunkStart + 4);
}



