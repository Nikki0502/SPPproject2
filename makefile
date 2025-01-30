CC = gcc
CFLAGS = -Wall -Wextra -fPIC -g -DDEBUG
LDFLAGS = -shared

.PHONY: all clean test

all: liballoc.so test_prog

liballoc.so: my_allocation.c strategies.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

test_prog: test.c
	$(CC) $(CFLAGS) $< -o $@

test: test_first 

test_first: liballoc.so test_prog
	@echo "Testing first-fit strategy..."
	@LD_PRELOAD=./liballoc.so ./test_prog

clean:
	rm -f *.so test_prog