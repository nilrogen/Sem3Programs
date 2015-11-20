#ifndef __LAMPORT_H
#define __LAMPORT_H

#include <include.h>

#define REQUEST 0
#define REPLY   1
#define RELEASE 2

typedef struct { 
	int clockval;
	int node;
	int type;
} lmsg_t;

extern lmsg_t hmsgton(lmsg_t);
extern lmsg_t nmsgtoh(lmsg_t);

extern lmsg_t prepare_message(int type, int node);
extern lmsg_t handle_message(lmsg_t);

extern int request_lock();
extern int release_lock(); 

#endif

