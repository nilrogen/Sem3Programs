#ifndef __BINHEAP_H
#define __BINHEAP_H

#include <include.h>


typedef struct BINHEAP binheap_t;

extern int init_heap(binheap_t *, int (*)(void *, void *));
extern int destroy_heap(binheap_t *);

extern int bheap_insert(binheap_t *, void *);
extern binheap_t *bheap_merge(binheap_t *, binheap_t *);

#endif
