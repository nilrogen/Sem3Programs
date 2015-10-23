/**
 * Author: Michael Gorlin
 */
#include "assignment2.h"
#include "evseq.h"

static inline int mutex_lock(pthread_mutex_t *);
static inline int mutex_unlock(pthread_mutex_t *);


extern int evtc_init(mevt_t *val) {
	val->counter = 0; 
	val->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	val->cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_mutex_init(val->mutex, NULL);
	pthread_cond_init(val->cond, NULL);

	return 0;
}

extern void mg_await(mevt_t *val, int ticket) {
	mutex_lock(val->mutex);
	
	while (ticket > val->counter) {
		pthread_cond_wait(val->cond, val->mutex);
	}
	mutex_unlock(val->mutex);
	pthread_cond_broadcast(val->cond);
}

extern void mg_advance(mevt_t *val) {
	val->counter += 1;
	pthread_cond_broadcast(val->cond);
}


extern int seq_init(mseq_t *val) {
	val->counter = 0; 
	val->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(val->mutex, NULL);

	return 0;
}

extern int ticket(mseq_t *val) {
	int retv;

	mutex_lock(val->mutex);
	retv = val->counter++;
	assert(retv == val->counter-1);

	mutex_unlock(val->mutex);

	return retv;
}

static inline int mutex_lock(pthread_mutex_t *mutex) {
	if (pthread_mutex_lock(mutex) != 0) {
		perror("Lock Error: ");
	}
	return 0;
}

static inline int mutex_unlock(pthread_mutex_t *mutex) {
	if (pthread_mutex_unlock(mutex) != 0) {
		perror("Unlock Error");
	}
	return 0;
}
