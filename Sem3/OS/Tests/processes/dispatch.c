#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


#define BLOCK_SIZE 64

typedef char byte;
typedef struct {
	byte type;
	short spid;
	short num_blocks;
} mheader_t;


byte *createMessage(mheader_t info, char *message[]) {
	byte *retm; 
	short blocks = info.num_blocks;

	retm = calloc(sizeof(mheader_t) + BLOCK_SIZE * blocks, sizeof(char));	

	memmove(retm, &info, sizeof(info));

	for (int i = 0; i < blocks; i++) {
		memmove(retm + sizeof(mheader_t) + BLOCK_SIZE * i, message[i], BLOCK_SIZE);
	}

	return retm;
}

void childProcess(int wchn) {
	mheader_t h; 
	char *message = (char *)malloc(sizeof(char) * BLOCK_SIZE);
	byte *tosend;

	h.type = 0; // PRINT MY MESSAGE
	h.spid = getpid();
	h.num_blocks = 1;

	sleep(h.spid % 4);

	snprintf(message, BLOCK_SIZE, "Hello MY PID is %d!\n", h.spid);
	tosend = createMessage(h, &message);

	write(wchn, tosend, sizeof(h) + BLOCK_SIZE);
	free(tosend);

	h.type = 1 << 4;
	snprintf(message, BLOCK_SIZE, "PID %d is going to die now!\n", h.spid);

	tosend = createMessage(h, &message);

	sleep(1);
	write(wchn, tosend, sizeof(h) + BLOCK_SIZE);
	free(tosend);

	free(message);
	exit(0);
}

void parentHandler(int rchn, int children) {
	char *message;
	mheader_t h;

	while (children > 0) {
		read(rchn, &h, sizeof(h));

		message = calloc(BLOCK_SIZE * h.num_blocks, sizeof(char));
		read(rchn, message, BLOCK_SIZE * h.num_blocks);

		printf("Child %d's Message: %s", h.spid, message);

		if (h.type >> 4 == 1) {
			children--;
		}
		free(message);
	}

}


int main(int argc, char *argv[]) {
	int com1[2];
	int cpid;
	int children = 4;

	if (pipe(com1) == -1) {
		perror("Failed to create pipe");
		exit(0);
	}

	for (int i = 0; i < children; ++i) {
		switch (cpid = fork()) {
		case -1:
			perror("Fork Failed");
			exit(2);
		default: // Parent
			printf("Parent Process --- Child PID: %d\n", cpid);
			break;
		case 0: // Child
			childProcess(com1[1]);
			exit(0);
		}
	}

	parentHandler(com1[0], children);

	return 0;
}
