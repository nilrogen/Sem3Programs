#include "bm_server.h"

static int shmid;
static int semid[2];

static ring_t *ring;

static void sighandler(int);
static int setsignals();
static int setupsocket();
static int setupshm();
static int setupsem();

static int handle_semop(int group, int type, int opv) {
	int tmp;
	struct sembuf opbuf;

	opbuf.sem_num = type;
	opbuf.sem_op  = opv;
	opbuf.sem_flg = 0;

	while ((tmp = semop(group, &opbuf, 1)) == -1 && errno == EINTR) ; 
	if (tmp == -1) {
		perror("semop");
		return -1;
	}
	return 0;
}

// Sem ops
static int p(int group, int type) {
	return handle_semop(group, type, -1);
}

static int v(int group, int type) {
	return handle_semop(group, type, 1);
}

static int produce(int type) {
	int rval; 

	p(semid[0], type); // P(producer)
	rval = ring->pvalue[type];
	ring->buffer[type][ring->p_in[type]] = rval;
	ring->pvalue[type]++;
	ring->p_in[type] = (ring->p_in[type] + 1) % N_ITEMS;
	v(semid[1], type); // V(consumer)

	int i, j;
	for (i = 0; i < N_RINGS; i++) {
		for (j = 0; j < N_ITEMS; j++) {
			printf("%d ", ring->buffer[i][j]);
		}
		printf("\n");
	}

	return rval;
}
static int consume(int type) {
	int rval;
	if (setupshm() == -1) {
		return -1;
	}

	printf("CONSUME: %d\n", type);

	p(semid[1], type); // P(consumer)
	rval = ring->buffer[type][ring->p_out[type]];
	ring->p_out[type] = (ring->p_out[type] + 1) % N_ITEMS;
	v(semid[0], type); // V(producer);

	printf("CONSUME DONE: %d\n", type);

	int i, j;
	printf("\n");
	for (i = 0; i < N_RINGS; i++) {
		for (j = 0; j < N_ITEMS; j++) {
			printf("%d ", ring->buffer[i][j]);
		}
		printf("\n");
	}

	return rval;
}

int child_main(int sockfd) {
	int tmp;
	bmmsg_t msg; 

	if (setupshm() == -1 || setupsem() == -1) {
		return -1;
	}

	while ((tmp = read(sockfd, &msg, sizeof(msg))) == -1 && errno == EINTR) ;
	if (tmp == -1) {
		perror("BM_SERVER-CHILD: Failed to read");
		return -1;
	}
	msg = bmm_ntoh(msg);

	switch(msg.mtype) {
	case BMPRODUCE: tmp = produce(msg.ring); break;
	case BMCONSUME: tmp = consume(msg.ring); break;
	default: tmp = -1; break;
	}
	if (tmp != -1) {
		msg = bmm_hton(BMREPLY, msg.ring, tmp, BMSUCCESS);
	}
	else {
		msg = bmm_hton(BMREPLY, msg.ring, -1, BMINTERNAL);
	}

	while ((tmp = write(sockfd, &msg, sizeof(msg))) == -1 && errno == EINTR) ; 
	if (tmp == -1) {
		perror("BM_SERVER-CHILD : Failed to send");
		shutdown(sockfd, SHUT_RDWR);
		return -1;
	}

	shutdown(sockfd, SHUT_RDWR);

	return 0;
}


int main(int argc, char *argv[]) {
	int pid, sockfd, csockfd;
	socklen_t csize;
	struct sockaddr caddr;

	if (setsignals() == -1 || (sockfd = setupsocket()) == -1) {
		exit(1);
	}
	if (setupshm() == -1) {
		sighandler(-1);
	}
	if (setupsem() == -1) {
		sighandler(-1);
	}



	while (1) {
		// Accept Loop
		while ((csockfd = accept(sockfd, &caddr, &csize)) == -1
				&& errno == EINTR) ; // Handle Interrupts
		if (csockfd == -1) {
			perror("Failed to handle accept");	
			sighandler(-1);
		}


		switch (pid = fork()) {
		default: // The Parent
			close(csockfd);
			break;
		case -1: // Error
			perror("Fork Failed");
			sighandler(-1);
		case 0: // The kid
			child_main(csockfd);
			exit(1);
		}
	}
	sighandler(-1);	
}

void sighandler(int signal) {
	int i; 

	fprintf(stderr, "BufferManager signal handler with signal #%d.\n", 
			signal);

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("Failed to remove shm");
	}
	for (i = 0; i < 2; i++) {
		if (semctl(semid[i], IPC_RMID, 0) == -1) {
			perror("semctl");
		}
	}

	exit(5);
}

int setsignals() {
	sigset_t mask; 
	struct sigaction new;

	int i, nsigs;
	int sigs[] = { SIGTERM, SIGHUP, SIGINT, SIGQUIT, SIGBUS, SIGSEGV, SIGFPE };

	/* Setup Zombie Prevention */
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	new.sa_handler = SIG_IGN;
	new.sa_mask = mask;
	new.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &new, NULL) == -1) {
		perror("Failed to setup SIGCHLD signal");
		return -1;
	}

	nsigs = sizeof(sigs) / sizeof(int);

	/* Handle Regular Signals */
	sigemptyset(&mask);
	for (i = 0; i < nsigs; i++)
		sigaddset(&mask, sigs[i]);

	for (i = 0; i < nsigs; i++) {
		new.sa_handler = sighandler;
		new.sa_mask = mask;
		new.sa_flags = 0;

		if (sigaction(sigs[i], &new, NULL) == -1) {
			perror("Cannot set signals");
			return -1;
		}
	}
	return 0;
}

int setupsocket() {
	int sockfd;
	struct sockaddr_in saddr;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT_BUFFERMANAGER);
	inet_pton(AF_INET, HOST_IP, &saddr.sin_addr);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Failed to create socket");
		return -1;
	}
	if (bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) == -1) {
		perror("Failed to bind socket");
		return -1;
	}
	if (listen(sockfd, 5) == -1) {
		perror("listen failed");
		return -1;
	}
	return sockfd;
}

int setupsem() {
	int i, j;
	union semun sc;

	sc.val = N_ITEMS; // semid[0] producer
	for (i = 0; i < 2; i++) {
		if ((semid[i] = semget(SEM_KEY+i, N_RINGS, IPC_CREAT | 0600)) == -1) {
			perror("semget");
			return -1;
		}
		for (j = 0; j < N_RINGS; j++) {
			if (semctl(semid[i], j, SETVAL, sc) == -1) {
				perror("semctl");	
				return -1;
			}
		}
		sc.val = 0; // semid[1] consumer
	}
	return 0;
}

int setupshm() {
	shmid = shmget(SHM_KEY, sizeof(ring_t), IPC_CREAT | 0600);
	if (shmid == -1) {
		perror("shmget failed");
		return -1;
	}

	ring = (ring_t *) shmat(shmid, NULL, 0);
	if (ring == (void *) -1) {
		perror("Failed to attach shm");
		return -1;
	}
	return 0;
}

