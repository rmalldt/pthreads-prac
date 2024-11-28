#include <string.h>
#include "mheader.h"

void join_threadx(void);
void join_multi(void);
void join_returnval(void);
void test_join_thread_ex(void)
{
    //join_threadx();
    //join_multi();
    //join_returnval();

}

void join_threadx(void)
{
    pthread_t tid;
    int rc, retval;
    int message = 10;
    
    // Create new thread
    rc = pthread_create(&tid, NULL, start_routine, &message);
    if (rc)
        handle_error(rc, "pthread_create");
    
    // Join; wait for the new thread to terminate
    rc = pthread_join(tid, (void **)&retval);
    if (rc)
        handle_error(rc, "pthread_join");

    printf("%s(): completed join, retval = %d\n", __func__, retval);
    pthread_exit(NULL);
}

void *start_routinebusy(void *arg)
{
    int *id = arg;
    double res = 0.0;
    for (int i = 0; i < 1000000; i++)
        res = res + (double) random();

    printf("%s(): thread %d, result = %e\n", __func__, *id, res);
    pthread_exit((void *) 0);
}

void join_multi(void)
{
    pthread_t tids[NTHREADS];
    int rc, retval, i;
    int args[NTHREADS];
    
    // Create child threads
    for (i = 0; i < NTHREADS; i++) {
        printf("%s(): creating thread = %d\n", __func__, i);
        args[i] = i;
        rc = pthread_create(&tids[i], NULL, start_routinebusy, &args[i]);
        if (rc)
            handle_error(rc, "pthread_create");
    }
    
    // Join; wait for the new threads to terminate
    for (i = 0; i < NTHREADS; i++) {
        rc = pthread_join(tids[i], (void *)&retval);
        if (rc)
            handle_error(rc, "pthread_join");
            
        printf("%s(): completed joining thread %d, retval = %d\n", __func__, i, retval);
    }
    pthread_exit(NULL);
}

void *start_routineretval(void *msg)
{
    char *s = malloc(sizeof(msg));  // allocate mem
    strcpy(s, msg);
    int i, j, temp;
    
    printf("%s(): reversing the string = %s\n", __func__, (char *) msg);
    for (i = 0, j = (int)strlen(s) - 1 ; i < j; j--, i++) { // reverse str
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
    return s;   // returning pointer to dynamically allocated storage
}               // this must be free in calling thread

void join_returnval(void)
{
    pthread_t tid;
    int rc;
    char message[] = "Hello there";     // arg for start_routine
    void *retval = NULL;                // to store returned value
    
    // Create new thread
    rc = pthread_create(&tid, NULL, start_routineretval, &message);
    if (rc != 0)
        handle_error(rc, "pthread_create");
    
    // Join; wait for the new thread to terminate
    rc = pthread_join(tid, &retval);           // get returned val
    if (rc)
        handle_error(rc, "pthread_join");
    if (retval != NULL) {                      // print the returned val
        printf("%s(): completed join, retval = %s\n", __func__, (char *)retval);
        free(retval);                          // free the allocated val
    }
    pthread_exit(NULL);
}


