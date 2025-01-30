#include "my_allocation.h"
#include <assert.h>

void test_basic_allocation() {
    void *p1 = malloc(16);
    void *p2 = malloc(32);
    assert(p1 != NULL);
    assert(p2 != NULL);
    assert(p1 != p2); // Ensure no overlap
    free(p1);
    free(p2);
}

void test_reuse_of_freed_block() {
    void *p1 = malloc(64);
    free(p1);
    void *p2 = malloc(64);
    assert(p1 == p2); // Freed block should be reused
    free(p2);
}

int main() {
    test_basic_allocation();
    test_reuse_of_freed_block();
    printf("All tests passed!\n");
    return 0;
}
//gdb --args env LD_PRELOAD=./liballoc.so ./test_prog   