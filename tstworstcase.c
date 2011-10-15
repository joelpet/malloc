#include "malloc.h"
#include "tstcommon.h"
#include <stdio.h>
#include <stdlib.h>


int main(
        int argc,
        void *argv[])
{
    int num_iterations = (argc > 1) ? atoi(argv[1]) : MAX_NUM_ITERATIONS;
    int i, iter, size, lowbreak, highbreak;
    MemoryBlock mem_blocks[MAX_NUM_ITERATIONS];

    if (num_iterations > MAX_NUM_ITERATIONS) {
        fprintf(stderr, "Warning: num_iterations larger than MAX_NUM_ITERATIONS, setting it to %d.\n", MAX_NUM_ITERATIONS);
        num_iterations = MAX_NUM_ITERATIONS;
    }

    for (i = 0; i < num_iterations; ++i) {
        mem_blocks[i].ptr = NULL;
    }

    lowbreak = sbrk(0);
    srand(SEED);

    for (iter = 0; iter < num_iterations; ++iter) {
        i = rand() % num_iterations;

        switch (rand() % 10) {
            case 0:
                /*
                 * Free a memory block (one out of ten times).
                 */

                free(mem_blocks[i].ptr);
                mem_blocks[i].ptr = NULL;
                break;

            default:

                /*
                 * Allocate new memory if the randomly chosen block is empty.
                 */

                if (mem_blocks[i].ptr == NULL) {
                    mem_blocks[i].size = 2 << (rand() % SIZE_EXP_SPAN + SIZE_MIN_EXP - 1);
                    mem_blocks[i].ptr = malloc(mem_blocks[i].size);

                    if (mem_blocks[i].ptr == NULL) {
                        perror("Could not get more memory from system in malloc.");
                        exit(1);
                    }
                
                }


                /*
                 * Then reallocate directly.
                 */

                mem_blocks[i].size = 2 << (rand() % SIZE_EXP_SPAN + SIZE_MIN_EXP - 1);
                mem_blocks[i].ptr = realloc(mem_blocks[i].ptr, mem_blocks[i].size);

                if (mem_blocks[i].ptr == NULL) {
                    perror("Could not get memory from system for realloc.");
                    exit(1);
                }

                break;
        }
    }


    /*
     * Make sure to free every memory block; we don't want to leak memory, even
     * though it's just a test program.
     */

    for (i = 0; i < num_iterations; ++i) {
        free(mem_blocks[i].ptr);
    }

    highbreak = sbrk(0);

    /*
     * Print the statistics.
     */

    fprintf(stderr, "Memory usage: %u b\n", highbreak - lowbreak);


    return 0;
}
