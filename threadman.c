#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
#include<stdio.h>

#include "threadman.h"
#include "queue.h"

typedef struct {
    threadman_func_t func;
    void *args;
} threadman_task_t;

struct threadman_pool_t {
    queue_t *tasks_q;
    pthread_t threads[MAX_THREADS];
    int thread_count;
    int pending_tasks_count;
    bool terminate_workers;
    pthread_cond_t worker_cond;
    pthread_cond_t pool_cond;
    pthread_mutex_t pool_mutex;
};

void *worker_func(void *arg) {
    threadman_pool_t *pool = (threadman_pool_t *) arg;
    while (true) {
        pthread_mutex_lock(&pool->pool_mutex);
        
        if (pool->terminate_workers) {
            pthread_mutex_unlock(&pool->pool_mutex);
            pthread_exit(NULL);
        }

        threadman_task_t *task = queue_pop(pool->tasks_q);
        
        if (task == NULL) {
            pthread_cond_wait(&pool->worker_cond, &pool->pool_mutex);
            pthread_mutex_unlock(&pool->pool_mutex);
            continue;
        }

        pthread_mutex_unlock(&pool->pool_mutex);

        task->func(task->args);
        free(task);

        pthread_mutex_lock(&pool->pool_mutex);
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

    pool->tasks_q = queue_create();
    pool->pending_tasks_count = 0;
    pool->thread_count = thread_count;
    pool->terminate_workers = false;
    
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&pool->threads[i], NULL, worker_func, (void *) pool);
    }

    return pool;
}

bool threadman_submit_task(threadman_pool_t *pool, threadman_func_t func, void* args) {
    pthread_mutex_lock(&pool->pool_mutex);

    threadman_task_t *task = (threadman_task_t *) malloc(sizeof(threadman_task_t));
    task->func = func;
    task->args = args;
    
    queue_push(pool->tasks_q, task);
    pool->pending_tasks_count++;

    pthread_mutex_unlock(&pool->pool_mutex);
    
    pthread_cond_signal(&pool->worker_cond);
    return true;
}

void threadman_wait(threadman_pool_t *pool) {
    pthread_mutex_lock(&pool->pool_mutex);
    while (pool->pending_tasks_count != 0) {
        pthread_cond_wait(&pool->pool_cond, &pool->pool_mutex);
    }
    pthread_mutex_unlock(&pool->pool_mutex);
}

void threadman_pool_free(threadman_pool_t *pool) {
    if (pool == NULL) {
        return;
    }
    
    pthread_mutex_lock(&pool->pool_mutex);
    pool->terminate_workers = true;
    pthread_mutex_unlock(&pool->pool_mutex);
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_cond_signal(&pool->worker_cond);
    }
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    while(!queue_empty(pool->tasks_q)) {
        void *front = queue_pop(pool->tasks_q);
        free(front);
    }
    free(pool->tasks_q);
    free(pool);
}
