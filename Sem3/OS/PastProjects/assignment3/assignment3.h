#ifndef __ASSIGNMENT3__
#define __ASSIGNMENT3__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/signal.h>

#define		SEMKEY			(key_t)549351763
#define		MEMKEY			(key_t)549351763
#define		NUMFLAVORS	 	4
#define		NUMSLOTS     	50
#define		NUMSEMIDS	 	3
#define		PROD		 	0
#define		CONSUMER	 	1
#define		MUTEX		 	2

struct donut_ring {
	int	flavor [NUMFLAVORS][NUMSLOTS];
	int	outptr [NUMFLAVORS];
};

#ifdef _SEM_SEMUN_UNDEFINED
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};
#endif /* _SEM_SEMUN_UNDEFINED */

extern int p(int, int);
extern int v(int, int);
extern int semsetall(int, int, int);

#endif /* __ASSIGNMENT3__ */


