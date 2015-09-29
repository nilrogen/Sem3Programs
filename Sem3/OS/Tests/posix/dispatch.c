#include "header.h"

static struct {
	int shmfd;
	val_t *p_data;
} important;


void signalHandler(int);
void signalSetup();

int main(int argc, char *argv[]) {
	val_t *data; 
	int fd;
	

	signalSetup();

	fd = setupSHM(TRUE);
	data = getSHM(fd, TRUE);

	important.shmfd = fd;
	important.p_data = data;

	printf("DATA: %d\n", data->value);

	if (fork() == 0) {
		execl("p1", "p1", NULL);
		printf("FUCK!\n");
		exit(1);
	}

	while (data->value == 0)
		;

	data->value = 2;

	printf("DATA: %d\n", data->value);

	signalHandler(-1);
	return 0;
}

void signalHandler(int error) {

	fprintf(stderr, "Signal #%d fired.\n", error);

	// UNLINK STUFF
	destroySHM(important.p_data, important.shmfd);

	// KILL SELF! TODO

	exit(5);
	   	
}

void signalSetup(void) {
	int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGBUS, 
					  SIGTERM, SIGSEGV, SIGFPE };
	int i, nsigs; 

	struct sigaction new_action;
	sigset_t sig_mask;

	nsigs = sizeof(signals) / sizeof(int);

	sigemptyset(&sig_mask);
	for (i = 0; i < nsigs; i++) {
		sigaddset(&sig_mask, signals[i]);
	}

	for (i = 0; i < nsigs; i++) {
		new_action.sa_handler = signalHandler;
		new_action.sa_mask    = sig_mask;
		new_action.sa_flags   = 0;

		if (sigaction(signals[i], &new_action, NULL) == -1) {
			perror("Failed to set signal handler: ");
			exit(1);
		}
	}
}
