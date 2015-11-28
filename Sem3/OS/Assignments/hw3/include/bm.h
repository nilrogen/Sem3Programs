#ifndef __BM_H
#define __BM_H

#include <include.h>
#include <arpa/inet.h>

#define HOSTNAME "linux-node-1"

#define BMPRODUCE 1
#define BMCONSUME 2 
#define BMREPLY   4 


/* Errors */
#define BMSUCCESS 0
#define BMIOERROR 1
#define BM

typedef struct {
	int	mtype;  // MESSAGE TYPE: PRODUCE, CONSUME, REPLY
	int ring;   // RING: 0 to N_RINGS - 1
	int ovalue; // Return value: -1 for error
	int evalue; // The Error number
} bmmsg_t;

extern bmmsg_t bmm_hton(int, int, int, int);
extern bmmsg_t bmm_ntoh(bmmsg_t);

extern int produce(int, int); 
extern int consume(int, int);
extern int bmperror(char *);

#endif
