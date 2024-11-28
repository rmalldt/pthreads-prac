#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*----------------------------------------------------------------------
 * One-time Initialization Example
 */
long num;               // global var
void init_random(void)  // initializer
{
    srandom(0);
    num = random();
}

// In single-threaded program
static int israndom_initialized = 0;    // static var
void random_func(void)                  // func that calls initializer
{
    if (israndom_initialized == 0) {
        init_random();
        israndom_initialized = 1;
    }
}

// In multi-threaded program
static pthread_once_t is_initialized = PTHREAD_ONCE_INIT;
void random_func1(void)                 // func that calls initializer
{
    pthread_once(&is_initialized, init_random);
}

/*----------------------------------------------------------
 * Normal Vs Static Initializer
 */
// Normal, without static initialization, a self-initializing routine foo()
static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static pthread_mutex_t lock;
void init_mutex(void)
{
    pthread_mutex_init(&lock, NULL);
}

void foo(void)
{
    pthread_once(&init_once, init_mutex);
    pthread_mutex_lock(&lock);
    
    // Do work
    
    pthread_mutex_unlock(&lock);
}

// With static initialization, the same routine can be:
static pthread_mutex_t static_mutex = PTHREAD_MUTEX_INITIALIZER;
void bar(void) 
{
    pthread_mutex_lock(&static_mutex);
    
    // Do work
    
    pthread_mutex_unlock(&static_mutex);
}

