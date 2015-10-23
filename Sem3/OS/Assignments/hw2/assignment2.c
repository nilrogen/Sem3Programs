#include "assignment2.h"
#include "evseq.h"
	
#define CPU_CK_COUNT 24
#define THREAD_SCOPE 1
#define CPU_SCOPE 0

static int output_state, scope_type;
static pthread_t thread_id[N_THREADS];

static struct donut_ring {
	int flavor[D_TYPES][D_SIZE];
	// Producer Mutex types
	mseq_t p_seq[D_TYPES];
	mseq_t c_seq[D_TYPES];
	mevt_t p_evt[D_TYPES];
	mevt_t c_evt[D_TYPES];
} *ring;

void *producer(void *);
void *consumer(void *);

void set_state(int, char ***);

void init_seed(ushort (*)[]);

void cpu_scope();
void thread_scope();

int main(int argc, char *argv[]) {
	int i, j;
	//struct timeval first_time, last_time;
	int con_args[N_CONSUMERS];
	//int pro_args[N_PRODUCERS];
	FILE *fout;
	struct timeval startt, endt; 

	gettimeofday(&startt, (struct timezone *) 0);

	set_state(argc, &argv);

	if (output_state & 1) {
		fout = fopen("timing", "a");
	}

	if (scope_type == CPU_SCOPE) {
		cpu_scope();
	}

	ring = (struct donut_ring *) calloc(sizeof(struct donut_ring), 1);
	if (ring == NULL) {
		printf("Failed to create space for ring buffer.\n");
		exit(1);
	}

	// Initialize all mutex types. 
	for (i = 0; i < D_TYPES; i++) {
		evtc_init(&ring->p_evt[i]);
		seq_init(&ring->p_seq[i]);

		evtc_init(&ring->c_evt[i]);
		seq_init(&ring->c_seq[i]);
	}


	printf("---- CREATING THREADS ----\n");

	for (i = 0; i < N_PRODUCERS; i++) {
		if(pthread_create(&thread_id[i], NULL, producer, NULL)) {
			printf("Failed to create thread!\n");
			return 1;
		}
	}

	for (i = N_PRODUCERS; i < N_THREADS; i++) {
		con_args[i - N_PRODUCERS] = i - N_PRODUCERS + 1;
		if(pthread_create(&thread_id[i], NULL, consumer, 
						 (void *) &con_args[i - N_PRODUCERS])) {

			printf("Failed to create thread!\n");
			return 1;
		}
	}
	
	printf("\n---- THREADS CREATED  ----\n");
	
	for (i = N_PRODUCERS; i < N_THREADS; i++) {
		pthread_join(thread_id[i], NULL);
		printf("*");
	}

	printf("\n---- PROGRAM FINISHED ----\n");


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
	printf("Execution Time: %d.%d sec\n", i, j);

	if (output_state & 1) {
		fprintf(fout, "%d.%d\n", i, j);
		fclose(fout);
	}
	return 0;
}

void *producer(void *arg) {
	uint donut;
	uint tick;
	ushort xsub[3];

	if (scope_type == THREAD_SCOPE) {
		thread_scope();
	}

	init_seed(&xsub);

	while (1) {
		donut = nrand48(xsub) % D_TYPES;
		tick = ticket(&ring->p_seq[donut]);

		mg_await(&ring->p_evt[donut], tick);
		mg_await(&ring->c_evt[donut], tick-D_SIZE+1);
		ring->flavor[donut][tick % D_SIZE] = tick;

		mg_advance(&ring->p_evt[donut]);
	}
	return NULL;
}

void *consumer(void *arg) {
	const int thread_num = *((int *)arg);
	int thread_state;
	int i, index;
	int *orders;
	uint donut, tick, rec; 
	char str[20];
	FILE *verbose_out, *project_out;
	struct tm *out_help;
	struct timeval finish_time;
	ushort xsub[3];

	if (scope_type == THREAD_SCOPE) {
		thread_scope();
	}
	init_seed(&xsub);

	if (output_state & 2) {
		sprintf(str, "fulloutput/c%d", thread_num);
		verbose_out = fopen(str, "w");
	}
	else {
		verbose_out = NULL;
	}
	memset(str, 0, 20);

	if (output_state & 3 && thread_num <= 5) {
		thread_state = 1;
		orders = (int *)calloc(sizeof(int), 120*2);
		sprintf(str, "cons%d", thread_num);
		project_out = fopen(str, "w");
	}
	else {
		project_out = NULL;
	}

	for (i = 0; i < N_CONS_TRIAL; i++) {
		donut = nrand48(xsub) % D_TYPES;
		tick = ticket(&ring->c_seq[donut]);

		index = tick % D_SIZE;

		mg_await(&ring->c_evt[donut], tick);
		mg_await(&ring->p_evt[donut], tick+1);
		// begin cs
		rec = ring->flavor[donut][index];
		// end cs
		mg_advance(&ring->c_evt[donut]); 

		if (output_state & 2) {
			fprintf(verbose_out, "%d %d\n", donut, rec);
		}
		if (thread_state && i < 120) {
			orders[i] = donut;
			orders[i*2] = rec;
		}
		if (i % 12 == 0) {
			usleep(1000);
		}
	}
 
	if (output_state & 2) {
		fclose(verbose_out);
	}
	if (thread_state) {
		memset(str, 0, 20);
		gettimeofday(&finish_time, (struct timezone *) 0);
		out_help = localtime(&finish_time.tv_sec);
		strftime(str, 20, "%H:%M:%S", out_help);
		fprintf(project_out, "Consumer %d", thread_num);
		fprintf(project_out, ", Time: %s", str);
		fprintf(project_out, "\n\nDonut order by dozen:");
		for (i = 0; i < 120; i++) {
			if (!(i % 12)) {
				fprintf(project_out, "\nDozen %d:", i / 12 + 1);
			}
			fprintf(project_out, " (%d, %d)", orders[i], orders[i*2]);
		}
	}

	printf(" %d", thread_num);
	return NULL;
}

void set_state(int argc, char ***argv) {
	int i, k;
	char val;

	if (strcmp((*argv)[0] + 2, "threadscope") == 0) {
		scope_type = THREAD_SCOPE;
	}
	else {
		scope_type = CPU_SCOPE;
	}

	for (i = 1; i  < argc; i++) {
		val = (*argv)[i][0];
		k = 0;
		if (val != '-') {
			continue;
		}

		val = (*argv)[i][k++];
		while (val != '\0') {
			switch (val) {
			case 't': // Timing output
				output_state |= 1;
				break;
			case 'f': // Full consumer output (for test.py usage)
				output_state |= 2;
				break;
			case 's': // Required Project output
				output_state |= 3;
				break;
			}
			val = (*argv)[i][k++];
		}
	}
}

void init_seed(ushort (*xsub)[]) {
	struct timeval randtime; 

	gettimeofday (&randtime, (struct timezone *) 0);
	(*xsub)[0] = (unsigned short) randtime.tv_usec;
	(*xsub)[1] = (unsigned short) (randtime.tv_usec >> 16);
	(*xsub)[2] = (unsigned short) (pthread_self());
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

void thread_scope() {
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
