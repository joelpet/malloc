#ifndef _malloc_h_
#define _malloc_h_

#include <string.h>

typedef long Align; /* for alignment to long boundary */

union header {
    /* block header */
    struct {
        union header *ptr;/* next block if on free list */
        unsigned size;
        /* size of this block */
    } s;
    Align x;
    /* force alignment of blocks */
};
typedef union header Header;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
static Header *morecore(unsigned nu);

#endif /* _malloc_h */
