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

#ifndef TRUE
# define TRUE  1
#endif
#ifndef FALSE
# define FALSE 0 
#endif

#define uint unsigned int
#define bool int

#define D_TYPES 4
#define D_SIZE 50

#define N_CONSUMERS 50
#define N_PRODUCERS 20

#define gettid() SYSCALL(SYS_gettid)



#endif
