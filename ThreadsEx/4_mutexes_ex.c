#include <string.h>
#include "mheader.h"

void shareddata_unsynced(void);
void shareddata_synced(void);
void mutex_tosequence(void);
void test_mutex_ex(void)
{
    //shareddata_unsynced();
    //shareddata_synced();
    //mutex_tosequence();
}

/*--------------------------------------------------------------------------
 * Ex: Synchronisation Problem
 */
#define TARGET_COUNT 1000000
void *start_routineunsynced(void *arg)
{
    pthread_t tid = pthread_self();
    int *count = arg;
    int i;
    
    printf("%s(): thread %p stating count at = %d\n", __func__, tid, *count);
    for (i = 0; i < TARGET_COUNT; i++)
        (*count)++;
    printf("%s(): thread %p final count = %d\n", __func__, tid, *count);
    pthread_exit(NULL);
}

void shareddata_unsynced(void)
{
    pthread_t tids[2];          // two threads
    int rc, i;
    unsigned int count = 0;     // shared data
    printf("%s(): expected count = %d\n", __func__, 2 * TARGET_COUNT);
    
    // Create threads
    for (i = 0; i < 2; i++) {
        rc = pthread_create(&tids[i], NULL, start_routineunsynced, &count);
        if (rc)
            handle_error(rc, "pthread_create");
        printf("%s(): created thread %p\n", __func__, tids[i]);
    }
    
    // Join threads
    for (i = 0; i < 2; i++) {
        rc = pthread_join(tids[i], NULL);
        if (rc)
            handle_error(rc, "pthread_create");
        printf("%s(): completed joining thread %p\n", __func__, tids[i]);
    }
    
    // Check result
    if (count != (2 * TARGET_COUNT))
        printf("%s(): error, total count = %u\n", __func__, count);
    else
        printf("%s(): ok, total count = %u\n", __func__, count);
    pthread_exit(NULL);
}

/*-------------------------------------------------------------------------
 * Ex: Synchronization using mutex
 */
pthread_mutex_t sdlock;   // mutex
void *start_routinesynced(void *arg)
{
    // Lock
    pthread_mutex_lock(&sdlock);
    
    // Execute critical section
    pthread_t tid = pthread_self();
    int *count = arg;
    int i;
    
    printf("%s(): thread %p stating count at = %d\n", __func__, tid, *count);
    for (i = 0; i < TARGET_COUNT; i++)
        (*count)++;
    printf("%s(): thread %p final count = %d\n", __func__, tid, *count);
    
    // Unlock
    pthread_mutex_unlock(&sdlock);
    
    pthread_exit(NULL);
}

void shareddata_synced(void)
{
    pthread_t tids[2];          // two threads
    int rc, i;
    unsigned int count = 0;     // shared data
    
    // Init mutex
    rc = pthread_mutex_init(&sdlock, NULL);
    if (rc)
        handle_error(rc, "pthread_mutex_init");
    
    printf("%s(): expected count = %d\n", __func__, 2 * TARGET_COUNT);
    
    // Create threads
    for (i = 0; i < 2; i++) {
        rc = pthread_create(&tids[i], NULL, start_routinesynced, &count);
        if (rc)
            handle_error(rc, "pthread_create");
        printf("%s(): created thread %p\n", __func__, tids[i]);
    }
    
    // Join threads
    for (i = 0; i < 2; i++) {
        rc = pthread_join(tids[i], NULL);
        if (rc)
            handle_error(rc, "pthread_join");
        printf("%s(): completed joining thread %p\n", __func__, tids[i]);
    }
    
    // Check result
    if (count != (2 * TARGET_COUNT))
        printf("%s(): error, total count = %u\n", __func__, count);
    else
        printf("%s(): ok, total count = %u\n", __func__, count);
    
    pthread_mutex_destroy(&sdlock);
    pthread_exit(NULL);
}

/*-------------------------------------------------------------------------------
 * Ex: One-time initialization mutex and Exclusive initialization
 */
#define WORDLEN 30
static char *wordlist[] = {             // Search List
    "char", "short", "int",
    "long", "float", "double",
    "void", "static", "const",
    "return", "restrict", "struct",
    "if", "else", "elseif",
    "for", "while", "do"
};
static char listsize = sizeof(wordlist)/ sizeof(char *);    // List Size

// One-time initialization
static pthread_once_t init_seq = PTHREAD_ONCE_INIT;
static pthread_mutex_t seqlock;
void init_seqlock(void) {
    pthread_mutex_init(&seqlock, NULL);
}

void *start_routinefindwords(void *arg)
{
    // Lock
    pthread_mutex_lock(&seqlock);
    pthread_t tid = pthread_self();     // thread id
    char *data = arg;                   // routine argument
    printf("===> %s(): lock acquired, thread %p\n", __func__, tid);
    
    // Execute critical section
    char *retval = NULL;                // data to be returned
    int i, found = 0;
    for (i = 0; i < listsize; i++) {
        if ((strcmp(data, wordlist[i])) == 0) {
            printf("found \"%s\", thread %p\n", wordlist[i], tid);
            found = 1;
            retval = malloc(50);
            strcpy(retval, wordlist[i]);
            strcat(retval, ": FOUND");
            break;
        }
    }
    if (!found)
        printf("couldn't find \"%s\", thread %p\n", data, tid);
    
    // Unlock
    printf("%s(): releasing lock, thread %p ===>\n", __func__, tid);
    pthread_mutex_unlock(&seqlock);
    return retval;
}

void mutex_tosequence(void)
{
    pthread_t tids[NTHREADS];   // 5 threads
    int rc, i;
    void *retval;               // to store returned value
    
    char *args[NTHREADS] = {
        "static",
        "include",
        "struct",
        "define",
        "else"
    };
    
    // Init mutex
    pthread_once(&init_seq, init_seqlock);
    
    // Create threads, make each thread search one word.
    // If word found, do dyna alloc and return the storage to this thread
    for (i = 0; i < NTHREADS; i++) {
        rc = pthread_create(&tids[i], NULL, start_routinefindwords, args[i]);
        if (rc)
            handle_error(rc, "pthread_create");
    }
    
    // Join threads and print the returned value
    for (i = 0; i < NTHREADS; i++) {
        rc = pthread_join(tids[i], &retval);
        if (rc)
            handle_error(rc, "pthread_join");
        printf("%s(): completed joining thread %p\n", __func__, tids[i]);
        
        if (retval != NULL) {
            printf("%s(): returned \"%s\", thread %p\n", __func__, (char *)retval, tids[i]);
            free(retval);   // free mem which was allocated in start_routine
        }
    }
    pthread_exit(NULL);
}
