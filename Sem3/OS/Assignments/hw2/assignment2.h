#ifndef __ASSIGNMENT2_H
#define __ASSIGNMENT2_H

#define _GNU_SOURCE

#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/syscall.h>

#include <signal.h>

#include <assert.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define D_TYPES 4
#define D_SIZE 1000

#define N_CONSUMERS 50
#define N_PRODUCERS 30
#define N_THREADS (N_CONSUMERS+N_PRODUCERS)

#define N_CONS_TRIAL (100 * 12)

#define gettid() syscall(SYS_gettid)

#endif
