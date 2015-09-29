#include "assignment1.h"

extern int setupSHM(bool create) {
	int fd;

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

extern donut_t *getSHM(int fd, bool create) {
	donut_t *retv; 

	if (create && ftruncate(fd, sizeof(donut_t)) == -1) {
		perror("Failed to truncate memory region");
		shm_unlink(MEMLOCATION);
		exit(1);
	}
	retv = (donut_t *) mmap(NULL, sizeof(donut_t), PROT_FLAGS, MAP_SHARED, fd, 0);

	return retv;
}

extern void destroySHM(donut_t *mem) {
	// unmap the data from memory
	if (munmap((void *)mem, sizeof(donut_t)) == -1) {
		perror("munmap failed");
		exit(1);
	}	
	// Unlink the name
	if (shm_unlink(MEMLOCATION) == -1) {
		perror("Failed to unlink location");
		exit(1);
	}
}

extern donut_t *getSHMSafe() {
	int fd;
	donut_t *retv;

	fd = setupSHM(FALSE);
	if ((retv = getSHM(fd, FALSE)) == MAP_FAILED) {
		perror("Failed to map region");
		close(fd);
		exit(1);
	}
	close(fd);
	return retv;
}

extern void safeUnmap(donut_t *data) {
	if (munmap((void *) data, sizeof(donut_t)) == -1) {
		perror("Failed to unmap region");
		exit(1);
	}
}




