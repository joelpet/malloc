#include "brk.h"
#include "malloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *malloc_first(size_t nbytes) {
    Header *p, *prevp;
    Header *moreroce(unsigned);
    unsigned nunits;

    if (nbytes == 0) {
        return NULL;
    }

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL) { /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

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
