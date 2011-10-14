/* first fit malloc
 *
 * This module contains the function that implements the First fit malloc
 * strategy, malloc_first().
 *
 */


/* malloc_first
 *
 * malloc_first returns the start address of the newly allocated memory.
 * It implements the First fit algorithm, which simply returns the first free
 * memory that is large enough.
 *
 */
void *malloc_first(
        size_t nbytes) /* number of bytes of memory to allocate */
{
    Header *p, *prevp;
    Header *moreroce(unsigned);
    unsigned nunits;

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL) { /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }


    /*
     * Iterate over the free list and find a block that is large enough to hold
     * nbytes of data.
     */

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { /* big enough */
            if (p->s.size == nunits) { /* exactly */
                prevp->s.ptr = p->s.ptr;
            }
            else {
                /* allocate tail end */
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            /* return Data part of block to user */
            return (void *)(p+1); 
        }
        if (p == freep) { /* wrapped around free list */
            if ((p = morecore(nunits)) == NULL)
                return NULL; /* none left */
        }
    }
}
