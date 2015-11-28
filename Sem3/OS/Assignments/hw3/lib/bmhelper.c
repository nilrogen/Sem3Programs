#include <bm.h>

static int bmerrno; 

extern bmmsg_t bmm_hton(int m, int r, int o, int e) {
	bmmsg_t msg;
	msg.mtype   = htonl(m);
	msg.ovalue  = htonl(o);
	msg.ring    = htonl(r);
	msg.evalue  = htonl(e);
	return msg;
}

extern bmmsg_t bmm_ntoh(bmmsg_t msg) {
	msg.mtype  = ntohl(msg.mtype);
	msg.ovalue = ntohl(msg.ovalue);
	msg.ring   = ntohl(msg.ring);
	msg.evalue = ntohl(msg.evalue);
	return msg;
}

static int handle(int sockfd, bmmsg_t msg) {
	int v;

	// Write to socket
	while ((v = write(sockfd, &msg, sizeof(msg))) == -1 && errno == EINTR) ;
	if (v == -1 || v != sizeof(msg)) {
		perror("Write failed");
		bmerrno = BMIOERROR;
		return -1;
	}

	// Wait and read reply. Test for non reads
	while ((v = read(sockfd, &msg, sizeof(bmmsg_t))) == -1 
			&& errno == EINTR) ; // TODO: Possible error
	if (v == -1) {
		perror("Read failed");
		bmerrno = BMIOERROR;
		return -1;
	}

	// Convert message
	msg = bmm_ntoh(msg);

	if (msg.mtype != BMREPLY && msg.ovalue != 0) {
		bmerrno = msg.evalue;
		return -1;
	}
	return msg.ovalue;
}

extern int bmconsume(int sockfd, int type) {
	bmmsg_t msg = bmm_hton(BMCONSUME, type, 0, 0);
	return handle(sockfd, msg);
}

extern int bmproduce(int sockfd, int type) {
	bmmsg_t msg = bmm_hton(BMPRODUCE, type, 0, 0);
	return handle(sockfd, msg);
}


extern void bmperror(const char *str) {
	fprintf(stderr, "%s:   \n", str);
}
