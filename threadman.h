#pragma once

#include<stdbool.h>
#include<pthread.h>

#define MAX_TASKS 1024
#define MAX_THREADS 1024

typedef void (*threadman_func_t)(void* args);

typedef enum threadman_task_status {
    QUEUED = 0,
    RUNNING = 1,
    FINISHED = 2
} threadman_task_status_t;

typedef struct threadman_task {
    threadman_func_t func;
    void *args;
    threadman_task_status_t status;
} threadman_task_t;

typedef struct threadman_pool {
    threadman_task_t *tasks[MAX_TASKS];
    int task_count;
    pthread_t threads[MAX_THREADS];
    int thread_count;
    int pending_tasks_count;
    bool terminate_workers;
    pthread_cond_t worker_cond;
    pthread_cond_t pool_cond;
    pthread_mutex_t pool_mutex;
} threadman_pool_t;

threadman_pool_t *threadman_pool_create(int thread_count);

bool threadman_submit_task(threadman_pool_t *pool, threadman_func_t func, void* args);

void threadman_wait(threadman_pool_t *pool);

void threadman_pool_free(threadman_pool_t *pool);
