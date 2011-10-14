#include "malloc.h"
#include "tstcommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_NUM_ITERATIONS 100000
#define SIZE_MIN_EXP 2
#define SIZE_EXP_SPAN 12


int main(
        int argc,
        char *argv[])
{
    int num_iterations = (argc > 1) ? atoi(argv[1]) : MAX_NUM_ITERATIONS;
    int i, j, lowbreak, highbreak;
    MemoryBlock mem_blocks[MAX_NUM_ITERATIONS];
    num_iterations = num_iterations > MAX_NUM_ITERATIONS ? MAX_NUM_ITERATIONS : num_iterations;

    for (i = 0; i < num_iterations; ++i) {
        mem_blocks[i].ptr = NULL;
    }

    lowbreak = sbrk(0);
    srand(time(NULL));

    /*
     * Allocate memory with random powers of two sizes.
     */

    for (i = 0; i < num_iterations; ++i) {
        mem_blocks[i].size = 2 << (rand() % SIZE_EXP_SPAN + SIZE_MIN_EXP - 1);
        mem_blocks[i].ptr = malloc(mem_blocks[i].size);

        if (mem_blocks[i].ptr == NULL) {
            perror("Could not get more memory from system.");
            exit(1);
        }
    }

    /*
     * Free the allocated memory.
     */

    for (i = 0; i < num_iterations; ++i) {
        free(mem_blocks[i].ptr);
        mem_blocks[i].ptr = NULL;
    }

    highbreak = sbrk(0);


    /*
     * Print the statistics.
     */

    fprintf(stderr, "Memory usage: %d b\n", highbreak - lowbreak);

    return 0;
}
