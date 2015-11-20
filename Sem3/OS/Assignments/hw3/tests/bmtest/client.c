#include "bmtest.h"

#include <time.h>

static int setup_socket() {
	int sockfd;
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, IP, &address.sin_addr);

	printf("OPEN PRODUCER\n");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Failed to open socket");
		return -1;
	}
	if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
		perror("Failed to connect!");
		return -1;
	}

	return sockfd;
}


#define PRINTSTUFF(msg) do { printf("--- Client --- "); bmp_print((msg)); } while (0)

static int client_main(const int type) {
	int sockfd;
	bmproto_t msg;

	size_t mlength = sizeof(bmproto_t);
	
	sockfd = setup_socket();
	if (sockfd == -1) {
		return -1;
	}

	// Write Setup Message
	msg = bmp_setup(type);

	//PRINTSTUFF(msg);
	write(sockfd, &msg, mlength);

	// Read Setup Reply Message
	read(sockfd, &msg, mlength);
	PRINTSTUFF(msg);

	if (bmp_status(msg) != BMP_SUCCESS) {
		printf("Setup with server failed\n");
		return -1;
	}

	srand(time(NULL) | getpid());
	
	while (bmp_status(msg) == BMP_SUCCESS) {
		// THIS IS THE MAIN LOOP CREATE STUFF
		// Write operation
		if (type == BMP_PRODUCE)
			msg = bmp_produce(rand() % 4);
		else
			msg = bmp_consume(rand() % 4);
		//PRINTSTUFF(msg);
		write(sockfd, &msg, mlength);

		// Read Reply
		read(sockfd, &msg, mlength);
		PRINTSTUFF(msg);
		printf("GOT VALUE (%d, %d)\n", bmp_selection(msg), bmp_value(msg));
	}


	// Clean up 
	msg = bmp_destroy(BMP_PRODUCE);
	write(sockfd, &msg, mlength);

	close(sockfd);

	return 0;
}

extern int producer_main() {
	return client_main(BMP_PRODUCE);
}

extern int consumer_main() {
	return client_main(BMP_PRODUCE);
}



