#include <stdio.h>
#include "mheader.h"

void creat_threadx(void);
void mult_threadx(void);
void sleep_threadx(void);
void test_create_thread_ex(void)
{
    //creat_threadx();
    //mult_threadx();
    //sleep_threadx();
}

void creat_threadx(void)
{
    pthread_t tid;       // new thread to be created
    pthread_attr_t attr; // new thread's attribute
    int rc;              // return code for pthread_create
    int message = 10;    // arg for start_routine
    
    // Create thread attribute
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    // Create new thread
    rc = pthread_create(&tid, NULL, start_routine, &message);
    if (rc) {           // check rc to ensure new thread is created
        printf("Error: pthread_create return code = %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Main thread ID = %p\n", pthread_self());
    pthread_exit(NULL); // allows other thread to continue execution
}

void mult_threadx(void)
{
    pthread_t tids[NTHREADS];
    int rc;
    int message[NTHREADS];
    
    for (int i = 0; i < NTHREADS; i++) {
        printf("Creating thread %d\n", i);
        message[i] = i;
        rc = pthread_create(&tids[i], NULL, start_routine, &message[i]);
        if (rc) {   // if rc != 0, print error
            printf("Error: pthread_create return code = %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    printf("Main thread ID = %p\n", pthread_self());
    pthread_exit(NULL);
}

void sleep_threadx(void)
{
    pthread_t tid;
    int rc;

    int message = 10;
    rc = pthread_create(&tid, NULL, start_routine, &message);
    if (rc) {
        printf("Error: pthread_create return code = %d\n", rc);
        exit(EXIT_FAILURE);
    }
    sleep(1);   // sleep main thread for 1s
    printf("Main thread: got here\n");
    pthread_exit(NULL);
}

/*-------------------------------------------------------------------------------
 * Functions to be used as the third argument (start_routine) for pthread_create
 */
void *start_routine(void *arg)
{
    int *message = arg;    // copy arg pointer to appropriate pointer type
    printf("%s(): new thread ID = %p, message = %d\n", __func__, pthread_self(), *message);
    pthread_exit(NULL);   // terminate this thread
}

