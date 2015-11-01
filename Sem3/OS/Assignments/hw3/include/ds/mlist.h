#ifndef __MLIST_H
#define __MLIST_H

#include <include.h>

struct mlist_node;

struct mlist {
	pthread_mutex_t lock; 
	struct mlist_node *head; 
};

struct mlist_node {
	struct mlist_node *head; 
	struct mlist_node *tail;
	void *data;
};

struct mlist_iter {
	struct mlist *list; 
	struct mlist_node *current; 
};

extern int mlist_init(struct mlist *); 
extern int mlist_destroy(struct mlist *);

extern int mlist_insert(struct mlist *, void *);
extern int mlist_append(struct mlist *, void *);

extern int mlist_iter_init(struct mlist *, struct mlist_iter *);
extern struct mlist_node *next(struct mlist_iter *);
extern struct mlist_node *prev(struct mlist_iter *);

#endif