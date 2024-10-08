# mymalloc

Names:
Neha Jeyaram (nj366)
Ivy Ng (iyn2)

We've built a variety of tests for our project to test all functions of our program. The first test that we have created is one that creates an array of objects. Each object is sized exactly so that malloc's memory is full, then each object is freed. Secondly, we have a test where we have an array of size 100 that randomly allocates and deallocates items a hundred times, then frees any remaining objects. This second test is repeated 20 times. Afterwards, we malloc 1 very big object that fills exactly the amount of memory that is available. In case of failure to deallocate something, the leak detector can clean up any bytes that have been left out to prevent errors.

We also have some error tests. There is a test that tries to malloc more memory than is available, a test that tries to malloc 0 memory, and tests to try to free pointers that either are null or point to something in the middle of a malloced array. Additionally, we have a test to ensure that we can't double free something or free something with an address that doesn't align with our 8 bytes. All tests do not require any arguments and can be run simply with ./memtest.
