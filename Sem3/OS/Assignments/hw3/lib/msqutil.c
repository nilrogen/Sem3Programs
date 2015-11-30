#include <msqutil.h>


extern int open_msg(key_t key) {
	int mid;
	if ((mid = msgget(key, IPC_CREAT | 0600)) == -1) {
		perror("msgget");
		return -1;
	}
	return mid;
}

extern int remove_msg(int id) {
	if (msgctl(id, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		return -1;
	}
	return 0;
}


/**
 * This function tells the node manager that the sending process is 
 * interested in a resource. The function returns when the node manager
 * has the resource
 */
extern int request(int id, int mutex) {
	int tmp, pid;
	long type;
	nmrequest_t req;
	
	size_t len = sizeof(nmrequest_t) - sizeof(long);

	pid = getpid();
	type = (long) pid;
	type <<= 16;
	type |= mutex;

	req.rtype = RELEASEMSG;
	req.pid = getpid();
	req.mutex = mutex;
	req.mtype = NMMTYPE;
	
	while ((tmp = msgsnd(id, &req, len, 0)) == -1 && errno == EINTR) ;
	if (tmp == -1) {
		perror("msgsnd");
		return -1;
	}

	while ((tmp = msgrcv(id, &req, len, type, 0)) == -1 && errno == EINTR) ; 
	if (tmp == -1) {
		perror("msgrcv");
		return -1;
	}
	return 0;
}

/**
 * This function tells the node manager that the sending process is
 * done with the resource. The node manage does not reply
 */
extern int release(int id, int mutex) {
	int tmp;
	nmrequest_t req;

	size_t len = sizeof(nmrequest_t) - sizeof(long);

	req.rtype = RELEASEMSG;
	req.pid = getpid();
	req.mutex = mutex;
	req.mtype = NMMTYPE;
	
	while ((tmp = msgsnd(id, &req, len, 0)) == -1 && errno == EINTR) ; 
	if (tmp == -1) {
		perror("msgsnd");
		return -1;
	}

	return 0;
}

