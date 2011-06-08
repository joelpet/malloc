#include "brk.h"
#include "malloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#ifndef STRATEGY
#define STRATEGY 1
#endif

#ifndef NRQUICKLISTS
#define NRQUICKLISTS 4
#endif

unsigned int units2bytes(unsigned int nunits) {
    return sizeof(Header) * (nunits-1);
}

static Header base; /* empty list to get started */
static Header *freep = NULL; /* start of free list */

#if STRATEGY == 4
static bool first_run = true;
static Header* quick_fit_lists[NRQUICKLISTS];
static int smallest_block_size_exp = 5;

int get_quick_fit_list_index(unsigned int nbytes) {
    unsigned int upper_bound;
    int i;
    for (i = 0; i < NRQUICKLISTS; ++i) {
        upper_bound = 2 << (i + smallest_block_size_exp - 1); /* == 2^(i+smallest_block_size_exp) */
        if (nbytes + sizeof(Header) <= upper_bound) {
            return i;
        }
    }
    return NRQUICKLISTS;
}

Header* init_quick_fit_list(int list_index, int num_blocks) {
    int i;
    int block_size = 2 << (list_index + smallest_block_size_exp - 1); /* bytes, incl Header */
    int nbytes = num_blocks * block_size;

    char *cp;
    Header *up;
    cp = sbrk(nbytes);/* TODO rätt? */
    if (cp == (char *) -1) /* no space at all */
        return NULL;
    up = (Header *) cp;

    int nunits = block_size / sizeof(Header) - 1; /* excl Header */

    /* fixa blocken */
    up->s.ptr = up + 1 + nunits;
    up->s.size = nunits;

    for (i = 1; i < num_blocks; ++i) {
        up = up->s.ptr;
        up->s.ptr = up + 1 + nunits;
        up->s.size = nunits;
    }

    up->s.ptr = NULL;

    return (Header *) cp;
}
#endif

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
#elif STRATEGY == 4
    int i;
    if (first_run) {
        for (i = 0; i < NRQUICKLISTS; ++i) {
            quick_fit_lists[i] = NULL;
        }
        first_run = false;
    }

    int list_index = get_quick_fit_list_index(nbytes);

    /* kolla om nbytes får plats i någon av quickfit-listorna */
    if (list_index < NRQUICKLISTS) {
        /* kolla om den listan redan är initierad OCH det finns ett ledigt block */
        if (quick_fit_lists[list_index] == NULL) {
            /*
             * - fråga systemet om lämpligt mkt mer minne
             * - bygger direkt en lista av fria block (i vårt exempel 64 bytes)
             *   som länkas in i fri-listan
             */
            int num_blocks = 10; /* TODO ta en multipel av minnessidor istället */
            Header* new_quick_fit_list = init_quick_fit_list(list_index, num_blocks);
            if (new_quick_fit_list == NULL) {
                return NULL;
            } else {
                quick_fit_lists[list_index] = new_quick_fit_list;
            }
        } else {
            void* pointer_to_return = (void *)(quick_fit_lists[list_index] + 1);
            quick_fit_lists[list_index] = quick_fit_lists[list_index]->s.ptr;
            return pointer_to_return;
        }
    } else {
        /* som vanligt. */
    }
#endif

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { /* big enough */
            if (p->s.size == nunits) { /* exactly */
                prevp->s.ptr = p->s.ptr;
            }
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
    Header *bp, *p, *nextbp;

    if (ptr == NULL) {
        return malloc(size);
    } else if (size <= 0) {
        free(ptr);
        return NULL;
    }

    bp = (Header*)ptr - 1;
    unsigned int numbytes = units2bytes(bp->s.size);
    nextbp = bp;
    nextbp += bp->s.size;
    /*unsigned int numbytes = &nextbp - &bp;*/


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

