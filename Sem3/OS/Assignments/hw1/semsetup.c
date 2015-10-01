/**
 * Author: Michael Gorlin
 * Date: 2015-10-01
 *
 * This file contains functions definitions to set up the 12 semaphores
 * needed for the POSIX implementation of assignment 1.
 */
#include "assignment1.h"


extern int setupSemaphores(donut_t *val) {
	int i, j;
	// format: [ insem, outsem, mutex ]
	int args[] = { D_SIZE, 0, 1 };
	for (i = 0; i < D_TYPES; i++) {
		for (j = 0; j < 3; j++) {
			if (sem_init(&val->sems[i][j], 1, args[j]) == -1) {
				perror("Semaphore failed to initialize");
				return -1;
			}
		}
	}
	return 0;
}

extern int destroySemaphores(donut_t *val) {
	int i, j;
	for (i = 0; i < D_TYPES; i++) {
		for (j = 0; j < 3; j++) {
			if (sem_destroy(&val->sems[i][j]) == -1) {
				perror("Semaphore is invalid");
				return -1;
			}
		}
	}
	return 0;
}

