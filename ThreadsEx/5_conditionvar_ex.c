#include "mheader.h"
#include <time.h>

void inefficient_join(void);
void manual_join(void);
void timedwait_ex(void);
void timedwait_ex2(void);
void test_conditionvar_ex(void)
{
    //inefficient_join();
    //manual_join();
    timedwait_ex();
    
}

/*------------------------------------------------------------------
 * Ex: Manual Inefficient Join (Busy waiting)
 */
int mdone = 0;                   // condition
void *start_child(void *arg)
{
    printf("child thread: begin\n");
    mdone = 1;
    return NULL;
}

void inefficient_join(void)
{
    printf("initial thread: begin\n");
    pthread_t tid;
    int rc;

    rc = pthread_create(&tid, NULL, start_child, NULL);
    if (rc)
        handle_error(rc, "pthread_create");

    /*
     * Wait for the child thread to terminate using while loop.
     * Using loop is very inefficient since this thread continues
     * to remain active and uses CPU resources unitl child thread
     * is terminated.
     */
    while (!mdone) {
        printf("initial thread: waiting...\n");
        sleep(0.1);
    }
        
    printf("initial thread: end\n");
    pthread_exit(NULL);
}

/*------------------------------------------------------------------
 * Ex: Manual Join
 */
int mstate = 0;

// Static mutex and condition variable
pthread_mutex_t tmutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tcond    = PTHREAD_COND_INITIALIZER;

void texit(void)
{
    pthread_mutex_lock(&tmutex);     // lock
    mstate = 1;                      // make change
    pthread_cond_signal(&tcond);     // signal waiting thread
    pthread_mutex_unlock(&tmutex);   // unlock
}

void *startt(void *arg)
{
    printf("child thread: begin\n");
    texit();
    return NULL;
}

void tjoin(void)
{
    pthread_mutex_lock(&tmutex);            // lock
    while (!mstate)                         // while not desired cond
        pthread_cond_wait(&tcond, &tmutex); // wait on the cond by releasing lock and sleep
    pthread_mutex_unlock(&tmutex);          // unlock
}

void manual_join(void)
{
    printf("initial thread: begin\n");
    pthread_t tid;
    int rc;

    rc = pthread_create(&tid, NULL, startt, NULL);
    if (rc)
        handle_error(rc, "pthread_create");

    tjoin();

    printf("initial thread: end\n");
    pthread_exit(NULL);
}

void timedwait_ex(void)
{
    pthread_mutex_t m;
    pthread_cond_t c;
    struct timespec ts;
    int rc;
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&c, NULL);
    
    pthread_mutex_lock(&m);
    clock_gettime(CLOCK_REALTIME, &ts); // get current time
    ts.tv_sec += 3;                     // set absolute time = current + timeout
    rc = 0;
    
    while (rc == 0)                              // if timeout, rc = ETIMEDOUT = 60
        rc = pthread_cond_timedwait(&c, &m, &ts);// ts is set to absolute time above
    
    pthread_mutex_unlock(&m);
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&c);
}
