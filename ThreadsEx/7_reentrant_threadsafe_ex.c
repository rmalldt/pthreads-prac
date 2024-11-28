#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mheader.h"


int diff(int x, int y);
char *strtoupper_v1(char *s);
char *strtoupper_v2(char *s);
void strtoupper_v3(char *in, char *out);
void test_count(void);
void test_reentrant_threadsafe_ex(void)
{
    //printf("%d\n", diff(7, 2));
    
    
//    char *res1 = strtoupper_v1("kale");
//    printf("%s\n", res1);
//    
//    char *res2 = strtoupper_v2("kopila");
//    printf("%s\n", res2);
//    free(res2);         // caller freed buffer
//    
//    
//    char *res3 = malloc(64);
//    strtoupper_v3("taizoke", res3);
//    printf("%s\n", res3);
//    free(res3);         // caller freed buffer
    
    
    test_count();
}

/*--------------------------------------------------------------
 * Reentrant and Non-reentrant functions example
 */

// Non-reentrant function
#define MAX_STR 64
char *strtoupper_v1(char *s)
{
    static char buffer[MAX_STR];    // static var
    int i;
    
    for (i = 0; s[i]; i++)
        buffer[i] = toupper(s[i]);
    buffer[i] = 0;
    return buffer;
}

// Reentrant but not recommended (Same interface/function signature)
char *strtoupper_v2(char *s)
{
    char *buffer;
    int i;
    
    /*
     * Dynamically allocated buffer that needs error-checking.
     * It is caller duty to free the buffer later.
     */
    buffer = malloc(MAX_STR);
    if (!buffer) {
        printf("Error: fail allocating buffer, exiting\n");
        exit(EXIT_FAILURE);
    }
    
    for (i = 0; s[i]; i++)
        buffer[i] = toupper(s[i]);
    buffer[i] = 0;
    return buffer;
}

// Reentrant and recommended (Modified interface/function signature)
void strtoupper_v3(char *in, char *out)
{
    int i;
    
    for (i = 0; in[i]; i++)
        out[i] = toupper(in[i]);
    out[i] = 0;
}

// Non-reentrant function
char lowercase_v1(char *s)
{
    static char *buffer;    // static var
    static int i;           // static var
    char c = 0;
    
    // Stores the string
    if (s != NULL)
        buffer = s;
  
    // Searches for lowercase char
    for (i = 0; (c = buffer[i]); i++) {
        if (islower(c)) {
            i++;
            break;
        }
    }
    return c;
}

// Reentrant function
char lowercase_v2(char *s, int *index)  // caller provides data
{
    char c = 0;
  
    // No initialization - the caller should have done it
    
    // Searches for lowercase char
    for (; (c = s[*index]); (*index)++) {
        if (islower(c)) {
            (*index)++;
            break;
        }
    }
    return c;
}

/*----------------------------------------------------------
 * Ex: Threadsafe function
 *  - does not contains any static or shared variable
 */
int diff(int x, int y)
{
    int res;
    res = y - x;
    if (res < 0)
        res = -res;
    
    return res;
}

// Thread-unsafe functions
void *increment_count_v1(void *arg)
{
    static int count = 0;
    count++;
    return (void *)count;
}

// Threadsafe function
void *increment_count_v2(void *arg)
{
    static int count = 0;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    
    pthread_mutex_lock(&lock);
    count++;
    pthread_mutex_unlock(&lock);
    
    return (void *)count;
}

void test_count(void)
{
    pthread_t tid1;
    pthread_t tid2;
    
    void *res;
    
    pthread_create(&tid1, NULL, increment_count_v2, NULL);
    pthread_create(&tid2, NULL, increment_count_v2, NULL);

    sleep(2);
    
    pthread_join(tid1, &res);
    pthread_join(tid2, &res);
    
    printf("%d\n", (int) res);
    
    pthread_exit(NULL);
}
