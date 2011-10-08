
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

void *malloc_quick(size_t nbytes)
{
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
