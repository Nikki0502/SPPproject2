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
    if (!block) { 
        LOG("Requesting more heap\n");
        block = request_space(total_size);
        // TODO errno setzen kein space mehr ENOMEN
        if (!block) return NULL;
        add_to_list(block);
    } else { 
        LOG("Found free block at %p (size: %zu) with user addr at %p\n", block, block->size,block +1);
        block->free = 0;
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
        // ist schon free TODO undefined behaivor idk
        return;
    }
    block->free = 1;
    LOG("Freed block %p  with user addr at %p\n" ,block, block + 1);
    merge_blocks(block);
}

void *calloc(size_t nelem, size_t elsize) {
    size_t size;
    if (nelem == 0 || elsize == 0) {
        return NULL;
    }
    size = nelem * elsize;
    if (elsize != 0 && size / elsize != nelem) {
        errno = ENOMEM; // int overflow
        return NULL;
    }
    void *ptr = malloc(size);
    if (ptr) {
        struct block_meta *block = (struct block_meta*)ptr - 1;
        size_t allocated_size = block->size - META_SIZE;
        memset(ptr, 0, allocated_size);
        LOG("Allocated block at %p (size: %zu) with user addr at %p\n", block, block->size,block +1);
    }
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    LOG("realloc(%p, %zu)\n", ptr, size);
    if (!ptr) {
        return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    struct block_meta *block = (struct block_meta*)ptr - 1;
    size_t new_aligned_size = ALIGN(size);
    size_t new_total_size = META_SIZE + new_aligned_size;

    // Check if current block can satisfy the request
    if (block->size >= new_total_size) {
        // Check if we can split the block
        if (block->size - new_total_size >= META_SIZE + ALIGN_SIZE) {
            split_block(block, new_total_size);
        }
        return ptr;
    }

    // Check if merging with next block is possible
    struct block_meta *next = block->next;
    if (next && next->free) {
        size_t combined_size = block->size + next->size;
        if (combined_size >= new_total_size) {
            block->size += next->size;
            remove_from_list(next);
            // Check if split is needed after merge
            if (block->size - new_total_size >= META_SIZE + ALIGN_SIZE) {
                split_block(block, new_total_size);
            }
            return ptr;
        }
    }

    // Allocate new block and copy data
    void *new_ptr = malloc(size);
    if (!new_ptr) {
        struct block_meta *prev = block->prev;
        size_t combined_size = block->size;

        // Calculate total size after merging
        if (prev && prev->free) combined_size += prev->size;
        if (next && next->free) combined_size += next->size;

        if (combined_size >= new_total_size) {
            // Merge previous (if free)
            if (prev && prev->free) {
                prev->size += block->size;
                remove_from_list(block);
                block = prev; // Update block to merged block
            }

            // Merge next (if free)
            if (next && next->free) {
                block->size += next->size;
                remove_from_list(next);
            }

            // Copy data to the start of the merged block
            void *merged_user_ptr = block + 1;
            memmove(merged_user_ptr, ptr, block->size - META_SIZE - (prev ? prev->size : 0));

            // Mark as allocated and split if needed
            block->free = 0;
            if (block->size - new_total_size >= META_SIZE + ALIGN_SIZE) {
                split_block(block, new_total_size);
            }

            return merged_user_ptr;
        } else {
            // No space even after merging
            return NULL;
        }
    }
    size_t old_user_size = block->size - META_SIZE;
    size_t copy_size = (size < old_user_size) ? size : old_user_size;
    memcpy(new_ptr, ptr, copy_size);
    free(ptr);

    return new_ptr;
}
