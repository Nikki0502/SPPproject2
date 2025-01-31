#include "my_allocation.h"
#include <assert.h>

void test_basic_allocation() {
    void *p1 = malloc(16);
    assert(p1 != NULL && "Allocation failed for p1");
    void *p2 = malloc(32);
    assert(p2 != NULL && "Allocation failed for p2");
    free(p1);
    free(p2);
    printf("test_basic_allocation passed!\n");
}

void test_reuse_of_freed_block() {
    void *p1 = malloc(64);
    assert(p1 != NULL && "Allocation failed for p1");
    free(p1);
    void *p2 = malloc(64);
    assert(p2 == p1 && "Did not reuse freed block");
    free(p2);
    printf("test_reuse_of_freed_block passed!\n");
}

void test_merging_of_freed_blocks() {
    void *p1 = malloc(16);
    void *p2 = malloc(16);
    void *p3 = malloc(16);
    assert(p1 && p2 && p3 && "Initial allocations failed");
    free(p3);
    free(p1);
    free(p2);
    void *p4 = malloc(112);
    assert(p4 != NULL && "Merging failed or heap full");
    free(p4);
    printf("test_merging_of_freed_blocks passed!\n");
}

void test_splitting_of_block() {
    void *p1 = malloc(64);
    assert(p1 != NULL && "Allocation failed for p1");
    free(p1);
    void *p2 = malloc(16);
    void *p3 = malloc(16);
    assert(p2 && p3 && "Split allocation failed");
    free(p2);
    free(p3);
    printf("test_splitting_of_block passed!\n");
}

void test_double_free() {
    void *p1 = malloc(16);
    assert(p1 != NULL && "Allocation failed for p1");
    free(p1);
    free(p1); // Should handle gracefully (no crash)
    printf("test_double_free passed!\n");
}

void test_zero_alloc() {
    void *p1 = malloc(0);
    assert(p1 == NULL && "Zero allocation did not return NULL");
    printf("test_zero_alloc passed!\n");
}

int main() {

    test_basic_allocation();
    test_reuse_of_freed_block();
    test_merging_of_freed_blocks();
    test_splitting_of_block();
    test_double_free();
    test_zero_alloc();

    printf("All tests passed!\n");

    return 0;
}