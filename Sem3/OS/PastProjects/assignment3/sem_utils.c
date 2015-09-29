#include "assignment3.h"

extern int p(int semidgroup, int donut_type) {
	struct sembuf semopbuf;

	semopbuf.sem_num = donut_type;
	semopbuf.sem_op = (-1);
	semopbuf.sem_flg = 0;

	if (semop(semidgroup, &semopbuf, 1) == -1) {
		return -1;
	}

	return 0;
}


extern int v(int semidgroup, int donut_type) {
	struct sembuf semopbuf;

	semopbuf.sem_num = donut_type;
	semopbuf.sem_op = 1;
	semopbuf.sem_flg = 0;

	if (semop(semidgroup, &semopbuf, 1) == -1) {
		return -1;
	}

	return 0;
}


extern int semsetall(int semgroup, int num_in_group, int set_all_value) {
	int i;
	union semun sem_ctl_un;
	
	sem_ctl_un.val = set_all_value;
	for (i = 0; i < num_in_group; i++) {
		if (semctl(semgroup, i, SETVAL, sem_ctl_un) == -1) 
			return -1;
	}

	return 0;
}


