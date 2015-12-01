#ifndef __NM_H
#define __NM_H

#include <include.h>
#include <lamport.h>
#include <msqutil.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define N_MUTEX 8 

typedef struct {
	int mutex;
	lmsg_t lm;
} nmsg_t;


#endif
