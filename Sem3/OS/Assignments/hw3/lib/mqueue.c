#include <ds/mqueue.h>

extern struct mlist_node *peek(mqueue *queue) {
	struct mlist_node *ret;
	pthread_mutex_lock(&queue->lock);
	ret = queue->head;	
	pthread_mutex_unlock(&queue->lock);

	return ret;
}

extern int enqueue(mqueue *queue, void *val) {
	return mlist_append(queue, val);		
}

extern struct mlist_node *dequeue(mqueue *queue) {
	return mlist_delete_front(queue);	
}



