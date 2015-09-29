#include "header.h"

extern int setupSHM(bool create) {
	int fd;

	// TODO: Short circuit evaluation might not work? 
	if (create && ((fd = shm_open(MEMLOCATION, O_NORMAL, 0600)) == -1)) {
		if (errno == EEXIST) {
			perror("Failed to create shm, unlinking existing shm");
			shm_unlink(MEMLOCATION);
		}
		else {
			perror("Failed to open shm");
		}
		exit(1);
	}
	if ((fd = shm_open(MEMLOCATION, O_NORMAL, 0600)) == -1) {
		perror("Failed to open shared memory region");
		exit(1);
	}
	return fd;
}

extern val_t *getSHM(int fd, bool create) {
	val_t *retv; 

	if (create  && ftruncate(fd, sizeof(val_t)) == -1) {
		perror("Failed to truncate memory region");
		shm_unlink(MEMLOCATION);
		exit(1);
	}
	retv = (val_t *) mmap(NULL, sizeof(val_t), PROT_FLAGS, MAP_SHARED, fd, 0);

	return retv;
}

extern void destroySHM(val_t *mem, int fd) {
	// unmap the data from memory
	if (munmap((void *)mem, sizeof(val_t)) == -1) {
		perror("munmap failed");
		exit(1);
	}	
	// Unlink the name
	if (shm_unlink(MEMLOCATION) == -1) {
		perror("Failed to unlink location");
		exit(1);
	}
	if (close(fd) == -1) {
		perror("Failed to close shm");
		exit(1);
	}
}

extern val_t *getSHMSafe() {
	int fd;
	val_t *retv;

	fd = setupSHM(FALSE);
	if ((retv = getSHM(fd, FALSE)) == MAP_FAILED) {
		perror("Failed to map region");
		close(fd);
		exit(1);
	}
	close(fd);
	return retv;
}

extern void safeUnmap(val_t *data) {
	if (munmap((void *) data, sizeof(val_t)) == -1) {
		perror("Failed to unmap region");
		exit(1);
	}
}




