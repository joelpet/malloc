#include "malloc.h"
#include "tstcommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_ITERATIONS 10000
#define SIZE_MIN_EXP 2
#define SIZE_EXP_SPAN 12


int main()
{
    int i, j, lowbreak, highbreak;
    MemoryBlock mem_blocks[NUM_ITERATIONS];

    fprintf(stderr, "Strategy: %d\n", STRATEGY);

    for (i = 0; i < NUM_ITERATIONS; ++i) {
        mem_blocks[i].ptr = NULL;
    }

    lowbreak = sbrk(0);
    srand(time(NULL));

    /*
     * Allocate memory with random powers of two sizes.
     */

    for (i = 0; i < NUM_ITERATIONS; ++i) {
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

    for (i = 0; i < NUM_ITERATIONS; ++i) {
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
