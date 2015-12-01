#include <include.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY (key_t) 1291293

typedef struct {
	long mtype;
	int value;
} m_t;

void mquit(int k) {
	if (msgctl(k, IPC_RMID, 0) == -1) {
		perror("Removal");
	} 
	exit(0);
}

void child() {
	m_t t;
	int mid;
	mid = msgget(KEY, IPC_CREAT | 0600);
	if (mid == -1) {
		perror("msgget");
		exit(1);
	}

	t.mtype = 1l;
	t.value = getpid();
	printf("CHILD PID %d\n", getpid());
	if (msgsnd(mid, &t, sizeof(int), 0) == -1) {
		perror("msgsnd");
		printf("CHILD");
		//mquit(mid);
	}
	
	exit(0);
}

int main(int argc, char *argv[]) {
	int pid;
	int mid;
	m_t msg;

	mid = msgget(KEY, IPC_CREAT | 0600);
	if (mid == -1) {
		perror("msgget");
		exit(1);
	}

	switch (pid = fork()) {
	default: printf("PARENT\n"); break;
	case -1: perror("Fork"); mquit(mid);
	case 0:
		child();
		exit(1);
	}
	

	if(msgrcv(mid, &msg, sizeof(int), 1l, 0) == -1) {
		perror("msgrcv");
		mquit(mid);
	}

	printf("mtype = %d\n", msg.mtype);
	printf("value = %d\n", msg.value);
	

	mquit(mid);
}

