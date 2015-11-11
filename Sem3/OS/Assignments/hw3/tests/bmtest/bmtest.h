#ifndef __BMTEST_H_
#define __BMTEST_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include <bmproto.h>

#define N 100
#define T 4

#define IP "127.0.0.1"
#define PORT 4990

typedef struct {
	int buffer[T][N];
	int p_out[T];
	int p_in[T];
	int serial[T];
	int spaces[T];
	int product[T];
} ringbuffer_t;

extern int producer_main();
extern int consumer_main();


#endif
