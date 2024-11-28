#ifndef mheader_h
#define mheader_h

#define NTHREADS    5
#define NMSGS       2
#define MAXSIZE     5

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define handle_error(en, msg)\
    { errno = en; perror(msg); exit(EXIT_FAILURE); };

typedef struct mdata {
    int id;
    char *message;
} MDATA;

//struct cond_bundle {
//    int                 cond_predicate;
//    pthread_mutex_t     cond_lock;
//    pthread_cond_t      cond_var;
//} COND_BUNDLE;

void *start_routine(void *);
void *start_routinebusy(void *);

#endif /* mheader_h */
