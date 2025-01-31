#include "my_allocation.h"
#include <assert.h>

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

int main(){
    test_realloc_merge_adjacent();
    return 0;
}