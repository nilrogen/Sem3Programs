#include "bmtest.h"

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

extern int producer_main() {
	int sockfd;
	int tid;

	bmproto_t msg;
	size_t mlength = sizeof(bmproto_t);
	
	sockfd = setup_socket();
	if (sockfd == -1) {
		return -1;
	}

	msg = bmp_setup(BMP_PRODUCE);
	printf("CLIENT:--------------\n");
	bmp_print(msg);

	printf("Writing!\n");
	write(sockfd, &msg, mlength);

	read(sockfd, &msg, mlength);
	printf("CLIENT:--------------\n");
	bmp_print(msg);

	if (bmp_status(msg) != BMP_SUCCESS) {
		printf("Setup with server failed\n");
		return -1;
	}
	tid = bmp_opnum(msg);

	while (bmp_status(msg) == BMP_SUCCESS) {
		// THIS IS THE MAIN LOOP CREATE STUFF
		msg = bmp_produce(tid, 0);
		printf("CLIENT:--------------\n");
		bmp_print(msg);
		write(sockfd, &msg, mlength);
		write(sockfd, &msg, mlength);


	}


	msg = bmp_destroy(BMP_PRODUCE);
	write(sockfd, &msg, mlength);

	close(sockfd);

	return 0;
}


