#include "my_allocation.h"

size_t align(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

void add_to_free_list(struct block_meta *block) {
    block->next_free = free_list_head;
    block->prev_free = NULL;
    if (free_list_head) free_list_head->prev_free = block;
    free_list_head = block;
}

void remove_from_free_list(struct block_meta *block) {
    if (block->prev_free) block->prev_free->next_free = block->next_free;
    else free_list_head = block->next_free;
    if (block->next_free) block->next_free->prev_free = block->prev_free;
}

struct block_meta *find_free_block(size_t size) {
    #if STRATEGY == FIRST_FIT
        return firstFit(size);
    //#elif STRATEGY == BEST_FIT
        // Best Fit implementation
    //#elif STRATEGY == NEXT_FIT
        // Next Fit implementation
    #endif
}

struct block_meta *request_space(size_t size) {
    struct block_meta *block = sbrk(0);
    if (sbrk(size) == (void*)-1) return NULL;
    block->size = size;
    block->free = 0;
    return block;
}

void merge_blocks(struct block_meta *block) {
    // Merge with next block
    struct block_meta *next = (struct block_meta*)((char*)block + block->size);
    if (next->free) {
        block->size += next->size;
        remove_from_free_list(next);
    }
    // Merge with previous block
    struct block_meta *prev = (struct block_meta*)((char*)block - block->prev_size);
    if (prev->free) {
        prev->size += block->size;
        remove_from_free_list(block);
        block = prev;
    }
    add_to_free_list(block);
}

void split_block(struct block_meta *block, size_t size) {
    struct block_meta *new_block = (struct block_meta*)((char*)block + size);
    new_block->size = block->size - size;
    new_block->prev_size = size;
    new_block->free = 1;
    block->size = size;
    add_to_free_list(new_block);
}

void *malloc(size_t size) {
    if (size == 0) return NULL;
    size_t aligned_size = align(size);
    size_t total_size = sizeof(struct block_meta) + aligned_size;

    struct block_meta *block = find_free_block(total_size);
    if (!block) {
        block = request_space(total_size);
        if (!block) return NULL;
    } else {
        remove_from_free_list(block);
        if (block->size - total_size >= sizeof(struct block_meta) + ALIGNMENT) {
            split_block(block, total_size);
        }
    }
    block->free = 0;
    return (void*)(block + 1);
}

void free(void *ptr) {
    if (!ptr) return;
    struct block_meta *block = (struct block_meta*)ptr - 1;
    block->free = 1;
    merge_blocks(block);
    add_to_free_list(block);
}