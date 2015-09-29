#include "assignment3.h"

int shmid, semid[3];

void sig_handler(int);
void set_signals(void);

int main(int argc, char *argv[]) {
	int in_ptr[NUMFLAVORS];
	int serial[NUMFLAVORS];
	int i;

	struct donut_ring *shared_ring;
	struct timeval randtime;

	unsigned short xsub1[3];

	set_signals(); /* set signal handler */

	for (i = 0; i < NUMFLAVORS; i++) {
		in_ptr[i] = 0;
		serial[i] = 0;
	}

	if ((shmid = shmget(MEMKEY, sizeof(struct donut_ring), 
						IPC_CREAT | 0600)) == -1) {
		perror("Shared get failed: ");
		exit(1);
	}

	if ((shared_ring = (struct donut_ring *) shmat(shmid, NULL, 0)) == -1) {
		perror("Shared attach failed: ");
		sig_handler(-1);
	}

	for(i = 0; i<NUMSEMIDS; i++)
		if ((semid[i] = semget (SEMKEY+i, NUMFLAVORS, 
					 IPC_CREAT | 0600)) == -1){
		perror("semaphore allocation failed: ");
		sig_handler(-1);
	}


	/* Allocation of time seed */	
	gettimeofday(&randtime, (struct timezone *) 0);				
	xsub1[0] = (ushort) randtime.tv_usec;
	xsub1[1] = (ushort) (randtime.tv_usec >> 16);
	xsub1[2] = (ushort) (getpid());

	nrand48(xsub1);

	/* set semaphores */
	if (semsetall(semid[PROD], NUMFLAVORS, NUMSLOTS) == -1) {
		perror("semsetsall failed: ");
		sig_handler(-1);
	}
	if (semsetall(semid[CONSUMER], NUMFLAVORS, 0) == -1) {
		perror("semsetsall failed: ");
		sig_handler(-1);
	}
	if (semsetall(semid[MUTEX], NUMFLAVORS, 1) == -1) {
		perror("semsetsall failed: ");
		sig_handler(-1);
	}


	
	/* 
	 * p(producer)
	 * p(mutex)
	 * make donut
	 * v(mutex)
	 * v(consumer)
	 */
	while (1)  {
		i = nrand48(xsub1) % NUMFLAVORS; /* get random flavor */
		p(semid[PROD], i);
		p(semid[MUTEX], i);
		gettimeofday(&randtime, (struct timezone *) 0);
		shared_ring->flavor[i][in_ptr[i]] = serial[i]; /* add serial[i] to ring buffer */
		serial[i]++;
		in_ptr[i] = (in_ptr[i] + 1) % NUMSLOTS;
		v(semid[MUTEX], i);
		v(semid[CONSUMER], i);
	}

	sig_handler(-1);
	return 0; /* Will never reach */
}

void sig_handler(int signal) {
	int i;

	printf("In signal handler with signal #%d.\n", signal);

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("handler failed to shm RMID: ");
	}

	for (i = 0; i < NUMSEMIDS; i++) {
		if (semctl(semid[i], 0, IPC_RMID, (union semun) 0) == -1) {
			perror("Handler failed to sem RMID: ");
		}
	}

	exit(5);
}

void set_signals(void) {
	sigset_t mask_sigs;
	struct sigaction new;

	int i, nsigs;
	int sigs[] = { SIGHUP, SIGINT, SIGQUIT, SIGBUS, SIGTERM, SIGSEGV, SIGFPE };

	nsigs = sizeof(sigs) / sizeof(int);

	sigemptyset(&mask_sigs);
	for (i = 0; i < nsigs; i++)
		sigaddset(&mask_sigs, sigs[i]);

	for (i = 0; i < nsigs; i++) {
		new.sa_handler = sig_handler;
		new.sa_mask = mask_sigs;
		new.sa_flags = 0;

		if (sigaction(sigs[i], &new, NULL) == -1) {
			perror("Cannot set signals: ");
			exit(1);
		}
	}
}



