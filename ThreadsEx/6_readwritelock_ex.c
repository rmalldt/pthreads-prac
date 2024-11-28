#include <stdio.h>
#include "mheader.h"

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
static void check_err(char * str, int rc)
{
    if (rc)
        handle_error(rc, str);
    return;
}

void readwritelock_single(void);
void readwritelock_multi(void);
void test_readwritelock_ex(void)
{
    //readwritelock_single();
    readwritelock_multi();
}

/*-----------------------------------------------------
 * Single-thread example
 */
void *read_routine(void *arg);
void readwritelock_single(void)
{
    pthread_t tid;
    int rc;
    
    // Lock writelock
    printf("Main: locking write lock\n");
    rc = pthread_rwlock_wrlock(&rwlock);
    check_err("pthread_rwlock_wrlock\n", rc);
    
    // Create new thread
    printf("Main: creating a new thread\n");
    rc = pthread_create(&tid, NULL, read_routine, NULL);
    check_err("pthread_create", rc);
    
    printf("Main: waiting holding the write lock\n");
    sleep(5);
    
    // Unlock writelock
    printf("Main: unlocking the write lock\n");
    rc = pthread_rwlock_unlock(&rwlock);
    check_err("pthread_rwlock_unlock\n", rc);
    
    // Join thread
    printf("Main: waiting the new thread to join\n");
    rc = pthread_join(tid, NULL);
    check_err("pthread_join\n", rc);
    
    // Destroy lock
    rc = pthread_rwlock_destroy(&rwlock);
    check_err("pthread_rwlock_destroy\n", rc);
    printf("Main: completed, exiting\n");
    pthread_exit(NULL);
}

void *read_routine(void *arg)
{
    int rc, count = 0;
    
    // Try to lock readlock
    printf("Thread: getting read lock\n");
    RETRY:
    rc = pthread_rwlock_tryrdlock(&rwlock);
    if (rc == EBUSY) {
        if (count >= 10) {
            printf("Thread: retried to many times, exiting failure\n ");
            exit(EXIT_FAILURE);
        }
        ++count;
        printf("Thread: couldn't get the lock, do other work and RETRY...\n");
        sleep(1);
        goto RETRY;
    }
    check_err("pthread_rwlock_tryrdlock\n", rc);
    sleep(2);
    
    // Unlock readlock
    printf("Thread: unlocking read lock\n");
    rc = pthread_rwlock_unlock(&rwlock);
    check_err("pthread_rwlock_unlock\n", rc);
    
    printf("Thread: complteted, exiting\n");
    return NULL;
}


/*-----------------------------------------------------
 * Multi-thread example
 */
void *write_routine(void *arg);
void readwritelock_multi(void)
{
    pthread_t tid1, tid2;
    int rc;
    
    // Lock writelock
    printf("Main: locking write lock\n");
    rc = pthread_rwlock_wrlock(&rwlock);
    check_err("pthread_rwlock_wrlock\n", rc);
    
    // Create new threads
    printf("Main: creating thread 1\n");
    rc = pthread_create(&tid1, NULL, write_routine, NULL);
    check_err("pthread_create thread 1", rc);
    printf("Main: creating thread 2\n");
    rc = pthread_create(&tid2, NULL, write_routine, NULL);
    check_err("pthread_create thread 2", rc);
    
    printf("Main: waiting holding the write lock\n");
    sleep(1);
    
    // Unlock writelock
    printf("Main: unlocking the write lock\n");
    rc = pthread_rwlock_unlock(&rwlock);
    check_err("pthread_rwlock_unlock\n", rc);
    
    // Join threads
    printf("Main: waiting the threads to join\n");
    rc = pthread_join(tid1, NULL);
    check_err("pthread_join\n", rc);
    rc = pthread_join(tid2, NULL);
    check_err("pthread_join\n", rc);
    
    // Destroy lock
    rc = pthread_rwlock_destroy(&rwlock);
    check_err("pthread_rwlock_destroy\n", rc);
    printf("Main: completed, exiting\n");
    
    pthread_exit(NULL);
}

void *write_routine(void *arg)
{
    int rc, count = 0;
    
    // Try to lock writelock
    printf("Thread %p: getting write lock\n", pthread_self());
    RETRY:
    rc = pthread_rwlock_trywrlock(&rwlock);
    if (rc == EBUSY) {
        if (count >= 10) {
            printf("Thread %p: retried to many times, exiting failure\n", pthread_self());
            exit(EXIT_FAILURE);
        }
        ++count;
        printf("Thread %p: couldn't get the lock, do other work and RETRY...\n", pthread_self());
        sleep(1);
        goto RETRY;
    }
    check_err("pthread_rwlock_trywrlock\n", rc);
    sleep(2);
    
    // Unlock writelock
    printf("Thread %p: unlocking write lock\n", pthread_self());
    rc = pthread_rwlock_unlock(&rwlock);
    check_err("pthread_rwlock_unlock\n", rc);
    
    printf("Thread %p: completed, exiting\n", pthread_self());
    return NULL;
}
