CC = gcc
CFLAGS = -Wall -Wextra -fPIC -g -I.
LDFLAGS = -shared
TEST_DIR = tests
TESTS = $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS = $(TESTS:.c=.bin)
STRATEGY ?= FIRST_FIT

# Convert strategy name to enum value
ifeq ($(STRATEGY), FIRST_FIT)
  STRAT_ENUM = 0
else ifeq ($(STRATEGY), BEST_FIT)
  STRAT_ENUM = 1
else ifeq ($(STRATEGY), NEXT_FIT)
  STRAT_ENUM = 2
endif

.PHONY: all clean test

all: liballoc.so $(TEST_BINS)

# Compile shared library with selected strategy
liballoc.so: my_allocation.c strategies.c
	$(CC) $(CFLAGS) -DSTRATEGY=$(STRAT_ENUM) $^ $(LDFLAGS) -o $@

# Pattern rule for test binaries
$(TEST_DIR)/%.bin: $(TEST_DIR)/%.c liballoc.so
	$(CC) $(CFLAGS) $< -o $@ -L. -lalloc

# Run all tests with preloaded allocator
tests: liballoc.so $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		echo "Running $$test ..."; \
		LD_PRELOAD=./liballoc.so LD_LIBRARY_PATH=. ./$$test || exit 1; \
	done

# Run single specific test
test: liballoc.so
ifndef TEST
	$(error Please specify test with TEST=name (without .c extension))
endif
	@echo "Running single test: $(TEST)"
	@LD_PRELOAD=./liballoc.so LD_LIBRARY_PATH=. ./$(TEST_DIR)/$(TEST).bin


clean:
	rm -f liballoc.so $(TEST_DIR)/*.bin

# Create tests directory if missing
$(shell mkdir -p $(TEST_DIR))