#include <include.h>
#include <msqutil.h>
#include <bm.h>
#include <netdb.h>

static int setupconnection();

int main(int argc, char *argv[]) {
	int sockfd, val, type, mid;

	if ((mid = open_msg(MQ_KEY)) == -1) {
		fprintf(stderr, "Failed to open message queue\n");
		exit(1);
	}

	/* loop for some time */
	while (1) {
		sockfd = setupconnection();
		if (sockfd == -1) {
			return 0;
		}

		type = 0;
		if (request(mid, type + 4) == -1) {
		// WHAT?
			printf("BADSHIT HAPPEND YO\n.");
			return -1;
		}

		val = bmconsume(sockfd, type);
		if (val == -1) {
			bmperror("Failed to consume");
		}
		fprintf(stderr, "CONSUMER -- CONSUMED VALUE %d\n", val);

		close(sockfd);	
		break; // TODO DELETE
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
		printf("PRODUCER -- BMIP - %s\n", inet_ntoa(addr.sin_addr));

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
