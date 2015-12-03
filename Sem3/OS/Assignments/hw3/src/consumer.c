#include <include.h>
#include <msqutil.h>
#include <bm.h>
#include <netdb.h>

static int setupconnection();

int main(int argc, char *argv[]) {
	int sockfd, val, type, mid;
	int i;

	if ((mid = open_msg(MQ_KEY)) == -1) {
		fprintf(stderr, "Failed to open message queue\n");
		exit(1);
	}
	srand(getpid());

	for (i = 0; i < 120; i++) {
	/* loop for some time */

		type = rand() % 4;
		if (request(mid, type + 4) == -1) {
			fprintf(stderr, "CONSUMER: Request Failed.\n");
			return -1;
		}
		sockfd = setupconnection();
		if (sockfd == -1) {
			release(mid, type + 4);
			return -1;
		}

		val = bmconsume(sockfd, type);
		if (val == -1) {
			bmperror("Failed to consume");
		}
		fprintf(stderr, "CONSUMER: type - %d, value - %d\n", type, val);

		if (release(mid, type + 4) == -1) {
			return -1;
		}

		close(sockfd);	
	}


	return 0;
}

int setupconnection() {
	int sockfd, cval, i;
	struct hostent *hinfo;
	struct sockaddr_in addr; 
	struct in_addr *val;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Failed to create socket.");
		return -1;
	}

	hinfo = gethostbyname(HOSTNAME);
	if (hinfo == NULL) {
		herror("gethostbyname failed");
		return -1;
	}

	addr.sin_port = htons(PORT_BUFFERMANAGER);
	addr.sin_family = AF_INET; 

	for (i = 0; i < sizeof(hinfo->h_addr_list) / sizeof(char *); i++) {
		val = (struct in_addr *) hinfo->h_addr_list[i];
		addr.sin_addr = *val;

		while ((cval = connect(sockfd, (struct sockaddr *) &addr,
				sizeof(addr))) == -1 && errno == EINTR) ; // Handle EINTR
		if (cval == -1) 
			perror("connect failed");
		else 
			break;
	}

	if (cval == -1) {
		perror("Connect failed");
		return -1;
	}

	
	return sockfd; 
}
