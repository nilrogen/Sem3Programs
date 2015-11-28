#ifndef __BM_SERVER_H
#define __BM_SERVER_H

#include <include.h>
#include <sys/shm.h>
#include <arpa/inet.h>

#define SHM_KEY 978888109

#define HOST_IP "0.0.0.0"

#define N_ITEMS 20
#define N_RINGS 4

typedef struct {
	int buffer[N_RINGS][N_ITEMS];
	int p_in[N_RINGS];
	int p_out[N_RINGS];
} ring_t;

extern void produce(); 
extern void consume();


#endif
