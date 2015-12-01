#ifndef __MSQUTIL_H
#define __MSQUTIL_H

#include <include.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#define MQ_KEY  (key_t) 786929221

#define NMMTYPE 1L

#define REQUESTMSG 0
#define RELEASEMSG 1
#define REPLYMSG   2

typedef struct {
	long mtype;
	int rtype; // REQUEST OR RELEASE
	int mutex; // Which mutex to request
	int pid;
} nmrequest_t;

typedef struct {
	long mtype; // Pid << 16 | Mutex
	int errorval;
} nmreply_t;

extern int open_msg(key_t);
extern int remove_msg(int);

// Producer and Consumer Use
extern int request(int, int);
extern int release(int, int);


// Node Manager Use
extern nmrequest_t msq_next(int);
extern int msq_reply(int, int, int, int);


#endif


