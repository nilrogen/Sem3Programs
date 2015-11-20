#include <sysvutil.h>

extern mgshm_t *get_shm(key_t key, size_t size, int flags) {
	mgshm_t *retv;
	int id; 

	if ((id = shmget(key, size+sizeof(int), flags)) == -1) {
		return NULL;
	}

	if ((retv = (mgshm_t *) shmat(id, NULL, 0)) == -1) {
		return NULL;
	}
	retv->id = id;
	return retv; 
}


extern int remove_shm(mgshm_t *region) {
	return shmctl(region->id, IPC_RMID, NULL);
}
