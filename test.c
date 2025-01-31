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
    assert(p4 == p1 && "Merging failed or heap full");
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

void test_calloc_initialization() {
    size_t nelem = 10;
    size_t elsize = sizeof(int);
    size_t total_size = nelem * elsize;

    // Test normal allocation
    int *arr = (int *)calloc(nelem, elsize);
    assert(arr != NULL && "calloc failed to allocate memory");

    // Check every byte in the user-visible memory is zero
    unsigned char *byte_ptr = (unsigned char *)arr;
    for (size_t i = 0; i < total_size; i++) {
        assert(byte_ptr[i] == 0 && "calloc did not zero-initialize memory");
    }

    free(arr);

    printf("test_calloc_initialization passed!\n");
}

void test_realloc_basic() {
    // Allocate initial block and write data
    int *arr = (int *)malloc(4 * sizeof(int));
    assert(arr != NULL && "Initial malloc failed");
    for (int i = 0; i < 4; i++) arr[i] = i; // [0, 1, 2, 3]

    int *arr2 = (int *)malloc(4 * sizeof(int));
    assert(arr2 != NULL && "Initial malloc failed");

    free(arr2);

    // Expand the block
    int *new_arr = (int *)realloc(arr, 8 * sizeof(int));
    assert(new_arr != NULL && "Realloc failed to expand");
    assert(new_arr == arr && "Block should have expanded in-place");
    for (int i = 0; i < 4; i++) assert(new_arr[i] == i && "Data corrupted after expand");

    // Shrink the block
    new_arr = (int *)realloc(new_arr, 2 * sizeof(int));
    assert(new_arr != NULL && "Realloc failed to shrink");
    assert(new_arr == arr && "Block should stay in-place when shrinking");
    assert(new_arr[0] == 0 && "Data corrupted after shrink");

    free(new_arr);
    printf("test_realloc_basic passed!\n");
}

void test_realloc_null_ptr() {
    // realloc(NULL, size) should behave like malloc
    char *ptr = (char *)realloc(NULL, 64);
    assert(ptr != NULL && "realloc(NULL, size) failed");
    free(ptr);
    printf("test_realloc_null_ptr passed!\n");
}

void test_realloc_size_zero() {
    void *ptr = malloc(32);
    assert(ptr != NULL && "Initial malloc failed");
    void *result = realloc(ptr, 0);
    assert(result == NULL && "realloc(ptr, 0) should return NULL");
    printf("test_realloc_size_zero passed!\n");
}

void test_realloc_merge_adjacent() {
    // Allocate three blocks: A B C
    void *A = malloc(16);
    void *B = malloc(16);
    assert(A && B && "Initial allocations failed");

    free(B); // Free the middle block (B)
    struct block_meta *block_A = (struct block_meta*)A - 1;

    // Realloc A to 32 bytes (requires merging with B)
    void *new_A = realloc(A, 32);
    assert(new_A != NULL && "Realloc with merge failed");
    assert(new_A == A && "Block should expand into adjacent free space");

    // Verify merged size (A + B = 16 + 16 + metadata)
    struct block_meta *merged_block = (struct block_meta*)new_A - 1;
    assert(merged_block->size >= 32 + META_SIZE && "Merged block size incorrect");

    free(new_A);
    printf("test_realloc_merge_adjacent passed!\n");
}

void test_realloc_data_integrity() {
    char *str = (char *)malloc(10);
    assert(str != NULL && "Initial malloc failed");
    strcpy(str, "hello");

    // Realloc to larger size
    char *new_str = (char *)realloc(str, 20);
    assert(new_str != NULL && "Realloc failed");
    assert(strcmp(new_str, "hello") == 0 && "Data corrupted after realloc");

    // Realloc to smaller size (truncate)
    new_str = (char *)realloc(new_str, 5);
    assert(new_str != NULL && "Realloc shrink failed");
    assert(strncmp(new_str, "hello", 5) == 0 && "Data corrupted after shrink");

    free(new_str);
    printf("test_realloc_data_integrity passed!\n");
}

void test_realloc_freed_pointer() {
    void *ptr = malloc(16);
    assert(ptr != NULL && "Initial malloc failed");
    free(ptr);
    void *result = realloc(ptr, 32);
    assert(result == NULL && "realloc on freed pointer should return NULL");
    printf("test_realloc_freed_pointer passed!\n");
}

int main() {

    const allocation_strategy strategies[] = {FIRST_FIT, BEST_FIT, NEXT_FIT};
    const char *strategy_names[] = {"First Fit", "Best Fit", "Next Fit"};

    for (int i = 0; i < 1; i++) {
        current_strat = strategies[i];
        
        test_basic_allocation();
        test_reuse_of_freed_block();
        test_merging_of_freed_blocks();
        test_splitting_of_block();
        test_double_free();
        test_zero_alloc();

        test_calloc_initialization();

        test_realloc_basic();
        test_realloc_null_ptr();
        test_realloc_size_zero();
        test_realloc_merge_adjacent();
        test_realloc_data_integrity();
        test_realloc_freed_pointer();

        printf("All tests passed for %s!\n\n",strategy_names[i]);
    }
   

    return 0;
}