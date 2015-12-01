#ifndef __LAMPORT_H
#define __LAMPORT_H

#include <include.h>
#include <ds/mqueue.h>

typedef struct {
	int clock;
	int node; 
	pthread_mutex_t lock;
	mqueue *queue; 
} lmp_mutex_t; 

typedef struct {
	int type; 
	int clock;
	int node;
	int pid; 	
	int replies;
} lmsg_t;

#define LREQUEST 0
#define LREPLY   1
#define LRELEASE 2

extern lmsg_t hmsgton(lmsg_t);
extern lmsg_t nmsgtoh(lmsg_t);

extern lmp_mutex_t *lmp_mutex_create(int);
extern int lmp_mutex_destroy(lmp_mutex_t *);

extern int handle_request(lmp_mutex_t *, lmsg_t); 
extern int handle_reply(lmp_mutex_t *, lmsg_t);
extern lmsg_t *handle_release(lmp_mutex_t *);


#endif
