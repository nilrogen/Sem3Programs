#ifndef __evseq_H
#define __evseq_H

#include "assignment2.h"
	
typedef struct mg_eventcounter {
	uint counter;
	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
} mevt_t;


typedef struct mg_sequencer {
	uint counter;
	pthread_mutex_t *mutex;
} mseq_t;

extern int evtc_init(mevt_t *);
extern void mg_await(mevt_t *, uint);
extern void mg_advance(mevt_t *);

extern uint seq_init(mseq_t *);
extern uint ticket(mseq_t *);

#endif
