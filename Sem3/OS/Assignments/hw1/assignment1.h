#ifndef __ASSIGNMENT1_H
#define __ASSIGNMENT1_H


#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <semaphore.h>

#include <signal.h>

#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define MEMLOCATION "/test_shm"
#define O_NORMAL O_CREAT | O_RDWR
#define O_CREATION O_NORMAL | O_EXCL

#define PROT_FLAGS PROT_WRITE | PROT_READ

#define FALSE 0
#define TRUE  1

/* DEFINE FOR PROGRAM */
#define D_TYPES 4
#define D_SIZE 40

#define N_DOZEN 10


typedef int bool;

typedef struct {
	int p_flavors[D_TYPES][D_SIZE];
	int p_out[D_TYPES];
	sem_t sems[D_TYPES][3];
} donut_t;

/* SHM functions */
extern int setupSHM(bool);
extern donut_t *getSHM(int, bool);
extern void destroySHM(donut_t *);
extern donut_t *getSHMSafe();
extern void safeUnmap(donut_t *);
extern donut_t *getSHMSafe();

/* SEM functions */
extern int setupSemaphores(donut_t *);
extern int destroySemaphores(donut_t *);

#define P(data, donut, type) sem_wait(&((data)->sems[(donut)][(type)]))
#define V(data, donut, type) sem_post(&((data)->sems[(donut)][(type)]))
#define Pin(data, donut) P(data, donut, 0)
#define Vin(data, donut) V(data, donut, 0)
#define Pout(data, donut) P(data, donut, 1)
#define Vout(data, donut) V(data, donut, 1)
#define Pmutex(data, donut) P(data, donut, 2)
#define Vmutex(data, donut) V(data, donut, 2)

#endif
