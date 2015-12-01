#include "nm.h"

// Role = LOCALHOST
static const char *HOSTS[] = { "", "localhost", "192.168.1.13" };
#define NUM_HOSTS (sizeof(HOSTS) / sizeof(char *))

//static pthread_mutex_t utillock;
static lmp_mutex_t **lmutex;

static int terminate;

void rcv_thread(void *data) {
	int sockfd, tmp;
	nmsg_t msg;

 	sockfd = *(int *) data;


	// Forever until we request termination?
	while (terminate == 0) {
		// Read
		while ((tmp = read(sockfd, &msg, sizeof(nmsg_t))) == -1 
				&& errno == EINTR) ; // Nothing
		if (tmp == -1) {
			fprintf(stderr, "Thread-%lu: ", pthread_self());
			perror("Failed to read");
			break;
		}

		// Convert
		msg.mutex = ntohl(msg.mutex);
		msg.lm = nmsgtoh(msg.lm);

		switch (msg.lm.type) {
		case LREQUEST:
			handle_request(lmutex[msg.mutex], msg.lm);
			break;
		case LREPLY: 
			handle_reply(lmutex[msg.mutex], msg.lm);
			break;
		case LRELEASE:
			handle_release(lmutex[msg.mutex]);
			break;
		}
	}

	
}

/*
int setup_conn(int role, int nnodes) {
	int i, sockin, sockout;
	struct hostent *hinfo;

	for (i = 0 ; i < role; i++) {
					

	}



}
*/

int main(int argc, char *argv[]) {
	int i, count, nnodes, role;
	struct hostent *ret; 
	struct in_addr **v;

	if (argc < 2) {
		role = 0; 
		nnodes = 1;
	}
	else {
		role = atoi(argv[1]);
		count = atoi(argv[2]);
	}


	ret = gethostbyname(HOSTS[1]);
	if (ret == NULL) {
		herror("Error");
		exit(0); 
	}
	
	v = (struct in_addr **) ret->h_addr_list;

	for (i = 0; i < sizeof(v)/sizeof(void *); i++) {
		printf("%s\n", inet_ntoa(*v[i]));
	}

	for (count = 0; count < nnodes - role; count++) {
		
		


	}
	
	
	return 0; 
}
