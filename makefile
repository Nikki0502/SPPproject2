CC = gcc
CFLAGS = -Wall -Wextra -fPIC -g -I. #-DDEBUG
LDFLAGS = -shared
TEST_DIR = tests
TESTS = $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS = $(TESTS:.c=.bin)
STRATEGY = NEXT_FIT # BEST_FIT # NEXT_FIT # FIRST_FIT

.PHONY: all clean test

all: liballoc.so $(TEST_BINS)

# Compile shared library with selected strategy
liballoc.so: my_allocation.c strategies.c
	$(CC) $(CFLAGS) -DSTRATEGY=$(STRATEGY) $^ $(LDFLAGS) -o $@

# Pattern rule for test binaries
$(TEST_DIR)/%.bin: $(TEST_DIR)/%.c liballoc.so
	$(CC) $(CFLAGS) $< -o $@ -L. -lalloc

# Run all tests with preloaded allocator
tests: liballoc.so $(TEST_BINS)
	@echo "Testing strategy: $(STRATEGY)"
	@for test in $(TEST_BINS); do \
		LD_PRELOAD=./liballoc.so LD_LIBRARY_PATH=. ./$$test || exit 1; \
	done


# Run single specific test
test: liballoc.so
ifndef TEST
	$(error Please specify test with TEST=name (without .c extension))
endif
	@echo "Testing strategy: $(STRATEGY)"
	@echo "Running single test: $(TEST)"
	@LD_PRELOAD=./liballoc.so LD_LIBRARY_PATH=. ./$(TEST_DIR)/$(TEST).bin


clean:
	rm -f liballoc.so $(TEST_DIR)/*.bin

# Create tests directory if missing
$(shell mkdir -p $(TEST_DIR))