#include "bm_server.h"

static int shmid;
static ring_t *ring;

static void sighandler(int);
static int setsignals();
static int setupsocket();
static int setupshm();

static int produce(int type) {
	if (setupshm() == -1) {
		return -1;
	}

	ring->buffer[type][ring->p_in[type]] = ring->pvalue[type];
	ring->pvalue[type]++;
	ring->p_in[type] = (ring->p_in[type] + 1) % N_ITEMS;

	int i, j;
	for (i = 0; i < N_RINGS; i++) {
		for (j = 0; j < N_ITEMS; j++) {
			printf("%d ", ring->buffer[i][j]);
		}
		printf("\n");
	}

	return 0;
}
static int consume(int type);

int child_main(int sockfd) {
	int tmp;
	bmmsg_t msg; 

	while ((tmp = read(sockfd, &msg, sizeof(msg))) == -1 && errno == EINTR) ;
	if (tmp == -1) {
		perror("BM_SERVER-CHILD: Failed to read");
		return -1;
	}
	msg = bmm_ntoh(msg);

	switch(msg.mtype) {
	case BMPRODUCE: 
		tmp = produce(msg.ring);
		if (tmp != -1)
			msg = bmm_hton(BMREPLY, msg.ring, tmp, BMSUCCESS);
		else 
			msg = bmm_hton(BMREPLY, msg.ring, -1, BMINTERNAL);
		break;
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
	printf("Signal Handler with signal #%d.\n", signal);

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("Failed to remove shm");
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

