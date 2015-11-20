#include "bmtest.h"

static ringbuffer_t *ring;
static pthread_mutex_t lockp[T];
static pthread_cond_t condp[T];
static pthread_mutex_t lockc[T];
static pthread_cond_t condc[T];

int produce(int);
int consume(int);

static int enforce_state(int sockfd, bmproto_t msg, int state) {
	if (msg.type != state || (state == 2 && msg.type != BMP_DESTROY)) {
		msg = bmp_reply(BMP_FAILURE, -1, -1);
		write(sockfd, &msg, sizeof(bmproto_t));
		shutdown(sockfd, SHUT_RDWR);
		return -1;
	}
	return 0;
}

#define PRINTSTUFF(msg) do { printf("--- SERVER --- "); bmp_print((msg)); } while (0)

void *client_handler(void *arg) {
	int type, value, selection;
	bmproto_t msg; 
	int sockfd = *(int *)arg;	
	size_t mlength = sizeof(msg);

	// Read Setup Message
	read(sockfd, &msg, mlength);
	PRINTSTUFF(msg);

	enforce_state(sockfd, msg, BMP_SETUP);
	
	// Get Type Info
	type = bmp_optype(msg);

	// Write Reply
	msg = bmp_reply(BMP_SUCCESS, -1, -1);
	write(sockfd, &msg, mlength);
	//PRINTSTUFF(msg);

	// Operation Handler
	while (1) {
		// Read Operation
		read(sockfd, &msg, mlength); 
		PRINTSTUFF(msg);

		// Checks 
		if (msg.type == BMP_DESTROY) {
			printf("THIS IS WEIRD\n");
			break;
		}
		else if (enforce_state(sockfd, msg, BMP_OPERATION)) {
			printf("FUCK THIS SHIT");
			return NULL;
		}

		// Handle Operation
		printf("Handling operation!!\n");
		selection = bmp_selection(msg);
		if (type == BMP_PRODUCE) {
			value = produce(selection);
		} else {
			value = consume(selection);
		}
		

		printf("(%d, %d)\n", selection, value);
		// Write Reply
		msg = bmp_reply(BMP_FAILURE, selection, value);
		PRINTSTUFF(msg);
		write(sockfd, &msg, mlength);
	}

	printf(" --- SERVER --- \nSHUTDOWN\n");
	shutdown(sockfd, SHUT_RDWR);

	return NULL;
}

int main(int argc, char *argv[]) {
	int sockfd, csockfd;
	struct sockaddr_in address;
	struct sockaddr padd;
	socklen_t csize;

	pthread_t thread;

	// Run Correct Main
	if (strcmp(argv[0]+2, "prod") == 0) {
		return producer_main();
	}
	else if (strcmp(argv[0]+2, "cons") == 0) {
		return producer_main();
	}

	// Set address info
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, IP, &address.sin_addr);

	// Setup Address info/socket
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, IP, &address.sin_addr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Stuff");
		return -1;
	}

	if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
		perror("Failed to bind Socket"); 
		return -1;
	}
	// Listen with Backlog of 5 requests 
	if (listen(sockfd, 5) == -1) {
		perror("Failed to listen");
		close(sockfd);
		return -1;
	}

	ring = (ringbuffer_t *) calloc(sizeof(ringbuffer_t), 0);

	// Listen to incomming producer/consumer client creation
	// Create new thread to handle the session
	while (1) {
		csockfd = accept(sockfd, &padd, &csize);
		if (csockfd == -1) {
			perror("Accept Failed");
			return -1;
		}
		printf("CREATING HANDLER\n");
		pthread_create(&thread, NULL, client_handler, &csockfd);
	}


	return 0;
}


int produce(int selection) {
	int retv; 

	pthread_mutex_lock(&lockp[selection]);
	while (ring->spaces[selection] == 0) {
		pthread_cond_wait(&condp[selection], &lockp[selection]);
	}
	retv = ring->serial[selection];
	ring->buffer[selection][ring->p_in[selection]] = retv;
	ring->serial[selection]++;
	ring->p_in[selection] = (ring->p_in[selection] + 1) % N;
	ring->spaces[selection]--;

	pthread_mutex_unlock(&lockp[selection]);

	pthread_mutex_lock(&lockc[selection]);
	ring->product[selection]++;
	pthread_mutex_unlock(&lockc[selection]);

	pthread_cond_signal(&condc[selection]);	
	
	return retv;
}


int consume(int selection) {
	int retv;
	pthread_mutex_lock(&lockc[selection]);
	while (ring->product[selection] == 0) {
		pthread_cond_wait(&condc[selection], &lockc[selection]);
	}
	retv = ring->buffer[selection][ring->p_out[selection]];
	ring->product[selection]--;
	ring->p_out[selection] = (ring->p_out[selection] + 1) % N;
	pthread_mutex_unlock(&lockc[selection]);

	pthread_mutex_lock(&lockp[selection]);
	ring->spaces[selection]++;
	pthread_mutex_lock(&lockp[selection]);

	pthread_cond_signal(&condp[selection]);

	return retv;
}
