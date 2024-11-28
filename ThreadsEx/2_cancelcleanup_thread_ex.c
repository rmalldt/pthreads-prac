#include "mheader.h"

void cancel_multthreads(void);
void cancel_thread(void);
void cancelcleanup_thread(int, char *[]);
void test_cancelcleaup_ex(void)
{
    //cancel_thread();
    cancel_multthreads();
 
    // Test 1:
//    int argc = 1;
//    char *argv[1] = {
//        "a.out"
//    };
//    cancelcleanup_thread(argc, argv);
    
    // Test 2:
//    int argc = 2;
//    char *argv[2] = {
//        "a.out",
//        "x"
//    };
//    cancelcleanup_thread(argc, argv);
    
    // Test 3:
//    int argc = 3;
//    char *argv[3] = {
//        "a.out",
//        "x",
//        "1"
//    };
//    cancelcleanup_thread(argc, argv);
    
}

/* Cancelling Thread */
void *start_routinecancel(void *arg)
{
    int rc; // cancellation state
    
    // Disable cancellation for a while, so that we don't immediately
    // react to the cancellation request
    rc = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (rc)
        handle_error(rc, "pthread_setcancelstate");
    printf("%s(): started, cancellation disabled\n", __func__);
    sleep(3);
    
    // Enable cancellation
    printf("%s(): about to enable cancellation\n", __func__);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (rc)
        handle_error(rc, "pthread_setcancelstate");

    sleep(5);   // cancellation point
    
    // Should never get here
    printf("%s(): not cancelled!\n", __func__);
    pthread_exit(NULL);
}

void cancel_thread(void)
{
    pthread_t tid;
    void *retval;
    int rc;
    
    rc = pthread_create(&tid, NULL, start_routinecancel, NULL);
    if (rc)
        handle_error(rc, "pthread_create");
    sleep(3);    // give new thread time to get started
    
    printf("%s(): sending cancellation request\n", __func__);
    rc = pthread_cancel(tid);
    if (rc)
        handle_error(rc, "pthread_join");
    
    // Join with the thread to see what its exit status
    rc = pthread_join(tid, &retval);
    if (rc)
        handle_error(rc, "pthread_join");
    
    if (retval == PTHREAD_CANCELED)
        printf("%s(): thread was cancelled\n", __func__);
    else
        printf("%s(): thread wasn't cancelled\n", __func__);
    
    pthread_exit(NULL);
}

void *start_routinecancelall(void *id)
{
    int i, rc, state;
    int *n = id;
    
    // Disable cancelability
    rc = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &state);
    if (rc)
        handle_error(rc, "pthread_setcancelstate");
    printf("%s(): started, cancellation disabled for thread %d\n", __func__, *n);
    sleep(2);
    
    // Enable cancelability
    printf("%s(): about to enable thread cancellation for thread %d\n", __func__, *n);
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &state);
    if (rc)
        handle_error(rc, "pthread_setcancelstate");
    
    // After some time inside loop, this thread will act upon the cancellation request
    // because the cancelability is enabled and printf is one of the functions that
    // provide implicit cancellation point.
    for (i = 0; i < 10; i++) {
        printf("%d. Hello from thread %d\n", i, *n);  // printf is a cancellation point
        sleep(2);   // sleep is also a cancellation point
    }
    
    // Alternatively, add explicit cancellation point
    //pthread_testcancel();
    
    // Should never get here
    printf("%s(): not cancelled thread %d!\n", __func__, *n);
    pthread_exit(NULL);
}

void cancel_multthreads(void)
{
    pthread_t tids[NTHREADS];
    int rc, i;
    int ids[NTHREADS];
    void *retval;
    
    // Create five threads
    for (i = 0; i < NTHREADS; i++) {
        ids[i] = i;
        rc = pthread_create(&tids[i], NULL, start_routinecancelall, &ids[i]);
        if (rc)
            handle_error(rc, "pthread_create");
    }

    sleep(5); // give new threads time to get started
    
    // Send cancellation request to all threads
    for (i = 0; i < NTHREADS; i++) {
        printf("%s(): sending cancellation request to thread %d\n", __func__, i);
        rc = pthread_cancel(tids[i]);
        if (rc)
            handle_error(rc, "pthread_cancel");
    }
    
    // Join all threads to check the cancellation status
    for (i = 0; i < NTHREADS; i++) {
        printf("%s(): joining thread %d\n", __func__, i);
        rc = pthread_join(tids[i], &retval);
        if (rc)
            handle_error(rc, "pthread_cancel");
        
        if (retval == PTHREAD_CANCELED)
            printf("%s(): thread %d was cancelled\n", __func__, i);
        else
            printf("%s(): thread %d wasn't cancelled\n", __func__, i);
    }
    
    pthread_exit(NULL);
}


/*----------------------------------------------------------------------------------------
 * Cancelling and Cleanup Handlers Example
 */
// Varaibles for cleanup handler example
static int done = 0;
static int cleanup_pop_arg = 0;
static int mcount = 0;

static void cleanup_handler(void *arg)
{
    printf("%s(): cleanup handler called\n", __func__);
    mcount = 0;
}

void *start_routinecancelclean(void *arg)
{
    time_t curr;
    printf("%s(): new thread started\n", __func__);
    
    pthread_cleanup_push(cleanup_handler, NULL);
    
    curr = time(NULL);
    while (!done) {
        pthread_testcancel();               // explicit cancellation point
        if (curr < time(NULL)) {
            curr = time(NULL);
            printf("count = %d\n", mcount);  // cancellation point
            mcount++;
        }
    }
    pthread_cleanup_pop(cleanup_pop_arg);
    return NULL;
}

void cancelcleanup_thread(int argc, char * argv[])
{
    pthread_t tid;
    int rc;
    void *retval;
    
    // Create thread
    rc = pthread_create(&tid, NULL, start_routinecancelclean, NULL);
    if (rc)
        handle_error(rc, "pthread_create");
    
    sleep(2);   // give new threads to get started
    if (argc > 1) {
        if (argc > 2)
            cleanup_pop_arg = atoi(argv[2]);
        done = 1;
    } else {
        printf("%s(): sending cancellation request\n", __func__);
        rc = pthread_cancel(tid);
        if (rc)
            handle_error(rc, "pthread_cancel");
    }
    
    // Join the threads to check the cancellation status
    rc = pthread_join(tid, &retval);
    if (rc)
        handle_error(rc, "pthread_join");
    
    if (retval == PTHREAD_CANCELED)
        printf("%s(): thread was cancelled, count = %d\n", __func__, mcount);
    else
        printf("%s(): thread terminated normally, count = %d\n", __func__, mcount);
    exit(EXIT_SUCCESS);
}
