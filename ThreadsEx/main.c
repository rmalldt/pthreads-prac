#include <stdio.h>
#include <time.h>

void test_create_thread_ex(void);
void test_cancelcleaup_ex(void);
void test_join_thread_ex(void);
void test_mutex_ex(void);
void test_conditionvar_ex(void);
void test_producer_consumer_ex(void);
void test_readwritelock_ex(void);
void test_reentrant_threadsafe_ex(void);
void test_scheduling_ex(void);
int main(int argc, const char * argv[])
{
    //test_create_thread_ex();
    //test_cancelcleaup_ex();
    //test_join_thread_ex();
    //test_mutex_ex();
    //test_conditionvar_ex();
    test_producer_consumer_ex();
    //test_readwritelock_ex();
    
    //test_reentrant_threadsafe_ex();
    
    //test_scheduling_ex();

    
    return 0;
}

