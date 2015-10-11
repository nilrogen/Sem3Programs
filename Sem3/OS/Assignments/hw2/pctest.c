#include "evseq.h"

#define DONS 50

struct shared {
	int buff[DONS];
	int in;
	int out;
} ring;

mevt_t pec, cec;
mseq_t seq;

#define max(a, b) (a) > (b) ? (a) : (b)

	
void *prod(void *arg) {
	uint t;
	while (1) {
		t = ticket(&seq);
		mg_await(&pec, t);
		ring.buff[t % DONS] = 1;
		mg_signal(&cec);
		printf("MADE A DONUT\n");
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

int main(int argc, char *argv[]) {
	pthread_t tid[2];
	int i;

	ring.in = 0; 
	ring.out = 0;

	for (i = 0; i < DONS; i++) {
		ring.buff[i] = 0 ;
	}

	printf("STUFF\n");


	evtc_init(&pec);
	seq_init(&seq);

	evtc_init(&cec);

	if ((tid[0] = pthread_create(&tid[0], NULL, prod, NULL)) != 0) {
		perror("FUCK");
	}
	if ((tid[1] = pthread_create(&tid[1], NULL, cons, NULL)) != 0) {
		perror("FUCK");
	}

	printf("STUFF DONE\n");

	pthread_join(tid[1], NULL);

	printf("STUFF FINISHED\n");
	return 0;
}
