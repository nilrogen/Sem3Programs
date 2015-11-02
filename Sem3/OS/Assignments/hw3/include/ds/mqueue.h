#ifndef __MQUEUE_H
#define __MQUEUE_H

#include <include.h>
#include "mlist.h"

typedef struct mlist mqueue; 

extern int mqueue_init(mqueue *);
extern int mqueue_destroy(mqueue *);
extern struct mlist_node *peek(mqueue *);
extern struct int *enqueue(mqueue *, void *); 
extern struct mlist_node *dequeue(mqueue *);

#endif


