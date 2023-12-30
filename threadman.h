#pragma once

#include<stdbool.h>
#include<pthread.h>

#define MAX_TASKS 1024
#define MAX_THREADS 1024

typedef struct threadman_pool_t threadman_pool_t;

typedef enum threadman_task_status {
    QUEUED = 0,
    RUNNING = 1,
    FINISHED = 2
} threadman_task_status_t;

typedef void (*threadman_func_t)(void* args);

threadman_pool_t *threadman_pool_create(int thread_count);

bool threadman_submit_task(threadman_pool_t *pool, threadman_func_t func, void* args);

void threadman_wait(threadman_pool_t *pool);

void threadman_pool_free(threadman_pool_t *pool);
