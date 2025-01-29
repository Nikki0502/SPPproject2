# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC
STRATEGIES = firstfit # bestfit nextfit
TESTS = test_basic

# Default target (build everything)
all: libs tests

# --- Shared Libraries for Strategies ---
libs: $(foreach strat,$(STRATEGIES),libmyalloc_$(strat).so)

libmyalloc_%.so: my_allocation.c strategies.c
	$(CC) $(CFLAGS) -DSTRATEGY=$(shell echo $* | tr a-z A-Z) -shared $^ -o $@

# --- Test Executables ---
tests: $(foreach test,$(TESTS),tests/$(test))

# Compile test programs
tests/%: tests/%.c
	$(CC) $(CFLAGS) $< -o $@

# --- Run Tests ---
test: $(foreach strat,$(STRATEGIES),test_$(strat))

test_%: libmyalloc_%.so tests/$(TESTS)
	@echo "\n=== Testing $* strategy ==="
	@for test in $(TESTS); do \
		echo "\nRunning $$test:"; \
		chmod +x tests/$$test; \
		LD_PRELOAD=./libmyalloc_$*.so ./tests/$$test || exit 1; \
	done

# Cleanup
clean:
	rm -f *.so tests/$(TESTS)

.PHONY: all libs tests test clean