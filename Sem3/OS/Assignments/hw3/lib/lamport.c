#include <lamport.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

extern lmsg_t hmsgton(lmsg_t msg) {
	msg.type    = htonl(msg.type);
	msg.clock   = htonl(msg.clock);
	msg.node    = htonl(msg.node);
	msg.pid     = htonl(msg.pid);
	msg.replies = htonl(msg.replies);
	
	return msg;
}

extern lmsg_t nmsgtoh(lmsg_t msg) {
	msg.clock   = ntohl(msg.clock);
	msg.node    = ntohl(msg.node);
	msg.type    = ntohl(msg.type);
	msg.pid     = ntohl(msg.pid);
	msg.replies = ntohl(msg.replies);

	return msg;
}


extern lmp_mutex_t *lmp_mutex_create(int node) {
	lmp_mutex_t *retv; 

	retv = (lmp_mutex_t *) malloc(sizeof(lmp_mutex_t)); 
	if (retv == NULL) {
		return NULL;
	}

	retv->clock = 0;
	retv->node = node;
	if (pthread_mutex_init(&retv->lock, NULL) != 0) {
		free(retv);
		return NULL;
	}

	retv->queue = (mqueue *) malloc(sizeof(mqueue));
	if (retv->queue == NULL) {
		return NULL;
	}
	if (mqueue_init(retv->queue) == -1) {
		free(retv->queue);
		free(retv);
		return NULL;
	}

	return retv;
}

extern int lmp_mutex_destroy(lmp_mutex_t *mutex) {
	if (mutex == NULL) {
		return 0;
	}
	if (mutex->queue == NULL) {
		free(mutex); 
		return 0;
	}

	mqueue_destroy(mutex->queue);
	pthread_mutex_destroy(&mutex->lock); 

	// mqueue destroy doesnt free the space.
	free(mutex->queue); 
	free(mutex);
	return 0;
}


extern int handle_request(lmp_mutex_t *mutex, lmsg_t msg) {
	struct mlist_iter iter;
	lmsg_t *val, *addv;
	int retv;
	int eval = 0;

	addv = (lmsg_t *) malloc(sizeof(lmsg_t));
	if (addv == NULL || mutex == NULL || mutex->queue == NULL) {
		return -1;
	}
	if (msg.type !=  LREQUEST) {
		return -1;
	}
	*addv = msg;

	pthread_mutex_lock(&mutex->lock);

	mutex->clock++;
	if (addv->clock == -1) {
		addv->clock = mutex->clock;
	}

	// Handle self enqueue of message.
	if (msg.node == mutex->node) {
		addv->clock = mutex->clock;
		if (enqueue(mutex->queue, (void *) addv) == -1) {
			fprintf(stderr, "Failed to enqueue.\n");
			pthread_mutex_unlock(&mutex->lock); 
			return -1;
		}

		retv = mutex->clock;
		pthread_mutex_unlock(&mutex->lock); 

		return retv;
	}

	// Handle request with clock value >= own clock value
	// Or when queue is empty
	if (msg.clock >= mutex->clock || mutex->queue->head == NULL) {
		mutex->clock = msg.clock;
		if (enqueue(mutex->queue, (void *) addv) == -1) {
			fprintf(stderr, "Failed to enqueue.\n");
			pthread_mutex_unlock(&mutex->lock); 
			return -1;
		}

		retv = mutex->clock;

		pthread_mutex_unlock(&mutex->lock); 

		return retv;
	}

	// Handle requests with clock value < own clock value
	// by iterating through the list inserting request at the
	// correct position.

	// Init iterator.
	if (mlist_iter_init(mutex->queue, &iter) == -1) {
		pthread_mutex_unlock(&mutex->lock);
		return -1;
	}

	val = DATA(current(&iter));
	while (!end(&iter)) {

		// IF request comes before iterator's element
		if (msg.clock < val->clock) { 
			// If his is before the head insert at the front
			if (iter.position == 0) {
				eval = mlist_insert(mutex->queue, (void *) addv);
			}
			else { // Otherwise add before the iterator
				eval = mlist_add_before(mutex->queue, current(&iter), addv);
			}
			break;
		}
		// If the clock values are the same look at the node values
		else if (msg.clock == val->clock) {
			if (msg.node < val->node) { // When Node number is smaller
				eval = mlist_add_before(mutex->queue, current(&iter), addv);
				break;
			}
		}
		val = DATA(next(&iter));
	}
	retv = mutex->clock;
	pthread_mutex_unlock(&mutex->lock);

	if (eval == -1) {
		fprintf(stderr, "Failed to add to list.\n");
		return -1;
	}
	
	return retv;
}

extern int handle_reply(lmp_mutex_t *mutex, lmsg_t msg) {
	struct mlist_iter iter;
	lmsg_t *val, *candidate = NULL;


	if (mutex == NULL || mutex->queue == NULL) {
		return -1;
	}

	pthread_mutex_lock(&mutex->lock);

	mutex->clock++;

	if (mlist_iter_init(mutex->queue, &iter) == -1) {
		fprintf(stderr, "Iterator failed to init.\n");
		pthread_mutex_unlock(&mutex->lock);
		return -1;
	}

	// Find the largest message k st :
	// (1) k.node = mutex->node
	// (2) k.clock <= m.clock
	val = DATA(current(&iter));
	while (!end(&iter)) {
		if (val->node == mutex->node && val->clock <= msg.clock) {
			candidate = val;
			if (val->clock == msg.clock)
				break;
		}
		val = DATA(next(&iter));
	}

	
	if (candidate == NULL) {
		pthread_mutex_unlock(&mutex->lock);
		fprintf(stderr, "Failed to find message to reply to.\n");
		return -1;
	}
	candidate->replies++;

	pthread_mutex_unlock(&mutex->lock);

	return 0;
}


extern lmsg_t *handle_release(lmp_mutex_t *mutex) {
	lmsg_t *retv; 
	if (mutex == NULL || mutex->queue == NULL) {
		return NULL;
	}

	pthread_mutex_lock(&mutex->lock);
	retv = DATA(dequeue(mutex->queue)); 

	// The return value is ignored in the rcv_threads
	// But the return value is sent in the snd_thread
	// SO we update the clock and type atomically
	// here.
	mutex->clock++;
	retv->clock = mutex->clock;
	retv->type = LRELEASE;
	pthread_mutex_unlock(&mutex->lock);

	return retv;
}

