#include "my_allocation.h"
#include <assert.h>

void test_realloc_merge_adjacent() {
    void *A = malloc(16);
    void *B = malloc(16);
    assert(A && B && "Initial allocations failed");

    free(B);

    void *new_A = realloc(A, 32);
    assert(new_A != NULL && "Realloc with merge failed");
    assert(new_A == A && "Block should expand into adjacent free space");

    struct block_meta *merged_block = (struct block_meta*)new_A - 1;
    assert(merged_block->size >= 32 + META_SIZE && "Merged block size incorrect");

    free(new_A);
    printf("test_realloc_merge_adjacent passed!\n");
}

int main(){
    test_realloc_merge_adjacent();
    return 0;
}