#include <lamport.h>

static int clockvalue; 


extern lmsg_t hmsgton(lmsg_t msg) {
	msg.clockval = htonl(msg.clockval);
	msg.node = htonl(msg.node);
	msg.type = htonl(msg.type);
	
	return msg;
}

extern lmsg_t nmsgtoh(lmsg_t) {
	msg.clockval = ntohl(msg.clockval);
	msg.node = ntohl(msg.node);
	msg.type = ntohl(msg.type);
	
	return msg;
}

extern lmsg_t prepare_message(int type, int node) {
	
}
