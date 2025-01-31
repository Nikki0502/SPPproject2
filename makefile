CC = gcc
CFLAGS = -Wall -Wextra -fPIC -g #-DDEBUG
LDFLAGS = -shared
STRAT ?= FIRST_FIT

.PHONY: all clean test

all: liballoc.so test_prog

liballoc.so: my_allocation.c strategies.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

test_prog: test.c
	$(CC) $(CFLAGS) $< -o $@

test: liballoc.so test_prog
	@echo "Testing Allocation..."
	@LD_PRELOAD=./liballoc.so ./test_prog


clean:
	rm -f *.so test_prog