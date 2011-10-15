#ifndef _tstcommon_h_
#define _tstcommon_h_

#ifndef SEED
#define SEED 81485
#endif

/*
 * MemoryBlock represents a memory block.
 */
typedef struct  {
    void * ptr;
    int size;
} MemoryBlock;

#endif /* _tstcommon_h_ */
