#include "my_allocation.h"

#define ALIGN_SIZE 8
#define ALIGN(size) (((size) + (ALIGN_SIZE-1)) & ~(ALIGN_SIZE-1))

allocation_strategy current_strat = FIRST_FIT;

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
    block->next_free = NULL;
    block->prev_free = NULL;
    block->free = 0;
}

struct block_meta *find_free_block(size_t size) {
    switch(current_strat) {
        case FIRST_FIT:
            return firstFit(size);
        case BEST_FIT:
            return bestFit(size);
        case NEXT_FIT:
            return nextFit(size);
        default:
            return NULL;
    }
}
// total size
struct block_meta *request_space(size_t size) {
    struct block_meta *block = sbrk(0);
    void *request = sbrk(size);
    if (request == (void*)-1) return NULL;
    
    block->size = size;
    block->free = 0;
    block->next_free = NULL;
    block->prev_free = NULL;
    return block;
}

/*
void merge_blocks(struct block_meta *block) {
    struct block_meta *next = (struct block_meta*)((char*)block + block->size);
    if (next->free) {
        block->size += next->size;
        remove_from_free_list(next);
    }
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
void print_heap() {
    struct block_meta *current = free_list_head;
    printf("Free List:\n");
    while (current) {
        printf("  [%p] size=%zu, free=%d\n", (void*)current, current->size, current->free);
        current = current->next_free;
    }
}
*/

void *malloc(size_t size) {
    if (size == 0) return NULL;
    size_t aligned_size = ALIGN(size);
    size_t total_size = META_SIZE + aligned_size;

    struct block_meta *block = find_free_block(total_size);
    if (!block) { // entweder erster block oder kein platz mehr
        block = request_space(total_size);
        if (!block) return NULL;
    } else { // passenden block gefunden
        remove_from_free_list(block);
        /* TODO
        if (block->size - total_size >= META_SIZE + ALIGN_SIZE) {
            split_block(block, total_size);
        }
        */
    }
    return(block + 1);
}

void free(void *ptr) {
    if (!ptr) return;
    struct block_meta *block = (struct block_meta*)ptr - 1;
    if(block->free == 1){
        // ist schon free TODO
    }
    block->free = 1;
    //merge_blocks(block); TODO
    add_to_free_list(block);
}

