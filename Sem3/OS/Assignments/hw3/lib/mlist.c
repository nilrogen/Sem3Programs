#include <ds/mlist.h>

static int destroy_list(struct mlist_node *head) {
	return 0;
}

extern int mlist_init(struct mlist *list) {
	if (list == NULL) {
		return -1;
	}
	if (pthread_mutex_init(&list->lock, NULL) != 0) {
		return -1;
	}
	list->head = NULL;
	return 0; 
}

extern int mlist_destroy(struct mlist *list) {
	if (list == NULL) {
		return -1;
	}
	pthread_mutex_lock(&list->lock);
	if (destroy_list(list->head) != 0) {
		return -1;
	}
	pthread_mutex_unlock(&list->lock);

	if(pthread_mutex_destroy(&list->lock) != 0) {
		return -1;
	}
	return 0;
}

extern int mlist_insert(struct mlist *list, void *data) {
	struct mlist_node *val, *tmp;

	if (list == NULL) {
		return -1;
	}

	val = (struct mlist_node *) malloc(sizeof(struct mlist_node));
	if (val == NULL) {
		return -1;
	}
	val->data = data;

	pthread_mutex_lock(&list->lock);
	if (list->head == NULL) {
		list->head = val;
		val->head = val;
		val->tail = val;
	}
	else {
		tmp = list->head->head;
		val->head = tmp;
		val->tail = list->head;
		tmp->tail = val;
		list->head->head = val;
	}
	pthread_mutex_unlock(&list->lock);

	return 0;	
}

extern int mlist_append(struct mlist *list, void *data) {
	struct mlist_node *val, *tmp;
	if (list == NULL) {
		return -1;
	}

	val = (struct mlist_node *) malloc(sizeof(struct mlist_node));
	if (val == NULL) {
		return -1;
	}
	val->data = data;

	pthread_mutex_lock(&list->lock);
	if (list->head == NULL) {
		list->head = val;		
		val->head = val;
		val->tail = val;
	}
	else {
		tmp = list->head->tail;
		val->head = list->head;
		val->tail = tmp;
		tmp->head = val;
		list->head->tail = val;
	}
	pthread_mutex_unlock(&list->lock);

	return 0;
}

extern int mlist_iter_init(struct mlist *list, struct mlist_iter *iter) {
	if (list == NULL || iter == NULL) {
		return -1;
	}
	iter->list = list;
	iter->current = list->head;
	return 0;
}

static struct mlist_node *iterate(struct mlist_iter *iter, int dir) {
	struct mlist_node *retv; 

	pthread_mutex_lock(&iter->list->lock);
	if (iter->list->head == NULL) {
		return NULL;
	}
	if (dir) {
		retv = iter->current->head;
	}
	else {
		retv = iter->current->tail;
	}
	 
	iter->current = retv;
	pthread_mutex_unlock(&iter->list->lock);

	return retv;
}


extern struct mlist_node *next(struct mlist_iter *iter) {
	return iterate(iter, 1);
}

extern struct mlist_node *prev(struct mlist_iter *iter) {
	return iterate(iter, 0);
}
