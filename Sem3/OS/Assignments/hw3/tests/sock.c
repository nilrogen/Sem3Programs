#ifndef __BMPROTO_H
#define __BMPROTO_H

#include <socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#define SETUP_TYPE 0
#define REPLY_TYPE 1
#define PRODUCTION_TYPE 2

#define 

typedef struct {
	int type;
	union {
		struct { // SETUP
			int process_type;
		} setup_msg;
		struct { // REPLY
			int status;
			int production_num; 
			int production_type;
			int production_get;
		} reply;
		struct { // PRODUCE
			int production_num;
			int production_type;
		} operation_msg;
	} message;
} bmproto_t;

int prod_client(int sfd);

int cons_client(int sfd);

int server(int sfd) {

}

#endif
