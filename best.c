/* best fit malloc
 *
 * This module contains the function that implements the Best fit malloc
 * strategy, malloc_best().
 *
 */

#include <limits.h>


/* malloc_best
 *
 * malloc_best returns the start address of the newly allocated memory.
 * It implements the Best fit algorithm, which tries to find the smallest free
 * block that is large enough.
 *
 */
void *malloc_best(
    size_t nbytes) /* number of bytes of memory to allocate */
{
    Header *p, *prevp;
    Header *moreroce(unsigned);
    unsigned nunits;
    int min_size = INT_MAX;
    Header *minp = NULL, *minprevp = NULL;

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL) { /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    minp = NULL;


    /*
     * Iterate over the free list and find the smallest block that is large
     * enough to hold nbytes of data.
     */

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { /* big enough */
            if (p->s.size == nunits) { /* exactly */
                prevp->s.ptr = p->s.ptr;
                freep = prevp;
                return (void *)(p + 1);
            }
            else {
                if (minp == NULL || p->s.size < min_size) {
                    minp = p;
                    minprevp = prevp;
                    min_size = minp->s.size;
                }
            }
        }
        if (p == freep) { /* wrapped around free list */
            if (minp != NULL) {
                /* allocate tail end */
                minp->s.size -= nunits;
                minp += minp->s.size;
                minp->s.size = nunits;
                freep = minprevp;
                return (void *)(minp + 1);
            }
            if ((p = morecore(nunits)) == NULL) {
                return NULL; /* none left */
            }
        }
    }
}
