#include <lamport.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

extern lmsg_t hmsgton(lmsg_t msg) {
	msg.clock = htonl(msg.clock);
	msg.node = htonl(msg.node);
	msg.type = htonl(msg.type);
	
	return msg;
}

extern lmsg_t nmsgtoh(lmsg_t msg) {
	msg.clock = ntohl(msg.clock);
	msg.node = ntohl(msg.node);
	msg.type = ntohl(msg.type);
	
	return msg;
}
