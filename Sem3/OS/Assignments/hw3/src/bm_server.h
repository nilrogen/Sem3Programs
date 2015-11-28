#ifndef __BM_SERVER_H
#define __BM_SERVER_H

#include <include.h>
#include <bm.h>
#include <sys/shm.h>

// If running bm_server it's assumed that the hostname in bm.h resolves 
// to some ip that can be accepted by this machine. 
#define SHM_KEY 978888109

#define HOST_IP "0.0.0.0" 

#define N_ITEMS 20
#define N_RINGS 4

typedef struct {
	int buffer[N_RINGS][N_ITEMS];
	int p_in[N_RINGS]; // Producer pointer
	int pvalue[N_RINGS];
	int p_out[N_RINGS]; // Consumer pointer
} ring_t;

#endif
