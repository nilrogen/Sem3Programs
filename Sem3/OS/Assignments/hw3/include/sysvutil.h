#ifndef __SYSVUTIL_H
#define __SYSVUTIL_H

#include <include.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define MQ_KEY  (key_t) 786929221
#define SEM_KEY (key_t) 788464649
#define SHM_KEY (key_t) 788881094

typedef struct {
	int id;
	void *region;
} mgshm_t;

extern mgshm_t *get_shm(key_t, size_t, int);
extern int remove_shm(mgshm_t *);




#endif


