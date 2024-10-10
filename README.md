# mymalloc
Names:
Neha Jeyaram (nj366)
Ivy Ng (iyn2)

We've built a variety of tests for our project to test all functions of our program. The first test that we have created is one that creates an array of objects. Each object is sized exactly so that malloc's memory is full, then each object is freed. Secondly, we have a test where we have an array of size 100 that randomly allocates and deallocates items a hundred times, then frees any remaining objects. This second test is repeated 20 times. Afterwards, we malloc 1 very big object that fills exactly the amount of memory that is available. In case of failure to deallocate something, the leak detector can clean up any bytes that have been left out to prevent errors.

We also have some error tests. There is a test that tries to malloc more memory than is available, a test that tries to malloc 0 memory, and tests to try to free pointers that either are null or point to something in the middle of a malloced array. Additionally, we have a test to ensure that we can't double free something or free something with an address that doesn't align with our 8 bytes. All tests do not require any arguments and can be run simply with ./memtest.

*** However, "inappropriate pointers" do cause the program to exit, so testing requires commenting out certain test cases in order ti continue testing. ***

memtest list:
    method 1: Fills memory with objects, stores distinct bytes within them, and checks if the heap stores these bytes accurately. then frees them all. Checks if allocation works correctly without rewriting.
    method 2: Randomly allocates and deallocates within the heap. Tests if malloc and free can work while alternating.
    merge check: Allocates 4 objects to fill heap, frees middle 2, then allocates an object to fill the gap. Tests if  chunk merging works.
    whole allocate check: Allocates object that fills heap. Checks if heap can be filled with one object.
    
    error checks:
        too big malloc: Attempts to allocate too many bytes.
        free pointer not allocated with malloc: Attempts to free a pointer not allocated using malloc.
        malloc 0 bytes: Attempts to allocate 0 bytes.
        free not the start of a chunk: Attempts to free a pointer in the middle of a chunk.
        free odd address: Attempts to free an odd memory address.
        double free: Attempts to free a pointer that was already freed.
    
    check leak detector: Allocates memory and never frees it, so leak detector can run.

memgrind list:
    method 1: mallocs and frees a 1-byte object, 120 times.
    method 2: mallocs 120 1-byte objects, then frees all of them.
    method 3: randomly mallocs and frees 1-byte objects until 120 1-byte objects have been malloced, then frees all.
    method 4: mallocs 10 1-byte objects then frees all of them, and then repeats this 12 times.
    method 5: mallocs 120 8-byte objects, then frees every 4th one. Then the rest are freed.
