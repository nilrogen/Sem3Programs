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
	nmreply_t rep;
	
	size_t len = sizeof(nmrequest_t) - sizeof(long);

	pid = getpid();
	type = (long) pid;
	/*
	type <<= 16;
	type |= (long) mutex;*/

	req.rtype = REQUESTMSG;
	req.pid = getpid();
	req.mutex = mutex;
	req.mtype = 1L;
	
	while ((tmp = msgsnd(id, &req, len, 0)) == -1 && errno == EINTR) ;
	if (tmp == -1) {
		perror("msgsnd");
		return -1;
	}

	len = sizeof(nmreply_t) - sizeof(long);

	while ((tmp = msgrcv(id, &rep, len, type, 0)) == -1 && errno == EINTR) ; 
	if (tmp == -1) {
		perror("msgrcv request");
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
	req.mtype = 1L;
	
	//fprintf(stderr, "Releaseing mutex\n");
	while ((tmp = msgsnd(id, &req, len, 0)) == -1 && errno == EINTR) ; 
	if (tmp == -1) {
		perror("msgsnd");
		return -1;
	}

	return 0;
}

extern nmrequest_t msq_next(int id) {
	int tmp; 
	nmrequest_t req;

	size_t len = sizeof(nmrequest_t) - sizeof(long);

	while ((tmp = msgrcv(id, &req, len, 1L, 0)) == -1 && errno == EINTR) ; 
	if (errno == ENOMSG) {
		req.mtype = -2;
		return req;
	}
	if (tmp == -1) {
		perror("msgrcv msq_next"); 
		fprintf(stderr, "%d\n", errno);
		exit(0);
		req.mtype = -1;
		return req;
	}

	return req;
}



extern int msq_reply(int id, int mutex, int pid, int eval) {
	int tmp;
	long type;
	nmreply_t rep;
	
	size_t len = sizeof(nmreply_t) - sizeof(long);

	type = (long) pid;
	//type <<= 16;
	//type |= (long) mutex; 

	rep.mtype = type;
	rep.errorval = eval;

	fprintf(stderr, "SNEDING MESSAGE ON %ld\n", type);
	while ((tmp = msgsnd(id, &rep, len, 0)) == -1 && errno == EINTR) ;
	if (tmp == -1) {
		perror("msgsnd");
		return -1;
	}
	return 0;
}




