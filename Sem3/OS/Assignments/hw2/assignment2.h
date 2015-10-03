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

#define gettid() SYSCALL(SYS_gettid)

typedef struct mg_eventcounter mevt_t;
typedef struct mg_sequencer mseq_t;

extern int evtc_init(mevt_t *);
extern void await(mevt_t *, uint);
extern void signal(mevt_t *);

extern uint seq_init(mseq_t *);
extern uint ticket(mseq_t *);


#endif
