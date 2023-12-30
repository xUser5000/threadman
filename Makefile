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
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -c threadman.c -o threadman.o

test: test.c threadman.o
	$(CC) $(DEBUG_FLAGS) -o test test.c threadman.o

.PHONY: valgrind
valgrind: test
	$(VALGRIND) ./test

.PHONY: thread_sanitizer
thread_sanitizer:
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -fsanitize=thread -c threadman.c -o threadman.tsan.o
	$(CC) $(DEBUG_FLAGS) -fsanitize=thread -o test_tsan test.c threadman.tsan.o
	./test_tsan

.PHONY: clean
clean:
	rm -rf threadman.o threadman.tsan.o test test_tsan
