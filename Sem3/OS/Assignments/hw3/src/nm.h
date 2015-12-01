#ifndef __NM_H
#define __NM_H

#include <include.h>
#include <lamport.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef struct {
	int mutex;
	lmsg_t lm;
} nmsg_t;

/**
 * Sets up the communication threads between all the servers. 
 * Each connection has a single thread that maintains reading
 * messages from the other node controllers. 
 */
extern int setup_conn(int, int);
extern void read_thread(void *);

extern void signal_thread(void *);

#endif
