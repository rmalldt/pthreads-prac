#include <assert.h>
#include <time.h>
#include <semaphore.h>
#include <time.h>
#include "mheader.h"

int loops = 10;                        // times to repeat: produce and consume
void producer_consumer_v1(void);
void producer_consumer_x(void);
void test_producer_consumer_ex(void)
{
    //producer_consumer_v1();
    //producer_consumer_x();
}

/*------------------------------------------------------------------------
 * Producer Consumer Problem solution 1
 *  - Only works for 1 producer and 1 consumer.
 */
int buffer1;                // shared buffer, int type for simplicity
int isfilled = 0;           // intially empty
pthread_mutex_t mutex1      = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1        = PTHREAD_COND_INITIALIZER;
void *producerv1(void *arg);
void *consumerv1(void *arg);
void producer_consumer_v1(void)
{
    pthread_t pro;
    pthread_t con1;
    pthread_t con2;
    int rc;
    
    // Create one producer and one consumer
    rc = pthread_create(&pro, NULL, producerv1, NULL);
    if (rc)
        handle_error(rc, "pthread_create producer\n");
    rc = pthread_create(&con1, NULL, consumerv1, NULL);
    if (rc)
        handle_error(rc, "pthread_create consumer\n");
    rc = pthread_create(&con2, NULL, consumerv1, NULL);
    if (rc)
        handle_error(rc, "pthread_create consumer\n");
    
    // Join threads
    rc = pthread_join(pro, NULL);
    if (rc)
        handle_error(rc, "pthread_join producer");
    rc = pthread_join(con1, NULL);
    if (rc)
        handle_error(rc, "pthread_join producer");
    rc = pthread_join(con2, NULL);
    if (rc)
        handle_error(rc, "pthread_join producer");
    
    // Destroy mutex and CV
    pthread_cond_destroy(&cond1);
    pthread_mutex_destroy(&mutex1);
    printf("%s(): End\n", __func__);
    pthread_exit(NULL);
}

void putv1(int value);
int getv1(void);
void *producerv1(void *arg)
{
    int i;
    for (i = 0; i < loops; i++) {
        pthread_mutex_lock(&mutex1);
        while (isfilled)                        // condition: must be empty
            pthread_cond_wait(&cond1, &mutex1); // otherwise, wait for the condition
        putv1(i);
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&mutex1);
    }
    printf("producer exiting thread\n");
    pthread_exit(NULL);
}

void *consumerv1(void *arg)
{
    int i;
    for (i = 0 ; i < loops; i++) {
        pthread_mutex_lock(&mutex1);
        while (!isfilled)                       // condition: must be filled
            pthread_cond_wait(&cond1, &mutex1); // otherwise, wait for the condition
        int temp = getv1();
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&mutex1);
        printf("%d\n", temp);
        //sleep(1);
    }
    printf("consumer exiting thread\n");
    pthread_exit(NULL);
}

// Condition: Put data only if buffer is empty.
void putv1(int value)
{
    assert(!isfilled);  // assert buffer is empty
    isfilled = 1;       // mark the buffer full
    buffer1 = value;    // put value
}

// Condition: Get data only if buffer is filled
int getv1(void)
{
    assert(isfilled);   // assert buffer is full
    isfilled = 0;       // mark the buffer empty
    return buffer1;     // return value
}

/*------------------------------------------------------------------------
 * Producer Consumer Problem solution
 *  - Works for multiple producers and consumers.
 */
int bufferx[MAXSIZE];                                           // buffer size
int bufcount                    = 0;                            // buffer count
int putindex                    = 0;                            // put index
int getindex                    = 0;                            // get index
int stop                        = 0;                            // stop processing
pthread_mutex_t mutexx          = PTHREAD_MUTEX_INITIALIZER;    // shared mutex
pthread_cond_t empty            = PTHREAD_COND_INITIALIZER;     // condition: empty
pthread_cond_t filled           = PTHREAD_COND_INITIALIZER;     // condition: filled
void *producerx(void *arg);
void *consumerx(void *arg);

void *producerz(void *arg);
void *consumerz(void *arg);

void putx(int value)
{
    bufferx[putindex] = value;
    putindex = (putindex + 1) % MAXSIZE; // no exceed MAXSIZE
    bufcount++;
}

int getx(void)
{
    int temp = bufferx[getindex];
    getindex = (getindex + 1) % MAXSIZE; // no exceed MAXSIZE
    bufcount--;
    return temp;
}

void producer_consumer_x(void)
{
    pthread_t pro;
    pthread_t con1;
    pthread_t con2;
    int rc;
    
    // Create one producer two consumers
    rc = pthread_create(&pro, NULL, producerz, NULL);
    if (rc)
        handle_error(rc, "pthread_create producer\n");
    rc = pthread_create(&con1, NULL, consumerz, NULL);
    if (rc)
        handle_error(rc, "pthread_create consumer1\n");
    rc = pthread_create(&con2, NULL, consumerz, NULL);
    if (rc)
        handle_error(rc, "pthread_create consumer1\n");
    
    // Join all threads
    pthread_join(pro, NULL);
    pthread_join(con1, NULL);
    pthread_join(con2, NULL);
    
    printf("%s(): End\n", __func__);
    pthread_exit(NULL);
}

void *producerx(void *arg)
{
    int i;
    for (i = 0; i < loops; i++) {
        pthread_mutex_lock(&mutexx);
        while (bufcount == MAXSIZE) {
            printf("producer waiting\n");
            pthread_cond_wait(&empty, &mutexx);
            printf("producer wakesup\n");
        }
            
        printf("producer putting %d from the buffer\n", i);
        putx(i);
        pthread_cond_signal(&filled);
        pthread_mutex_unlock(&mutexx);
    
    }
    /*
     * By the time producer is exiting, if there is:
     *  - one consumer waiting:  producer will signal it; process ends.
     *  - two consumers waiting: producer will signal one of the consumers, and
     *                           other will be signaled by the exiting consumer;
     *                           process ends.
     */
    pthread_mutex_lock(&mutexx);
    stop++;
    pthread_cond_signal(&filled);
    pthread_mutex_unlock(&mutexx);
    printf("producer exited: stop = %d\n", stop);
    pthread_exit(NULL);
}

void *consumerx(void *arg)
{
    pthread_t tid = pthread_self();
    int i;
    
    for (i = 0 ; i < loops; i++) {
        pthread_mutex_lock(&mutexx);
        while (bufcount == 0 && !stop) {
            printf("consumer %p waiting\n", tid);
            pthread_cond_wait(&filled, &mutexx);
            printf("consumer %p wakesup\n", tid);
        }
        int temp = getx();
        printf("consumer %p getting %d from the buffer\n", tid, temp);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutexx);
    }
    /*
     * By the time producer is exiting, if there is:
     *  - one consumer waiting:  producer will signal it; process ends.
     *  - two consumers waiting: producer will signal one of the consumers, and
     *                           other will be signaled by this exiting consumer;
     *                           process ends.
     */
    pthread_mutex_lock(&mutexx);
    stop++;
    pthread_cond_signal(&filled);
    pthread_mutex_unlock(&mutexx);
    printf("consumer %p exited, stop = %d\n", tid, stop);
    pthread_exit(NULL);
}

/*-----------------------------------------------------------------
 * Producer Consumer Problem solution
 *  - Consumers with timed wait
 */
void *producerz(void *arg)
{
    int i;
    for (i = 0; i < loops; i++) {
        pthread_mutex_lock(&mutexx);
        while (bufcount == MAXSIZE) {
            printf("producer waiting\n");
            pthread_cond_wait(&empty, &mutexx);
            printf("producer wakesup\n");
        }
        printf("producer putting %d from the buffer\n", i);
        putx(i);
        pthread_cond_signal(&filled);
        pthread_mutex_unlock(&mutexx);
    }
    printf("producer exited\n");
    pthread_exit(NULL);
}

void *consumerz(void *arg)
{
    pthread_t tid = pthread_self();
    struct timespec ts;
    int i, rc;
    
    for (i = 0 ; i < loops; i++) {
        pthread_mutex_lock(&mutexx);
        clock_gettime(CLOCK_REALTIME, &ts); // get current time
        ts.tv_sec += 2;                     // set absolute time = current time + timeout period
        rc = 0;                             // to store return val from pthread_cond_timedwait
        while (bufcount == 0 && rc == 0) {  // if timedout, rc = ETIMEDOUT = 60;
            printf("consumer %p waiting\n", tid);
            rc = pthread_cond_timedwait(&filled, &mutexx, &ts); // ts is set to absolute time above
            printf("consumer %p wakesup\n", tid);
        }
        int temp = getx();
        printf("consumer %p getting %d from the buffer\n", tid, temp);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutexx);
        if (rc >= ETIMEDOUT)
            break;
    }
    printf("consumer %p exited\n", tid);
    pthread_exit(NULL);
}

