/**
 * Author: Michael Gorlin
 */
#include "assignment2.h"
#include "evseq.h"

static inline int mutex_lock(pthread_mutex_t *mutex) {
	if (pthread_mutex_lock(mutex) != 0) {
		perror("THERE IS AN ERROR HERE");
	}
	return 0;
}

static inline int mutex_unlock(pthread_mutex_t *mutex) {
	if (pthread_mutex_unlock(mutex) != 0) {
		perror("THERE IS AN ERROR HERE (UNLOCK)");
	}
	return 0;
}

extern int evtc_init(mevt_t *val) {
	static int snum = 0;
	val->counter = 0; 
	val->nv = snum++;
	val->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	val->cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_mutex_init(val->mutex, NULL);
	pthread_cond_init(val->cond, NULL);

	return 0;
}

/**
 * This is a simplification of manually setting the val
 * by calling signal val times.
 */
extern int set_counter(mevt_t *evtc, uint val) {
	evtc->counter = val;
	printf("Evc %d counter: %d\n", evtc->nv, evtc->counter);

	return 0;
}

/**
 * When this is called, if the ticket is equal to counter 
 * then do not block, otherwise block.
 */
extern void mg_await(mevt_t *val, uint ticket) {
	mutex_lock(val->mutex);
	while (ticket > val->counter) {
		pthread_cond_wait(val->cond, val->mutex);
	}
	mutex_unlock(val->mutex);
	pthread_cond_broadcast(val->cond);
}

extern void mg_signal(mevt_t *val) {
	val->counter += 1;
	pthread_cond_broadcast(val->cond);
}

extern uint seq_init(mseq_t *val) {
	val->counter = 0; 
	val->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(val->mutex, NULL);

	return 0;
}

extern uint ticket(mseq_t *val) {
	uint retv;

	mutex_lock(val->mutex);

	// CS Start
	retv = val->counter++;
	assert(retv == val->counter-1);
	// CS End

	mutex_unlock(val->mutex);

	return retv;
}
