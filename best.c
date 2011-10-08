
void *malloc_best(size_t nbytes)
{
    Header *p, *prevp;
    Header *moreroce(unsigned);
    unsigned nunits;
    Header *minp, *minprevp;
    bool found_fitting_block = false;

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL) { /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    minp = prevp;

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { /* big enough */
            if (p->s.size == nunits) { /* exactly */
                prevp->s.ptr = p->s.ptr;
            }
            else {
                if (p->s.size < minp->s.size) {
                    found_fitting_block = true;
                    minp = p;
                    minprevp = prevp;
                    continue;
                }
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
            if (found_fitting_block) {
                /* allocate tail end */
                minp->s.size -= nunits;
                minp += minp->s.size;
                minp->s.size = nunits;
                freep = minprevp;
                return (void *)(minp + 1);
            }
            if ((p = morecore(nunits)) == NULL)
                return NULL; /* none left */
        }
    }
}
