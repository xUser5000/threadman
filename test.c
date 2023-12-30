#include<stdio.h>
#include<pthread.h>

#include "utest.h"
#include "threadman.h"

UTEST_MAIN();

pthread_mutex_t basic_mutex = PTHREAD_MUTEX_INITIALIZER;
int x = 0;
void basic_helper(void* arg) {
    pthread_mutex_lock(&basic_mutex);
    x++;
    pthread_mutex_unlock(&basic_mutex);
}
UTEST(Threadman, basic) {
    threadman_pool_t *pool = threadman_pool_create(100);
    int task_count = 1000;
    for (int i = 0; i < task_count; i++) {
        threadman_submit_task(pool, basic_helper, (void *) &x);
    }
    threadman_wait(pool);
    ASSERT_EQ(x, task_count);
    threadman_pool_free(pool);
    threadman_pool_free(NULL);
}
