#ifndef _tstcommon_h_
#define _tstcommon_h_

#define MAX_NUM_ITERATIONS 500000
#define SIZE_MIN_EXP 2
#define SIZE_EXP_SPAN 10
#define SEED 81485

/*
 * MemoryBlock represents a memory block.
 */
typedef struct  {
    void * ptr;
    int size;
} MemoryBlock;

#endif /* _tstcommon_h_ */
