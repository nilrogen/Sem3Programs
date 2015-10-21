#include "assignment2.h"
#include "evseq.h"

static pthread_t thread_id[N_THREADS];

static struct donut_ring {
	uint flavor[D_TYPES][D_SIZE];
	int	outptr[D_TYPES];
	// Producer Mutex types
	mseq_t p_seq[D_TYPES];
	mseq_t c_seq[D_TYPES];
	mevt_t p_evt[D_TYPES];
	mevt_t c_evt[D_TYPES];
} *ring;

void *producer(void *);
void *consumer(void *);

void across_cpu(void);
void on_core(void);

void init_seed(ushort (*xsub)[3]) {
	struct timeval randtime; 

	gettimeofday (&randtime, (struct timezone *) 0);
	*xsub[0] = (unsigned short) randtime.tv_usec;
	*xsub[1] = (unsigned short) (randtime.tv_usec >> 16);
	*xsub[2] = (unsigned short) (pthread_self());
}

/*
inline void lock(uint donut) {
	mg_await(&ring->mut_evt[donut], ticket(&ring->mut_seq[donut]));
}

inline void unlock(uint donut) {
	mg_signal(&ring->mut_evt[donut]);
}
*/
	

int main(int argc, char *argv[]) {
	int i;
	//struct timeval first_time, last_time;
	int con_args[N_CONSUMERS];
	int pro_args[N_PRODUCERS];

	// The program defaults to processor scope
	int state;
	pthread_attr_t tattr; 
	struct sched_param sscheme;

	state = 0;
	// If the program run was threadscope change state.
	if (strcmp(argv[0]+2, "threadscope") == 0) {
		state = 1;
	}
		


	if ((ring = (struct donut_ring *) calloc(sizeof(struct donut_ring), 1)) == NULL) {
		printf("Failed to create space for ring buffer.\n");
		exit(1);
	}

	// Initialize all mutex types. 
	for (i = 0; i < D_TYPES; i++) {
		evtc_init(&ring->p_evt[i]);
		// Set Producer event counter to counter D_SIZE - 1
		set_counter(&ring->p_evt[i], D_SIZE-1);
		seq_init(&ring->p_seq[i]);

		evtc_init(&ring->c_evt[i]);
		seq_init(&ring->c_seq[i]);
		/* Consumer threads need to block if no donuts are created
		   so first ticket is consumed. */
		ticket(&ring->c_seq[i]);
	}



	printf("---- CREATING THREADS ----\n");

	for (i = 0; i < N_PRODUCERS; i++) {
		pro_args[i] = i + 1;
		if(pthread_create(&thread_id[i], NULL, producer, (void *) &pro_args[i]) != 0) {
			printf("Failed to create thread!\n");
			return 1;
		}
	}

	for (i = N_PRODUCERS; i < N_THREADS; i++) {
		con_args[i - N_PRODUCERS] = i - N_PRODUCERS + 1;
		if(pthread_create(&thread_id[i], NULL, consumer, 
						 (void *) &con_args[i - N_PRODUCERS]) != 0) {
			printf("Failed to create thread!\n");
			return 1;
		}
	}
	
	printf("---- THREADS CREATED  ----\n");
	
	for (i = N_PRODUCERS; i < N_THREADS; i++) {
		pthread_join(thread_id[i], NULL);
		printf("*");
	}



	printf("---- PROGRAM FINISHED ----\n");
	return 0;
}

void *producer(void *arg) {
	int val = *((int *) arg);
	uint donut;
	uint tick;
	ushort xsub[3];

	init_seed(&xsub);

	//printf("Producer %d Running!\n", gettid());

	while (1) {
		donut = nrand48(xsub) % D_TYPES;
		tick = ticket(&ring->p_seq[donut]);
		mg_await(&ring->p_evt[donut], tick);
		// CS

		ring->flavor[donut][tick % D_SIZE] = tick;

		mg_signal(&ring->c_evt[donut]);
	}
	return NULL;
}

void *consumer(void *arg) {
	int i;
	int val = *((int *)arg);
	char n[20];
	uint donut, tick, rec; 

	FILE *out;

	ushort xsub[3];

	init_seed(&xsub);
	sprintf(n, "crap/c%d", val);
	out = fopen(n, "w");

	for (i = 0; i < N_CONS_TRIAL; i++) {
		donut = nrand48(xsub) % D_TYPES;

		tick = ticket(&ring->c_seq[donut]);
		mg_await(&ring->c_evt[donut], tick);

		// CS

		rec = ring->flavor[donut][(((int) tick) - 1) % D_SIZE];

		fprintf(out,"%d %d %d\n", donut, rec, (tick-1) % D_SIZE);
		

		mg_signal(&ring->p_evt[donut]); 
		usleep(1000);
	}
	printf(" %d", val);

	return NULL;
}
