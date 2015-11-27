#include "bm_server.h"

static struct {
	int shmid;

} globals;

void sighandler(int signal) {
	printf("Signal Handler with signal #%d.\n", signal);

	if (shmctl(globals.shmid, IPC_RMID, 0) == -1) {
		perror("Failed to remove shm");
	}

	exit(5);
}

void set_signals() {
	sigset_t mask; 
	struct sigaction new;

	int i, nsigs;
	int sigs[] = { SIGTERM, SIGHUP, SIGQUIT, SIGTERM, SIGSEGV, SIGFPE };

	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	new.sa_handler = SIG_DFL;
	new.sa_mask = mask;
	new.sa_flags = SA_NOCLDWAIT;
	if (sigaction(SIGCHLD, &new, NULL) == -1) {
		perror("Failed to setup SIGCHLD signal");
		exit(1);
	}

	nsigs = sizeof(sigs) / sizeof(int);

	sigemptyset(&mask);
	for (i = 0; i < nsigs; i++)
		sigaddset(&mask, sigs[i]);

	for (i = 0; i < nsigs; i++) {
		new.sa_handler = sighandler;
		new.sa_mask = mask;
		new.sa_flags = 0;

		if (sigaction(sigs[i], &new, NULL) == -1) {
			perror("Cannot set signals");
			exit(1);
		}
	}

}

int main(int argc, char *argv[]) {


	while (1) {
		// ACCEPT

		switch (pid = fork()) {
		default: // The Parent
		case -1: // Error
			sighandler(-1);
		case 0: // The kid


		}

	}
	
	return 0;
}


