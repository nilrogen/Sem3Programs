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

enum bmptype {
	BMP_SETUP=0,
	BMP_REPLY=1,
	BMP_OPERATION=2,
	BMP_DESTROY=4
};

enum bmpstatus {
	BMP_SUCCESS=0,
	BMP_FAILURE=1
};
enum bmpoperation {
	BMP_PRODUCE=0,
	BMP_CONSUME=1
};

typedef struct {
	enum bmptype type;
	union {
		struct { // SETUP
			int operation_type;
		} setup_msg;
		struct { // REPLY
			int operation_num;  // TID
			enum bmpstatus status;
			enum bmpoperation operation_type; 
			int selection; 
			int value;  
		} reply;
		struct { // OPERATION
			int operation_num;
			enum bmpoperation operation_type; 
			int value;

		} operation_msg;
		struct { // DESTROY
			int operation_num;
		}
	} message;
} bmproto_t;

inline bmpproto_t bmp_setup(enum bmpoperation opt) {
	bmpproto_t ret;
	ret.type = BMP_SETUP;
	ret.setup_msg.operation_type = opt;
	return ret;
}

inline bmpproto_t bmp_setup_reply(enum bmpstatus status, int opnum) {
	bmpproto_t ret;
	ret.type = BMP_REPLY;
	ret.reply_msg.status = status;
	ret.reply_msg.operation_num = opnum;
	ret.reply_msg.operation_num = opnum;
	return ret;
}

inline bmpproto_t bmp_produce_reply(enum 


int prod_client(int sfd);

int cons_client(int sfd);

int server(int sfd) {

}

#endif
