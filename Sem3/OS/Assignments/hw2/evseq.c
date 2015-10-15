/**
 * Author: Michael Gorlin
 */
 #include "evseq.h"


extern int evtc_init(mevt_t *val) {

	val->counter = 0; 
	val->owner_tid = (pid_t) 0; 
	pthread_mutex_init(&val->innerMutex, NULL);
	pthread_mutex_init(&val->outerMutex, NULL);
	pthread_cond_init(&val->cond, NULL);
	val->initialized = TRUE;

	return 0;
}

/**
 * This is a simplification of manually setting the val
 * by calling signal val times.
 */
extern int set_counter(mevt_t *evtc, uint val) {
	etvc->counter = val;
	return 0;
}

/**
 * When this is called if the ticket is equal to counter 
 * you dont block, otherwise you do block.
 */
extern void mg_await(mevt_t *val, uint ticket) {
	pthread_mutex_lock(&val->innerMutex);
	while (val->counter <= ticket) {
		pthread_cond_wait(&val->cond, &val->innerMutex);
	}
	pthread_mutex_unlock(&val->innerMutex);
}

extern void mg_signal(mevt_t *val) {
	//TODO? THIS MAY NEED A LOCK. 
	val->counter++;
	pthread_cond_broadcast(&val->cond);
}

extern uint seq_init(mseq_t *val) {
	// TODO: Remove eventually 
	if (val->initialized == TRUE) {
		assert(0 == 1);
	}

	val->counter = 0; 
	pthread_mutex_init(&val->mutex, NULL);
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
