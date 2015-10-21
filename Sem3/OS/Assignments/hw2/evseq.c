/**
 * Author: Michael Gorlin
 */
 #include "assignment2.h"
 #include "evseq.h"


extern int evtc_init(mevt_t *val) {

	val->counter = 0; 
	val->owner_tid = (pid_t) 0; 
	pthread_mutex_init(&val->mutex, NULL);
	pthread_cond_init(&val->cond, NULL);

	return 0;
}

/**
 * This is a simplification of manually setting the val
 * by calling signal val times.
 */
extern int set_counter(mevt_t *evtc, uint val) {
	evtc->counter = val;
	return 0;
}

/**
 * When this is called if the ticket is equal to counter 
 * you dont block, otherwise you do block.
 */
extern void mg_await(mevt_t *val, uint ticket) {
	pthread_mutex_lock(&val->mutex);
	while (val->counter < ticket) {
		pthread_cond_wait(&val->cond, &val->mutex);
	}
	pthread_mutex_unlock(&val->mutex);
}

extern void mg_signal(mevt_t *val) {
	//TODO? THIS MAY NEED A LOCK. 
	val->counter++;
	pthread_cond_broadcast(&val->cond);
}

extern uint seq_init(mseq_t *val) {
	val->counter = 0; 
	pthread_mutex_init(&val->mutex, NULL);

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