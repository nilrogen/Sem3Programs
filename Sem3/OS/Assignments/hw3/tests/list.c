#include <stdlib.h>
#include <stdio.h>

#include <ds/mlist.h>

void add(int val, struct mlist *list, int type) {
	int *toinsert = (int *) malloc(sizeof(int));
	*toinsert = val;

	if (type)
		mlist_append(list, (void *) toinsert);
	else
		mlist_insert(list, (void *) toinsert);
}

int main(int argc, char *argv[]) {
	struct mlist *list;
	struct mlist_iter *iter;
	struct mlist_node *current;
	int i; 

	list = (struct mlist *) malloc(sizeof(struct mlist));
	iter = (struct mlist_iter *) malloc(sizeof(struct mlist_iter));

	mlist_init(list);

	for (i = 0; i < 10; i++) {
		add(i, list, 1);
	}

	mlist_iter_init(list, iter);

	current = iter->current;
	do {
		printf("%d\n", *(int *) current->data);
		current = next(iter);
	} while (current != list->head);

	printf("DELETION----\n");
	current = mlist_delete_front(list);
	printf("Front %d\n", *(int *) current->data);
	free(current);
	current = mlist_delete_back(list);
	printf("Back %d\n", *(int *) current->data);
	free(current);
	current = mlist_delete_at(list, 5);
	printf("5th %d\n", *(int *) current->data);
	free(current);

	mlist_iter_init(list, iter);

	current = iter->current;
	do {
		printf("%d\n", *(int *) current->data);
		current = next(iter);
	} while (current != list->head);

	mlist_destroy(list);

	free(iter);
	free(list);
	return 0;
}

