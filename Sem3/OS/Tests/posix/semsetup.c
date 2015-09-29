#include "header.h"

extern void setupSEM(val_t *val) {
	if (sem_init(&val->sem, 1, 1) == -1)
		perror("FUCK!?");
}
