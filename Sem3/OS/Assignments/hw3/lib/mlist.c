#include <ds/mlist.h>

static struct mlist_node *nonatomic_delete_at(struct mlist *, int);

extern int mlist_init(struct mlist *list) {
	if (list == NULL) {
		return -1;
	}
	if (pthread_mutex_init(&list->lock, NULL) != 0) {
		return -1;
	}
	list->length = 0; 
	list->head = NULL;
	return 0; 
}

extern int mlist_destroy(struct mlist *list) {
	struct mlist_node *val;
	
	pthread_mutex_lock(&list->lock);
	while (list->head != NULL) {
		val = nonatomic_delete_at(list, 0);
		free(val->data);
		free(val);
	}
	pthread_mutex_unlock(&list->lock);

	pthread_mutex_destroy(&list->lock);
	return 0;
}

extern int mlist_add_after(struct mlist *list, struct mlist_node *node, 
						   void *data) {

	struct mlist_node *val;

	if (list == NULL || node == NULL) {
		return -1;
	}

	val = (struct mlist_node *) malloc(sizeof(struct mlist_node));
	if (val == NULL) {
		return -1;
	}
	val->data = data;

	pthread_mutex_lock(&list->lock); 
	val->tail = node; 
	val->head = node->head;
	node->head->tail = val;
	node->head = val;


	list->length++;
	pthread_mutex_unlock(&list->lock);

	return 0;
}

extern int mlist_add_before(struct mlist *list, struct mlist_node *node, 
							void *data) {
	
	return mlist_add_after(list, node->tail, data);
}

static int add_to_list(struct mlist *list, void *data, int type) {
	struct mlist_node *val, *tmp;

	if (list == NULL) {
		return -1;
	}

	val = (struct mlist_node *) malloc(sizeof(struct mlist_node));
	if (val == NULL) {
		return -1;
	}
	val->data = data;

	// insert -> front of list
	// append -> back of lsit
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
	if (type) { // Insert (change head)
		list->head = val;
	} 
	list->length += 1;
	pthread_mutex_unlock(&list->lock);

	return 0;	
}

extern int mlist_insert(struct mlist *list, void *data) {
	return add_to_list(list, data, 1);
}

extern int mlist_append(struct mlist *list, void *data) {
	return add_to_list(list, data, 0);
}

static struct mlist_node *nonatomic_delete_at(struct mlist *list, int index) {
	struct mlist_node *retv;

	if (list == NULL) {
		return NULL;
	}

	retv = list->head;
	if (retv == NULL) {
		return NULL;
	}
	while (index != 0) {
		if (index < 0) {
			retv = retv->tail;
			index++;
		}
		else {
			retv = retv->head;
			index--;
		}
	}
	
	list->length -= 1;
	if (retv == list->head) {
		list->head = retv->head;
	}
	if (retv->tail == retv->head) {
		list->head = NULL;
		retv->head = NULL;
		retv->tail = NULL;
		return retv;
	}

	retv->tail->head = retv->head;
	retv->head->tail = retv->tail;

	retv->head = NULL;
	retv->tail = NULL;

	return retv;
}

extern struct mlist_node *mlist_delete_at(struct mlist *list, int index) {
	struct mlist_node *retv;
	if (list == NULL) {
		return NULL;
	}
	
	pthread_mutex_lock(&list->lock);
	retv = nonatomic_delete_at(list, index);
	pthread_mutex_unlock(&list->lock);

	return retv;
}

extern struct mlist_node *mlist_delete_front(struct mlist *list) {
	return mlist_delete_at(list, 0);	
}

extern struct mlist_node *mlist_delete_back(struct mlist *list) {
	return mlist_delete_at(list, -1);	
}

extern int mlist_iter_init(struct mlist *list, struct mlist_iter *iter) {
	if (list == NULL || iter == NULL || list->head == NULL) {
		return -1;
	}
	iter->position = 0;
	iter->list = list;
	iter->current = list->head;
	return 0;
}

static struct mlist_node *iterate(struct mlist_iter *iter, int type) {
	struct mlist_node *retv; 

	pthread_mutex_lock(&iter->list->lock);
	if (iter->list->head == NULL) {
		return NULL;
	}
	switch (type) {
	case 0:  retv = iter->current->tail; iter->position--; break;
	case 1:  retv = iter->current->head; iter->position++; break;
	default: retv = iter->current; 
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

extern struct mlist_node *current(struct mlist_iter *iter) {
	return iterate(iter, 2);
}

extern int end(struct mlist_iter *iter) {
	return abs(iter->position) == iter->list->length;
}
