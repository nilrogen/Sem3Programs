#include "bm.h"

#define SHM_KEY 978888109

static struct {
	int shmid;
	ring_t *ring;
} globals;

void sighandler(int);
int setsignals();
int setupsocket();
int setupshm();

int child_main(int sockfd) {

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
		case -1: // Error
			perror("Fork Failed");
			sighandler(-1);
		case 0: // The kid
			child_main(csockfd);
			exit(1);
		}
	}
	
	return 0;
}

void sighandler(int signal) {
	printf("Signal Handler with signal #%d.\n", signal);

	if (shmctl(globals.shmid, IPC_RMID, 0) == -1) {
		perror("Failed to remove shm");
	}

	exit(5);
}

int setsignals() {
	sigset_t mask; 
	struct sigaction new;

	int i, nsigs;
	int sigs[] = { SIGTERM, SIGHUP, SIGQUIT, SIGTERM, SIGSEGV, SIGFPE };

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
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) != -1) {
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
	globals.shmid = shmget(SHM_KEY, sizeof(ring_t), IPC_CREAT | 0600);
	if (globals.shmid == -1) {
		perror("shmget failed");
		return -1;
	}

	globals.ring = (ring_t *) shmat(globals.shmid, NULL, 0);
	if (globals.ring == (void *) -1) {
		perror("Failed to attach shm");
		return -1;
	}
	return 0;
}

