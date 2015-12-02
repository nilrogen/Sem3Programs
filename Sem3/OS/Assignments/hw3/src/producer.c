#include <include.h>
#include <msqutil.h>
#include <bm.h>
#include <netdb.h>

static int mid;

static int setupconnection();
static int setsignals();
static void sighandler(int);

int main(int argc, char *argv[]) {
	int sockfd, val, type;

	if (setsignals() == -1) {
		fprintf(stderr, "Failed to setup signal handler.\n");
		return -1;
	}
	if ((mid = open_msg(MQ_KEY)) == -1) {
		fprintf(stderr, "Failed to open message queue\n");
		sighandler(-1);
	}
	
	while (1) {
		sockfd = setupconnection();
		if (sockfd == -1) {
			remove_msg(mid);
			return -1;
		}


		type = 0;
		printf("PRODUCER - Requesting Mutex\n");
		if (request(mid, type) == -1) {
			return -1;
			sighandler(-1);	

		}

		val = bmproduce(sockfd, type);
		if (val == -1) {
			bmperror("Failed to produce");
		}
		printf("PRODUCER -- PRODUCED VALUE %d\n", val);

		close(sockfd);	
		
		if ( release(mid, type) == -1) {
			return -1;
		}
	}
	

//	remove_msg(mid);
	return 0;
}

void sighandler(int signal) {
	fprintf(stderr, "Producer signal handler with signal #%d.\n", signal);

	if (remove_msg(mid) == -1) { 
		fprintf(stderr, "Failed to remove message queue.\n"); 
	}

	exit(1);
}

int setsignals() {
	sigset_t mask; 
	struct sigaction new;

	int i, nsigs;
	int sigs[] = { SIGTERM, SIGHUP, SIGINT, SIGQUIT, 
				   SIGBUS, SIGSEGV, SIGFPE };
	nsigs = sizeof(sigs) / sizeof(int);

	/* Handle Regular Signals */
	sigemptyset(&mask);
	for (i = 0; i < nsigs; i++)
		sigaddset(&mask, sigs[i]);

	for (i = 0; i < nsigs; i++) {
		new.sa_handler = sighandler;
		new.sa_mask = mask;
		new.sa_flags = 0;

		if (sigaction(sigs[i], &new, NULL) == -1) {
			perror("Cannot set signals");
			return -1;
		}
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
