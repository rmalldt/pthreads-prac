#include <stdio.h>
#include "mheader.h"

void set_policypriority_atinit(void);
void set_contentionscope(void);
void test_scheduling_ex(void)
{
    //set_policypriority_atinit();
    //set_contentionscope();
}

void set_policypriority_atinit(void)
{
    // Set priority with scheduling parameter
    struct sched_param schedparam;
    schedparam.sched_priority = 3;
   
    // Thread attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // Set inherit-scheduler attribute
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    
    // Set policy
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    // Set scheduling parameter
    pthread_attr_getschedparam(&attr, &schedparam);

    // Create thread
    pthread_t tid;
    int arg = 10;
    pthread_create(&tid, &attr, start_routine, &arg);
    
    // Destroy thread attribute
    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}

void set_contentionscope(void)
{
    // Thread attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // Set inherit-scheduler attribute
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    int rc;
    
    rc = pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
    if (rc)
        handle_error(rc, "pthread_attr_setscope");
}
