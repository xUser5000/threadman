#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<stdio.h>

#include "threadman.h"

void *worker_func(void *arg) {
    threadman_pool_t *pool = (threadman_pool_t *) arg;
    while (true) {
        pthread_mutex_lock(&pool->pool_mutex);
        threadman_task_t *task = NULL;
        for (int i = 0; i < pool->task_count; i++) {
            if (pool->tasks[i]->status == QUEUED) {
                task = pool->tasks[i];
                break;
            }
        }
        if (task == NULL) {
            pthread_cond_wait(&pool->worker_cond, &pool->pool_mutex);
            pthread_mutex_unlock(&pool->pool_mutex);
            continue;
        }
        task->status = RUNNING;
        pthread_mutex_unlock(&pool->pool_mutex);

        task->func(task->args);

        pthread_mutex_lock(&pool->pool_mutex);
        task->status = FINISHED;
        pool->pending_tasks_count--;
        if (pool->pending_tasks_count == 0) {
            pthread_cond_signal(&pool->pool_cond);
        }
        pthread_mutex_unlock(&pool->pool_mutex);
    }
}

threadman_pool_t *threadman_pool_create(int thread_count) {
    threadman_pool_t *pool = (threadman_pool_t *) malloc(sizeof(threadman_pool_t));

    bool failed = false;
    failed |= (pthread_cond_init(&pool->worker_cond, NULL) != 0);
    failed |= (pthread_cond_init(&pool->pool_cond, NULL) != 0);
    failed |= (pthread_mutex_init(&pool->pool_mutex, NULL) != 0);
    if (failed) {
        perror("failed to initialize thread pool\n");
    }

    pool->task_count = 0;
    pool->pending_tasks_count = 0;
    pool->thread_count = thread_count;
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&pool->threads[i], NULL, worker_func, (void *) pool);
    }

    return pool;
}

bool threadman_submit_task(threadman_pool_t *pool, threadman_func_t func, void* args) {
    pthread_mutex_lock(&pool->pool_mutex);

    if (pool->task_count == MAX_TASKS) {
        pthread_mutex_unlock(&pool->pool_mutex);
        return false;
    }

    threadman_task_t *task = (threadman_task_t *) malloc(sizeof(threadman_task_t));
    task->func = func;
    task->args = args;
    task->status = QUEUED;
    
    pool->tasks[pool->task_count] = task;
    pool->task_count++;
    pool->pending_tasks_count++;

    pthread_mutex_unlock(&pool->pool_mutex);
    
    pthread_cond_signal(&pool->worker_cond);
    return true;
}

void threadman_wait(threadman_pool_t *pool) {
    pthread_mutex_lock(&pool->pool_mutex);
    if (pool->pending_tasks_count == 0) {
        pthread_mutex_unlock(&pool->pool_mutex);
        return;
    }
    pthread_cond_wait(&pool->pool_cond, &pool->pool_mutex);
    pthread_mutex_unlock(&pool->pool_mutex);
}

void threadman_pool_free(threadman_pool_t *pool) {

}