/**
 * Author: Michael Gorlin
 */

#include "assignment2.h"

struct mg_eventcounter { 
	unsigned int counter;
	bool initialized;
	pid_t owner_tid;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
};

struct mg_sequencer {
	unsigned int counter;
	bool initialized;
	pthread_mutex_t mutex;
}:


extern int evtc_init(mevt_t *val) {
	// TODO: Remove eventually 
	if (val->initialized == TRUE) {
		assert(0 == 1);
	}

	val->counter = 0; 
	val->owner_tid = (pid_t) 0; 
	pthread_mutex_init(&val->mutex, NULL);
	pthread_cond_init(&val->cond, NULL);
	val->initialized = TRUE;

	return 0;
}

extern await(mevt_t *val, uint ticket) {
	pthread_mutex_lock(val->mutex);
	// TODO FINISH


}



extern int seq_init(mseq_t *val) {
	// TODO: Remove eventually 
	if (val->initialized == TRUE) {
		assert(0 == 1);
	}

	val->counter = 0; 
	val->mutex = PTHREAD_MUTEX_INITIALIZER;
	val->initialized = TRUE;

	return 0;
}

extern uint ticket(mseq_t *val) {
	uint retv;

	pthread_mutex_lock(&val->mutex);

	// CS Start
	retv = val->counter++;
	// CS End

	pthread_mutex_unlock(&val->mutex);

	return retv;
}
