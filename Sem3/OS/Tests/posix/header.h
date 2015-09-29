#ifndef __HEADER_H
#define __HEADER_H

#include <unistd.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <semaphore.h>

#include <signal.h>

#include <errno.h>

#define MEMLOCATION "/test_shm"
#define O_NORMAL O_CREAT | O_RDWR
#define O_CREATION O_NORMAL | O_EXCL

#define PROT_FLAGS PROT_WRITE | PROT_READ

#define FALSE 0
#define TRUE  1

typedef int bool;

typedef struct {
	int value;
	sem_t sem;
} val_t;

extern int setupSHM(bool);
extern val_t *getSHM(int, bool);
extern void destroySHM(val_t *, int);
extern val_t *getSHMSafe();
extern void safeUnmap();
extern val_t *getSHMSafe();

extern void setupSEM(val_t *);

#endif
