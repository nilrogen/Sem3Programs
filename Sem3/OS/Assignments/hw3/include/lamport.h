#ifndef __LAMPORT_H
#define __LAMPORT_H

#include <include.h>
#include <ds/mqueue.h>

typedef struct {
	int clock;
	int node; 
	mqueue *queue; 
} lmp_mutex_t; 

typedef struct {
	int clock;
	int node; 
	int type;
	int replies; 
} lmsg_t;

#define LREQUEST 0
#define LREPLY   1
#define LRELEASE 2

extern lmsg_t hmsgton(lmsg_t);
extern lmsg_t nmsgtoh(lmsg_t);

extern int handle_request(lmp_mutex_t *, lmsg_t *, lmsg_t *); 
//extern int handle_reply(lmp_mutex_t *, 


#endif
