CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic \
	-Wformat=2 -Wno-unused-parameter -Wshadow \
	-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
	-Wredundant-decls -Wnested-externs -Wmissing-include-dirs \
	-Wjump-misses-init -Wlogical-op
DEBUG_FLAGS=-g
LIBS=-pthread

BDIR=build

VALGRIND=valgrind --leak-check=yes

threadman.o: threadman.h threadman.c
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -c -o threadman.o threadman.c

queue.o: queue.c queue.c
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -c queue.c -o queue.o

test:
	make thread_sanitizer
	make valgrind

.PHONY: valgrind
valgrind: test.c threadman.o queue.o
	$(CC) $(DEBUG_FLAGS) -o test test.c threadman.o queue.o
	$(VALGRIND) ./test

.PHONY: thread_sanitizer
thread_sanitizer:
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -fsanitize=thread -c threadman.c -o threadman.tsan.o
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -fsanitize=thread -c queue.c -o queue.tsan.o
	$(CC) $(DEBUG_FLAGS) -fsanitize=thread -o test_tsan test.c threadman.tsan.o queue.tsan.o
	./test_tsan

.PHONY: clean
clean:
	rm -rf threadman.o threadman.tsan.o test test_tsan queue.o queue.tsan.o
