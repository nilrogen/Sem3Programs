#include "assignment3.h"

int main(int argc, char *argv[]) {
	struct donut_ring *shared_ring;
	int i, j;
	int consumer_num = atoi(argv[1]);
	int shmid, semid[3];
	int donuts[120][2];
	struct timeval randtime;
	unsigned short xsub1[3];

	if ((shmid = shmget(MEMKEY, sizeof(struct donut_ring), 0)) == -1) {
		perror("Shared get failed: ");
		exit(1);
	}
	if ((shared_ring = (struct donut_ring *) shmat(shmid, NULL, 0)) == NULL) {
		perror("Shared attach failed: ");
		exit(1);
	}
	for (i = 0; i < NUMSEMIDS; i++) {
		if ((semid[i] = semget(SEMKEY+i, NUMFLAVORS, 0)) == -1) {
			perror("Semaphore get failed: ");
			exit(1);
		}
	}	

	gettimeofday(&randtime, (struct timezone *) 0);				
	xsub1[0] = (ushort) randtime.tv_usec;
	xsub1[1] = (ushort) (randtime.tv_usec >> 16);
	xsub1[2] = (ushort) (getpid());


	/*
	 * p(consumer)
	 * p(mutex)
	 * grab donut
	 * v(mutex)
	 * v(producer)
	 */
	for (i = 0; i < 120; i++) {
		j = nrand48(xsub1) % NUMFLAVORS; /* gen random donut */
		p(semid[CONSUMER], j);
		p(semid[MUTEX], j);
		gettimeofday(&randtime, (struct timezone *) 0);				
		donuts[i][0] = shared_ring->flavor[j][shared_ring->outptr[i]];
		shared_ring->outptr[i] = (shared_ring->outptr[i] + 1) % NUMSLOTS;
		donuts[i][1] = j;
		v(semid[MUTEX], j);
		v(semid[PROD], j);
	}

	gettimeofday(&randtime, (struct timezone *) 0);				
	printf("\nConsumer %d Done:", (int)consumer_num);
	printf("\nPID: %d, Time: %d", getpid(), randtime.tv_sec);
	printf("\n\nDonut order by dozen:");
	for (i = 0; i < 120; i++) {
		if (!(i % 12))
			printf("\nDozen %d:", i % 10);
		printf(" (%d, %d)", donuts[i][1], donuts[i][0]);
	}
	printf("\n\n");

	return 0;
}
