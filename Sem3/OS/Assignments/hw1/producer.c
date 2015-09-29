#include "assignment1.h"

#define PRINT 1


static donut_t *p_data;

void signalHandler(int);
void signalSetup();

int main(int argc, char *argv[]) {
	int fd, pdonut, i;
	int counter[D_TYPES], p_in[D_TYPES];

	ushort xsub1[3];
	struct timeval randtime;

	signalSetup();

	// Setup shared memory region.
	// Functions exit on failure. 
	fd = setupSHM(TRUE);
	p_data = getSHM(fd, TRUE);
	close(fd);
	if (p_data == MAP_FAILED) {
		signalHandler(-1);
	}

	setupSemaphores(p_data);

	// Setup nrand48
	gettimeofday(&randtime, (struct timezone *) 0);
	xsub1[0] = (ushort) randtime.tv_usec;
	xsub1[1] = (ushort) randtime.tv_usec >> 16;
	xsub1[2] = (ushort) getpid();


	for (i = 0; i < D_TYPES; i++) {
		p_in[i] = 0;
		counter[i] = 1;
	}

	while (1) {
		// get type
		pdonut = nrand48(xsub1) % D_TYPES;

		Pin(p_data, pdonut);
		Pmutex(p_data, pdonut);

		// CS
		p_data->p_flavors[pdonut][p_in[pdonut]] = counter[pdonut];
		++counter[pdonut];
		p_in[pdonut] = (p_in[pdonut] + 1) % D_SIZE;

		Vmutex(p_data, pdonut);
		Vout(p_data, pdonut);
	}

	destroySemaphores(p_data);
	destroySHM(p_data);
	
	return 0;
}

void signalHandler(int error) {

	if (PRINT)
		fprintf(stderr, "Signal #%d fired.\n", error);

	destroySemaphores(p_data);
	destroySHM(p_data);

	exit(1);
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


