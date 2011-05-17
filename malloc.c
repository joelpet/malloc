#include "brk.h"
#include "malloc.h"
#include <stdbool.h>
#include <unistd.h>

#ifndef STRATEGY
#define STRATEGY 1
#endif

static Header base; /* empty list to get started */
static Header *freep = NULL; /* start of free list */

/* malloc: general-purpose storage allocator */
void *malloc(size_t nbytes)
{
    Header *p, *prevp;
    Header *moreroce(unsigned);
    unsigned nunits;
#if STRATEGY == 2
    Header *minp, *minprevp;
    bool found_fitting_block = false;
#elif STRATEGY == 3
    Header *maxp, *maxprevp;
    bool found_fitting_block = false;
#endif

    if (nbytes == 0) {
        return NULL;
    }

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL) { /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

#if STRATEGY == 2
    minp = prevp;
#elif STRATEGY == 3
    maxp = prevp;
#endif

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { /* big enough */
            if (p->s.size == nunits) /* exactly */
                prevp->s.ptr = p->s.ptr;
            else {
#if STRATEGY == 2
                if (p->s.size < minp->s.size) {
                    found_fitting_block = true;
                    minp = p;
                    minprevp = prevp;
                    continue;
                }
#elif STRATEGY == 3
                if (p->s.size > maxp->s.size) {
                    found_fitting_block = true;
                    maxp = p;
                    maxprevp = prevp;
                    continue;
                }
#endif
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
#if STRATEGY == 2
            if (found_fitting_block) {
                /* allocate tail end */
                minp->s.size -= nunits;
                minp += minp->s.size;
                minp->s.size = nunits;
                freep = minprevp;
                return (void *)(minp + 1);
            }
#elif STRATEGY == 3
            if (found_fitting_block) {
                /* allocate tail end */
                maxp->s.size -= nunits;
                maxp += maxp->s.size;
                maxp->s.size = nunits;
                freep = maxprevp;
                return (void *)(maxp + 1);
            }
#endif
            if ((p = morecore(nunits)) == NULL)
                return NULL; /* none left */
        }
    }
}

#define NALLOC 1024

/* minimum #units to request */
/* morecore: ask system for more memory */
static Header *morecore(unsigned nu)
{
    char *cp;
    Header *up;
    if (nu < NALLOC)
        nu = NALLOC;
    cp = sbrk(nu * sizeof(Header));
    if (cp == (char *) -1) /* no space at all */
        return NULL;
    up = (Header *) cp;
    up->s.size = nu;
    free((void *)(up+1));
    return freep;
}


/* free: put block ap in free list */
void free(void *ap)
{
    if (ap == NULL) {
        return;
    }

    Header *bp, *p;
    bp = (Header *)ap - 1;

    if (bp->s.size <= 0) {
        return;
    }

    /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; /* freed block at start or end of arena */
    if (bp + bp->s.size == p->s.ptr) {
        /* join to upper nbr */
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr;
    if (p + p->s.size == bp) {
        /* join to lower nbr */
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else
        p->s.ptr = bp;
    freep = p;
}

void *realloc(void *ptr, size_t size) {

    Header *bp, *p, *nextp, *newp;

    if (ptr == NULL) {
        return malloc(size);
    } else if (size <= 0) {
        free(ptr);
        return NULL;
    }

    bp = (Header*)ptr-1;
    if (size == bp->s.size) {
        return ptr;
    }

    /* sista blocket i listan (innan det tar slut) */
    if (bp >= bp->s.ptr) {
        /* mallockera mer minne och kopiera data eetc. */
        p = malloc(size);
        memcpy(ptr, p, size);
        free(ptr);
    }
    
    /* check if the new block is smaller --> fitts */
    if (size < bp->s.size) {
        /* divide the block and update bp */
        nextp = bp->s.ptr;
        newp = bp + size;
        newp->s.ptr = nextp;
        newp->s.size = bp->s.size-size;
        bp->s.size = size;
        /* check if there was a perfect fit */
        if (nextp == newp){
          newp->s.size += nextp->s.size;
          newp->s.ptr = nextp->s.ptr;
        }
        return ptr;        
    }
    /* check if there is an adjacent block that fitts the new size */
    if (bp->s.size + ptr == bp->s.ptr && bp->s.size >= size - bp->s.size) {
        nextp = bp->s.ptr;
        /* perfect fit */
        if (size == bp->s.size + nextp->s.size) {
            bp->s.ptr = nextp->s.ptr;
            bp->s.size = size;
            return ptr;
        }
        /* divide the next block and update bp */
        newp = bp + size;
        newp->s.ptr = nextp->s.ptr;
        newp->s.size = nextp->s.size - (size - bp->s.size);
        bp->s.size = size;
        
    }

    /* mallockera mer minne och kopiera data eetc. */
    p = malloc(size);
    memcpy(ptr, p, size);
    free(ptr);

}


