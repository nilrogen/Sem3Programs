#include "assignment2.h"
#include "evseq.h"

static mevt_t pec, cec;
static mseq_t seq;

static pthread_t thread_id[N_CONSUMERS + N_PRODUCERS];

struct donut_ring {
	uint flavor[D_TYPES][D_SIZE];
	int	outptr[D_TYPES];
	mseq_t p_seq[D_TYPES];
	mevt_t p_evt[D_TYPES];

	// TODO Do I need this int	serial[D_TYPES];
} *ring;

void *producer(void *);
void *consumer(void *);

void across_cpu(void);
void on_core(void);

void init_seed(ushort **xsub) {
	struct timeval randtime; 

	gettimeofday (&randtime, (struct timezone *) 0);
	xsub[0] = (unsigned short) randtime.tv_usec;
	xsub[1] = (unsigned short) (randtime.tv_usec >> 16);
	xsub[2] = (unsigned short) (pthread_self());
}
	

int main(int argc, char *argv[]) {
	int i, j;
	struct timeval first_time, last_time;
	int arg_array[N_CONSUMERS];


	if ((ring == (struct donut_ring *) calloc(sizeof(struct donut_ring), 1)) != NULL) {
		printf("Failed to create space for ring buffer.\n");
		exit(1);
	}

	for (i = 0; i < D_TYPES; i++) {
		// Set Producer event counter to counter 50
		evtc_init(&ring->p_evt[i]);
		set_counter(&ring->p_evt[i], D_SIZE);
		seq_init(&ring->p_seq[i]);
		
	}



	printf("0->>>STUFF\n");


	evtc_init(&pec);
	seq_init(&seq);

	evtc_init(&cec);

	if ((tid[0] = pthread_create(&tid[0], NULL, prod, NULL)) != 0) {
		perror("FUCK");
	}
	if ((tid[1] = pthread_create(&tid[1], NULL, cons, NULL)) != 0) {
		perror("FUCK");
	}

	printf("0->>>STUFF DONE\n");

	pthread_join(tid[1], NULL);

	printf("0->>>STUFF FINISHED\n");
	return 0;
}

void *prod(void *arg) {
	uint donut;
	uint tick;
	ushort xsub[3];

	init_seed(&xsub);

	while (1) {
		donut = nrand48(xsub) % D_TYPES;
		tick = ticket(&ring->p_seq[donut]);

		mg_await(&ring->p_evt[donut], tick);
		// CS
		ring->flavor[donut][tick % D_TYPES] = tick;

		mg_signal(&ring->p_evt[donut]); 
		mg_signal(&ring->c_evt[donut]);
	}
	return NULL;
}

void *cons(void *arg) {
	int i;
	for (i = 0; i < 20; i++) {
		printf("GOT A DONUT\n");
		mg_await(&cec, ring.out+1);
		ring.out++;
		mg_signal(&pec);
	}

	return NULL;
}
