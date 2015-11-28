#include <include.h>
#include <bm.h>
#include <netdb.h>

int request(int v) { return 0; }

int setupconnection() {
	int sockfd, cval;
	struct hostent *hinfo;
	struct sockaddr_in addr; 
	bmmsg_t stuff;

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

	addr.sin_addr = **(struct in_addr **) hinfo->h_addr_list[0];
	while ((cval = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr))) == -1
	 		&& errno == EINTR) ; // Handle EINTR
	if (cval == -1) {
		perror("Connect failed");
		return -1;
	}

	
	return sockfd; 
}

int main(int argc, char *argv[]) {
	int sockfd, val;

	while (1) {
		sockfd = setupconnection();

		request(0);

		val = bmproduce(sockfd, 0);
		if (val == -1) {
			bmperror("Failed to produce");
		}

		close(sockfd);	
		break; // TODO DELETE
	}

	return 0;
}




