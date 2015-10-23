#ifndef __evseq_H
#define __evseq_H

#include "assignment2.h"
	
typedef struct mg_eventcounter {
	int counter;
	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
} mevt_t;


typedef struct mg_sequencer {
	int counter;
	pthread_mutex_t *mutex;
} mseq_t;

extern int evtc_init(mevt_t *);
extern void mg_await(mevt_t *, int);
extern void mg_advance(mevt_t *);

extern int seq_init(mseq_t *);
extern int ticket(mseq_t *);

#endif
