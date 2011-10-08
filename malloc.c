#include "brk.h"
#include "malloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef STRATEGY
#define STRATEGY 1
#endif

#ifndef NRQUICKLISTS
#define NRQUICKLISTS 4
#endif

static Header base; /* empty list to get started */
static Header *freep = NULL; /* start of free list */

/* Include the different malloc implementation source files here. */
#include "first.c"
#include "best.c"
#include "worst.c"
#include "quick.c"


/* malloc: general-purpose storage allocator */
void *malloc(size_t nbytes) {
    if (nbytes == 0) {
        return NULL;
    }

#if   STRATEGY == 1
    return malloc_first(nbytes);
#elif STRATEGY == 2
    return malloc_best(nbytes);
#elif STRATEGY == 3
    return malloc_worst(nbytes);
#elif STRATEGY == 4
    return malloc_quick(nbytes);
#else
    exit(1);
#endif
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
#if STRATEGY == 4
    int list_index;
    Header* old_first_free;
#endif

    if (ap == NULL) {
        return;
    }

    Header *bp, *p;
    bp = (Header *)ap - 1;

    if (bp->s.size <= 0) {
        return;
    }

#if STRATEGY == 4
    list_index = get_quick_fit_list_index(bp->s.size * sizeof(Header));
    if (list_index < NRQUICKLISTS) {
        old_first_free = quick_fit_lists[list_index];
        quick_fit_lists[list_index] = bp;
        bp->s.ptr = old_first_free;
        return;
    }
#endif

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
    Header *bp, *p;

    if (ptr == NULL) {
        return malloc(size);
    } else if (size <= 0) {
        free(ptr);
        return NULL;
    }

    bp = (Header*) ptr - 1;
    unsigned int numbytes = sizeof(Header) * bp->s.size;

    if (size == numbytes) {
        return ptr;
    }

    /* mallockera mer minne och kopiera data eetc. */
    p = malloc(size);
    if (p == NULL) {
        return NULL;
    }

    if (size < numbytes) {
        numbytes = size;
    }
    memcpy(p, ptr, numbytes);
    free(ptr);

    return p;
}

