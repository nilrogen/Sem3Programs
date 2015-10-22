#include "assignment2.h"
#include "evseq.h"
	
#define CPU_CK_COUNT 24

static int state; 
static pthread_t thread_id[N_THREADS];

static uint flavor[D_TYPES][D_SIZE];
static struct donut_ring {
	// Producer Mutex types
	mseq_t p_seq[D_TYPES];
	mseq_t c_seq[D_TYPES];
	mseq_t m_seq[D_TYPES];
	mevt_t p_evt[D_TYPES];
	mevt_t c_evt[D_TYPES];
	mevt_t m_evt[D_TYPES];
} *ring;

void *producer(void *);
void *consumer(void *);


void init_seed(ushort (*xsub)[3]) {
	struct timeval randtime; 

	gettimeofday (&randtime, (struct timezone *) 0);
	*xsub[0] = (unsigned short) randtime.tv_usec;
	*xsub[1] = (unsigned short) (randtime.tv_usec >> 16);
	*xsub[2] = (unsigned short) (gettid());
}

void cpu_scope() {
	int proc_cnt=0, i;
	cpu_set_t mask;	
	ushort xsub[3];

	init_seed(&xsub);

	sched_getaffinity(getpid(), sizeof(cpu_set_t), &mask);

	for (i = 0; i < CPU_CK_COUNT; i++) {
		proc_cnt += (CPU_ISSET(i, &mask)) ? 1 : 0;
	}

	CPU_ZERO(&mask);
	CPU_SET(nrand48(xsub) % proc_cnt, &mask);

	printf("AFFINITY SET TO: on %d Cores\n", proc_cnt);
	for (i = 0; i < proc_cnt; i++) {
		printf("%d ", (CPU_ISSET(i, &mask)) ? 1 : 0);
	}
	printf("\n");

	sched_setaffinity(0, sizeof(cpu_set_t), &mask);

}
void thread_scope(int val) {
	int proc_cnt = 0, i;
	cpu_set_t mask;	
	ushort xsub[3];

	init_seed(&xsub);

	sched_getaffinity(gettid(), sizeof(cpu_set_t), &mask);

	for (i = 0; i < CPU_CK_COUNT; i++) {
		proc_cnt += (CPU_ISSET(i, &mask)) ? 1 : 0;
	}

	CPU_ZERO(&mask);
	i = nrand48(xsub) % proc_cnt;
	CPU_SET(i, &mask);
	printf("%d ", i);

	sched_setaffinity(0, sizeof(cpu_set_t), &mask);
}

inline void lock(int donut) {
	mg_await(&ring->m_evt[donut], ticket(&ring->m_seq[donut]));
}

inline void unlock(int donut) {
	mg_signal(&ring->m_evt[donut]);
}
	

int main(int argc, char *argv[]) {
	int i, j, timing;
	//struct timeval first_time, last_time;
	int con_args[N_CONSUMERS];
	int pro_args[N_PRODUCERS];
	FILE *fout;
	struct timeval startt, endt; 

	gettimeofday(&startt, (struct timezone *) 0);


	fout = fopen("timing", "a");

	state = 0;
	// If the program run was threadscope change state.
	if (strcmp(argv[0]+2, "threadscope") == 0) {
		state = 1;

	}
	else {
		cpu_scope();
	}

	if (argc > 1 && strcmp(argv[1], "-v") == 0) { 
		timing = 1;
	}
	else {
		timing = 0;
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

		evtc_init(&ring->m_evt[i]);
		seq_init(&ring->m_seq[i]);
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


	if (timing) {
		gettimeofday(&endt, (struct timezone *) 0);
		if ((i = endt.tv_sec - startt.tv_sec) == 0) {
			j = endt.tv_usec - startt.tv_usec;
		}	
		else {
			if (endt.tv_usec - startt.tv_usec < 0) {
				i--;
				j = 1000000 + (endt.tv_usec - startt.tv_usec);
			}
			else {
				j = endt.tv_usec - startt.tv_usec;
			}
		}
		fprintf(fout, "%d.%d\n", i, j);
	}
	fclose(fout);
	return 0;
}

void *producer(void *arg) {
	int val = *((int *) arg);
	uint donut;
	uint tick;
	ushort xsub[3];

	if (state) {
		thread_scope(val);
	}

	init_seed(&xsub);

	//printf("Producer %d Running!\n", gettid());

	while (1) {
		donut = nrand48(xsub) % D_TYPES;
		tick = ticket(&ring->p_seq[donut]);
		mg_await(&ring->p_evt[donut], tick);
		// CS
		//lock(donut);
		flavor[donut][tick % D_SIZE] = tick;
		//unlock(donut);

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

	if (state) {
		thread_scope(val);
	}

	init_seed(&xsub);
	sprintf(n, "crap/c%d", val);
	out = fopen(n, "w");


	for (i = 0; i < N_CONS_TRIAL; i++) {
		donut = nrand48(xsub) % D_TYPES;

		tick = ticket(&ring->c_seq[donut]);
		mg_await(&ring->c_evt[donut], tick);
		// CS
		//lock(donut);
		rec = flavor[donut][(((int) tick) - 1) % D_SIZE];
		//unlock(donut);
		mg_signal(&ring->p_evt[donut]); 

		fprintf(out, "%d %d\n", donut, rec);
		
		usleep(1000);
	}
	fclose(out);
	printf(" %d", val);
	return NULL;
}
