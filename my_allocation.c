#include "my_allocation.h"

static struct block_meta *list_head = NULL;
//static struct block_meta *last_block = NULL; // for first fit later

allocation_strategy current_strat = FIRST_FIT;

void add_to_list(struct block_meta *block) {
    block->next = list_head;
    block->prev = NULL;
    if (list_head) list_head->prev = block;
    list_head = block;
}

void remove_from_list(struct block_meta *block) {
    if (block->prev) block->prev->next = block->next;
    else list_head = block->next;
    if (block->next) block->next->prev = block->prev;
    block->next = NULL;
    block->prev = NULL;
    block->free = 0;
}

struct block_meta *find_free_block(size_t size) {
    switch(current_strat) {
        case FIRST_FIT:
            return firstFit(size,list_head);
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
    block->next = NULL;
    block->prev = NULL;
    return block;
}


void merge_blocks(struct block_meta *block) {
    struct block_meta *next = block->next;
    if(next){
        if (next->free) {
            LOG("Merging with free block behind at %p (size: %zu) with user addr at %p\n", next, next->size,next+1);
            block->size += next->size;
            remove_from_list(next);
        }
    }
    struct block_meta *prev = block->prev;
    if(prev){
        if (prev->free) {
            LOG("Merging with free block before at %p (size: %zu) with user addr at %p\n", prev, prev->size,prev+1);
            prev->size += block->size;
            remove_from_list(block);
            block = prev;
        }
    }
}

void split_block(struct block_meta *block, size_t size) {
    struct block_meta *new_block = (struct block_meta*)((char*)block + size);
    new_block->size = block->size - size;
    new_block->free = 1;
    block->size = size;
    new_block->next = block->next;
    block->next = new_block;
    new_block->prev = block;
    LOG("Splitt into block at %p (size: %zu) with user addr at %p\n", block, block->size,block +1);
    LOG("And new_block at %p (size: %zu)with user addr at %p\n", new_block , new_block->size, new_block+1);
}

void print_heap() {
    struct block_meta *current = list_head;
    LOG("List:\n");
    while (current) {
        LOG("  [%p] size=%zu, free=%d\n", (void*)current, current->size, current->free);
        current = current->next;
    }
}

void *malloc(size_t size) {
    LOG("malloc(%zu)\n", size);
    // TODO errno setzen size 0
    if (size == 0) return NULL;
    size_t aligned_size = ALIGN(size);
    size_t total_size = META_SIZE + aligned_size;

    struct block_meta *block = find_free_block(total_size);
    if (!block) { // entweder erster block oder kein platz mehr
        LOG("Requesting more heap\n");
        block = request_space(total_size);
        // TODO errno setzen kein space mehr
        if (!block) return NULL;
        add_to_list(block);
    } else { // passenden block gefunden
        LOG("Found free block at %p (size: %zu) with user addr at %p\n", block, block->size,block +1);
        block->free = 0;
        // groß genug um 2 bloecke zu machen
        if (block->size - total_size >= META_SIZE + ALIGN_SIZE) {
            split_block(block, total_size);
        }
    }
    LOG("Allocated block at %p (size: %zu) with user addr at %p\n", block, block->size,block +1);
    return(block + 1);
}

void free(void *ptr) {
    if (!ptr) return;
    struct block_meta *block = (struct block_meta*)ptr - 1;
    LOG("free(%p), block size: (%zu) and beginning at %p \n", ptr, block->size, block);
    if(block->free == 1){
        // ist schon free TODO
        return;
    }
    block->free = 1;
    LOG("Freed block %p  with user addr at %p\n" ,block, block + 1);
    merge_blocks(block);
}

