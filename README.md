# Threadman

Threadman is a statically-linked threadpool library in C. Under the hood, it makes use of the POSIX API's threads, mutexes, and condition variables.

# Features
- [x] Specify a fixed number of threads that get reused
- [x] Unlimited queue size
- [x] Wait for all in-progress tasks to finish
- [x] Free all resources (including threads)

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
