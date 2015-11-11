#include "bmtest.h"

#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

static ringbuffer_t *ring;
static pthread_mutex_t lock[T];
static pthread_cond_t cond[T];

void *prod_client(void *arg) {
	int sockfd = *(int *) arg;
	bmproto_t header;

	read(sockfd, &header, sizeof(header));
	bmp_print(header);

	printf("TID = %d\n", gettid());
	header = bmp_reply(BMP_SUCCESS, gettid(), BMP_PRODUCE, 0, 0);
	write(sockfd, &header, sizeof(header));

	while (1) {
		read(sockfd, &header, sizeof(header));


	}
	return NULL;
}
void *cons_client(void *arg) {
	//int sockfd = *(int *) arg;

	return NULL;
}

int main(int argc, char *argv[]) {
	int sockfd, csockfd;
	struct sockaddr_in address;
	struct sockaddr padd;
	socklen_t csize;

	pthread_t thread;

	// Set address info
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, IP, &address.sin_addr);

	// Run Correct Main
	if (strcmp(argv[0]+2, "prod") == 0) {
		producer_main();
	}
	else {
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
		if (listen(sockfd, 5) == -1) {
			perror("Failed to listen");
			close(sockfd);
			return -1;
		}

		// Listen to incomming producer/consumer client creation
		// Create new thread to handle the session
		while (1) {
			csockfd = accept(sockfd, &padd, &csize);
			if (csockfd == -1) {
				perror("Accept Failed");
				return -1;
			}
			printf("CREATING HANDLER\n");
			pthread_create(&thread, NULL, prod_client, &csockfd);
		}
	}

	return 0;
}
