#include "nm.h"

// Role = LOCALHOST
static const char *HOSTS[] = { "", "cs91515-2", "cs91515-3", "cs91515-4"};
#define N_HOSTS (sizeof(HOSTS) / sizeof(char *))
#define N_NODES (N_HOSTS - 1)


//static pthread_mutex_t utillock;
static lmp_mutex_t *lmutex[N_MUTEX];

static int msgid;
static int nodenumber;

static int terminate;

static void sendtoall(int *, nmsg_t);
static pthread_t setup_conn(int);

static void sighandler(int);
static int setsignals();

void *rcv_thread(void *data) {
	int sockfd, tmp;
	nmsg_t msg;

 	sockfd = *(int *) data;

	setsignals();


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
	sighandler(-1);
	return NULL;
}

void *snd_thread(void *data) {
	int i;
	lmsg_t *hmsg;
	nmsg_t msg;
	nmrequest_t req;
	int *connections = (int *) data;

	setsignals();
	
	assert(connections != NULL);
	printf("NM: %d\n", sizeof(connections) / sizeof(int));
	for (i = 0; i < sizeof(connections) / sizeof(int); i++) {
		printf("NM: %d\n", connections[i]);
	}


	while (terminate == 0) {
		// Check if any queues can go (replied from all nodes)
		for (i = 0 ; i < N_MUTEX; i++) {
			// If empty
			if (lmutex[i]->queue->length == 0)
				continue;

			// Check head
			hmsg = DATA(peek(lmutex[i]->queue));
			if (hmsg->replies == N_NODES) {
				// Tell Process it can go
				printf("NM: Process %d reply\n", hmsg->pid);
				if (msq_reply(msgid, i, hmsg->pid, 0) == -1) {
					fprintf(stderr, "Failure to reply.\n");
				}

			}
		}

		// Get the next message from the message queue
		req = msq_next(msgid);
		if (req.mtype == -2L) { // If no messages sleep
			usleep(100);
			continue;
		}
		if (req.mtype == -1L) { // Failure
			fprintf(stderr, "msq failue.\n");
			continue;
		}

		// Which mutex is this refering too
		msg.mutex = req.mutex; 

		// If a local process releases its mutex then we locally handle the
		// release and send a release message to all the 
		if (req.rtype == LRELEASE) {
			msg.lm = *handle_release(lmutex[msg.mutex]);
		}
		else {

			// Handle next request
			msg.lm.type = LREQUEST;
			msg.lm.clock = -1; // Set
			msg.lm.node = nodenumber;
			msg.lm.pid  = req.pid;
			msg.lm.replies = 1;

			// Add to lamport mutex
			msg.lm.clock = handle_request(lmutex[msg.mutex], msg.lm);
			if (msg.lm.clock == -1) {
				fprintf(stderr, "Failed to handle request.\n");
				continue;
			}
		}

		// Send to connected entities
		printf("Seending to all");
		sendtoall(connections, msg);
	}
	sighandler(-1);
	return NULL;
}



int main(int argc, char *argv[]) {
	int i, role;
	pthread_t tid;
	
	setsignals();

	if (argc == 1) {
		role = 1; 
	}
	else {
		role = atoi(argv[1]);
	}

	nodenumber = role - 1;
	printf("NM (%d) Started.\n", nodenumber);

	// Create MSQ
	if ((msgid = open_msg(MQ_KEY)) == -1) {
		perror("open_msg");
		return -1;
	}

	for (i = 0; i < N_MUTEX; i++) {
		lmutex[i] = lmp_mutex_create(nodenumber);
		if (lmutex[i] == NULL) {
			fprintf(stderr, "Failed to create mutex");
			sighandler(-1);
		}
	}

	// Connects and creates new threads
	if ((tid = setup_conn(role)) == -1) {
		printf("Failed to setup connections.\n");
		sighandler(-1);
	}

	printf("NM: Main Wait\n");

	int sigs[] = { SIGTERM, SIGHUP, SIGINT, SIGQUIT, 
				   SIGBUS, SIGSEGV, SIGFPE };
	sigset_t mask;

	/* Handle Regular Signals */
	sigemptyset(&mask);
	for (i = 0; i < sizeof(sigs)/sizeof(int); i++)
		sigaddset(&mask, sigs[i]);
	sigwait(&mask, &i);

	sighandler(i);
	return 0;
}

pthread_t setup_conn(int role) {
	int i, j, iov, sockin, sockout;
	int *connection;
	pthread_t tid;
	struct hostent *hinfo;
	struct sockaddr_in addr;
	struct sockaddr other;
	socklen_t osize;
	struct in_addr *val;

	// snd_thread passed in a list of connected socket fd values
	connection = (int *) malloc(sizeof(int) *( N_NODES - 1));

	printf("Trying to Connect totally\n");

	// Accept Connections for future started nm

	// Create Socket
	sockin = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockin == -1) {
		perror("socket");
		return -1;
	}
	iov = 1;
	if (setsockopt(sockin, SOL_SOCKET, SO_REUSEADDR,
					&iov, sizeof(int)) == -1) {
		perror("setsockopt");
	}
	// Set values
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NODEMANAGER);
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	if (bind(sockin, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
		perror("bind");
		return -1;
	}
	if (listen(sockin, 5) == -1) {
		perror("listen");
		return -1;
	}

	for (i = 1; i < role; i++) {
		while ((sockout = accept(sockin, &other, &osize)) == -1 
				&& errno == EINTR) {
			; // Nothing
		}
		if (sockout == -1) {
			perror("accept");
			return -1;
		}
		printf("NM: Accepted connection\n");

		connection[i-1] = sockout;
		printf("%d\n", sockout);
		if (pthread_create(&tid, NULL, rcv_thread, &connection[i-1]) != 0) {
			return -1;
		}
	}
	for (i = role + 1; i < N_HOSTS; i++) {

		sockout = socket(AF_INET, SOCK_STREAM, 0); 
		if (sockout == -1) {
			perror("socket");
			return -1;
		}
		iov = 1;
		if (setsockopt(sockin, SOL_SOCKET, SO_REUSEADDR, 
						&iov, sizeof(int)) == -1) {
			perror("setsockopt");
		}

		hinfo = gethostbyname(HOSTS[i]); 
		if (hinfo == NULL) {
			herror("gethostbyname failed");
			return -1;
		}

		memset(&addr, 0, sizeof(addr));
		addr.sin_port = htons(PORT_NODEMANAGER);
		addr.sin_family = AF_INET; 

		for ( j = 0 ; j < sizeof(hinfo->h_addr_list) / sizeof(char *); j++) {
			val = (struct in_addr *) hinfo->h_addr_list[j];
			addr.sin_addr = *val;

			while ((iov = connect(sockout, (struct sockaddr *) &addr, 
					sizeof(addr))) == -1 && errno == EINTR) ;
			if (iov == -1) {
				fprintf(stderr, "Failed on NODE %d - ip %s - ", 
						j, inet_ntoa(*val));
				perror("connect failed");
			}
			else {
				connection[i-2] = sockout;
				printf("%d", sockout);
				pthread_create(&tid, NULL, rcv_thread, &connection[i-2]);
				break;
			}
		}
		printf("NM: Connected to host: %s\n", HOSTS[i]);

	}
	if (pthread_create(&tid, NULL, snd_thread, connection) != 0) {
		fprintf(stderr, "Failed to create snd_thread.\n");
		return -1;
	}

	printf("NM: Connections setup.\n");
	return tid;
}

static void sendtoall(int *conns, nmsg_t msg) {
	int i, iov;

	msg.mutex = htonl(msg.mutex);
	msg.lm = hmsgton(msg.lm);


	for (i = 0 ; i < N_NODES - 1; i++) {
		while ((iov = write(conns[i], &msg, sizeof(msg))) == -1 
				&& errno == EINTR)
			; // nothing

		if (iov == -1) {
			perror("write failed");
		}
	}
}

void sighandler(int signal) {
	int i; 

	terminate = 1;

	fprintf(stderr, "NodeManager signal handler with signal #%d.\n", 
			signal);

	if (msgctl(msgid, IPC_RMID, 0) == -1) {
		perror("Failed to remove message queue");
	}

	for (i = 0; i < N_MUTEX; i++) {
		lmp_mutex_destroy(lmutex[i]);
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

