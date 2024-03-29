#ifndef __MSQUTIL_H
#define __MSQVUTIL_H

#include <include.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#define MQ_KEY  (key_t) 786929221

typedef struct {
	long mtype;
	int mutex; // Which mutex to request
	int pid;
} nmrequest_t;

typedef struct {
	long mtype; // Pid << 32 | Mutex
	int errorval;
} nmreply_t:

extern int open_msg(key_t);
extern int remove_msg(int);

// Producer and Consumer Use
extern int request(int);

// Node Manager Use
extern int handle();




#endif


