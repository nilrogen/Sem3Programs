#include <include.h>
#include <lamport.h>



int main(void) {
	lmp_mutex_t *mutex = lmp_mutex_create(0);
	lmsg_t value; 
	

	value.type = LREQUEST;
	value.clock = 0; 
	value.node = 0; 
	value.replies = 0; 
	value.pid = getpid();

	handle_request(mutex, value);

	value.clock++;

	handle_request(mutex, value);

	value.node = 1;
	handle_reply(mutex, value);

	value.clock = 1;
	handle_request(mutex, value);


	printf("%d\n", ((lmsg_t *) mutex->queue->head->data)->replies);

	

	
}


