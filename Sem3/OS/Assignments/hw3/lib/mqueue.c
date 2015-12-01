#include <ds/mqueue.h>
#include <as3util.h>

extern int mqueue_init(mqueue *queue) {
	return mlist_init(queue);
}
extern int mqueue_destroy(mqueue *queue) {
	return mlist_destroy(queue);
}

extern struct mlist_node *peek(mqueue *queue) {
	struct mlist_node *ret;
	ATOMIC_GET(queue->lock, queue->head, ret);

	return ret;
}

extern int enqueue(mqueue *queue, void *val) {
	return mlist_append(queue, val);		
}

extern struct mlist_node *dequeue(mqueue *queue) {
	return mlist_delete_front(queue);	
}



