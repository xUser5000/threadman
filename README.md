# Threadman

Threadman is a statically-linked threadpool library in C. Under the hood, it makes use of the POSIX API's threads, mutexes, and condition variables.

# Features
- [x] Specify a fixed number of threads that get reused
- [x] Unlimited queue size
- [x] Wait for all in-progress tasks to finish
- [x] Free all resources (including threads)

# API

```c
#define MAX_THREADS 1024

typedef struct threadman_pool_t threadman_pool_t;

typedef void (*threadman_func_t)(void* args);

/* create a new thread pool with a `thread_count` worker threads */
threadman_pool_t *threadman_pool_create(int thread_count);

/* submit task to the threadpool */
bool threadman_submit_task(threadman_pool_t *pool, threadman_func_t func, void* args);

/* wait for all tasks to finish */
void threadman_wait(threadman_pool_t *pool);

/* deallocate all resources of the threadpool */
void threadman_pool_free(threadman_pool_t *pool);
```



# Example

```c
#include "threadman.h"

void concurrent_func(void* arg) {
    // do some fancy concurrent programming
}

int main(void) {
    int WORKER_THREADS = 12;
    threadman_pool_t *pool = threadman_pool_create(WORKER_THREADS);
    
    int task_count = 1000;
    for (int i = 0; i < task_count; i++) {
        threadman_submit_task(pool, concurrent_func, NULL);
    }
    threadman_wait(pool);
    threadman_pool_free(pool);
    
    return 0;
}

```



# TODO
- [ ] Support lazy creation of threads
- [ ] Reduce locking contention
  - [ ] Look for a method to compute locking contention


# Compiling
- Threadman is only supported on Linux and has been tested on Ubuntu 22.04 LTS.
- To compile Threadman, run `make` in the root directory of the project and you will have a `threadman.o` file that you can link within your project.
- Run `make clean` in the root directory of the project to remove all build/test artifacts.


# Testing
- Install testing dependencies:

  ```bash
  sudo apt update
  sudo apt install valgrind # for memory debugging, memory leak detection, and profiling
  ```

- Run `make test` in the root directory of the project to run tests.
