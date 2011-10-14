/* quick fit malloc
 *
 * This module contains the function that implements the Quick fit malloc
 * strategy, malloc_quick(). It also contains some helper functions for
 * malloc_quick().
 *
 */

#define POW2(E) 2 << (E - 1)

/*
 * A boolean indicating whether this is the first run of malloc_quick or not.
 */
static bool first_run = true;

/*
 * An array that holds a pointer to the header of the first block in the the
 * quick fit lists.
 */
static Header* quick_fit_lists[NRQUICKLISTS];

/*
 * Defines the smallest quick fit list block size, as follows:
 *
 * smallest block size (bytes) = 2^(smallest_block_size_exp)
 */
static int smallest_block_size_exp = 5;

/* get_quick_fit_list_index
 *
 * get_quick_fit_list_index returns the quick fit list index for the given
 * number of bytes of data to allocate.
 * The size of the header is added in the calculations, so nbytes should be the
 * number of bytes of data to allocate.
 *
 */
int get_quick_fit_list_index(
        unsigned int nbytes) /* number of data bytes to allocate */
{
    unsigned int upper_bound;
    int i;
    for (i = 0; i < NRQUICKLISTS; ++i) {
        upper_bound = POW2(i + smallest_block_size_exp);
        if (nbytes + sizeof(Header) <= upper_bound) {
            return i;
        }
    }
    return NRQUICKLISTS;
}

/* init_quick_fit_list
 *
 * init_quick_fit_list gets more memory from system and initiates blocks of
 * correct size for the given list_index, returning a pointer to the head of
 * the list.
 *
 */
Header* init_quick_fit_list(
        int list_index) /* the index of the quick fit list to initialize */
{
    int i;

    /*
     * Calculate the block size and ask system for more memory. Always a multiple of
     * the memory's page size, such that at least one block fits.
     */

    int block_size = POW2(list_index + smallest_block_size_exp); /* bytes, incl Header */
    int nbytes = sysconf(_SC_PAGESIZE);
    if (nbytes < 0) {
        return NULL;
    }
    nbytes += (block_size / nbytes) * nbytes;
    int num_blocks = nbytes / block_size;
    char *cp;
    Header *up;
    cp = sbrk(nbytes); /* ask for memory */
    if (cp == (char *) -1) /* no space at all */
        return NULL;
    up = (Header *) cp; /* pointer to start of new memory */

    int nunits = block_size / sizeof(Header);


    /*
     * Initialize the block headers with correct size and next-pointer.
     */

    for (i = 0; i < num_blocks - 1; ++i) {
        up->s.ptr = up + nunits;
        up->s.size = nunits;
        up = up->s.ptr;
    }


    /* 
     * Let the last block point to NULL, but still with correct size.
     */

    up->s.ptr = NULL;
    up->s.size = nunits;

    return (Header *) cp; /* pointer to beginning of first header */
}


/* malloc_quick
 *
 * malloc_quick returns the start address of the newly allocated memory.
 *
 */
void *malloc_quick(
        size_t nbytes) /* number of bytes of memory to allocate */
{
    Header *moreroce(unsigned);
    unsigned nunits;
    int list_index, i;

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    list_index = get_quick_fit_list_index(nbytes);

    /* 
     * Use another strategy for too large allocations. We want the allocation
     * to be quick, so use malloc_first().
     */

    if (list_index >= NRQUICKLISTS) {
        return malloc_first(nbytes);
    }


    /*
     * Initialize the quick fit lists if this is the first run.
     */

    if (first_run) {
        for (i = 0; i < NRQUICKLISTS; ++i) {
            quick_fit_lists[i] = NULL;
        }
        first_run = false;
    }


    /*
     * If the quick fit list pointer is NULL, then there are no free memory
     * blocks present, so we will have to create some before continuing.
     */

    if (quick_fit_lists[list_index] == NULL) {
        Header* new_quick_fit_list = init_quick_fit_list(list_index);
        if (new_quick_fit_list == NULL) {
            return NULL;
        } else {
            quick_fit_lists[list_index] = new_quick_fit_list;
        }
    }


    /*
     * Now that we know there is at least one free quick fit memory block,
     * let's use return that and also update the quick fit list pointer so that
     * it points to the next in the list.
     */

    void* pointer_to_return = (void *)(quick_fit_lists[list_index] + 1);
    quick_fit_lists[list_index] = quick_fit_lists[list_index]->s.ptr;

    return pointer_to_return;
}
