#include "my_allocation.h"

static struct block_meta *list_head = NULL;
static struct block_meta *last_block = NULL; 

allocation_strategy current_strat = 
#ifdef STRATEGY
    (allocation_strategy)STRATEGY;  
#else
    FIRST_FIT; 
#endif

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
            return bestFit(size,list_head);
        case NEXT_FIT:
            return nextFit(size,list_head,last_block);
        default:
            return NULL;
    }
}

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
    // block behind
    if(next){
        if (next->free) {
            LOG("Merging with free block before at %p (size: %zu) with user addr at %p\n", next, next->size,next+1);
            next->size += block->size;
            remove_from_list(block);
            if (current_strat == NEXT_FIT && last_block==block) {
                last_block = next;
            }
            block = next;
        }
    }
    struct block_meta *prev = block->prev;
    //block before
    if(prev){
        if (prev->free) {
            LOG("Merging with free block behind at %p (size: %zu) with user addr at %p\n", prev, prev->size,prev+1);
            block->size += prev->size;
            remove_from_list(prev);
            if (current_strat == NEXT_FIT && last_block == prev) {
                last_block = block;
            }
        }
    }
}

void split_block(struct block_meta *block, size_t size) {
    struct block_meta *new_block = (struct block_meta*)((char*)block + size);
    new_block->size = block->size - size;
    new_block->free = 1;
    block->size = size;
    new_block->prev = block->prev;
    if (new_block->prev){
        new_block->prev->next = new_block;
    }
    block->prev = new_block;
    new_block->next = block;
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
    if (size == 0) return NULL;
    size_t aligned_size = ALIGN(size);
    size_t total_size = META_SIZE + aligned_size;

    struct block_meta *block = find_free_block(total_size);
    // no block available
    if (!block) { 
        LOG("Requesting more heap\n");
        block = request_space(total_size);
        if (!block){
            errno = ENOMEM; // not enough mem
            return NULL;
        }
        add_to_list(block);
    } else { // foudn available block
        LOG("Found free block at %p (size: %zu) with user addr at %p\n", block, block->size,block +1);
        block->free = 0;
        if (block->size - total_size >= META_SIZE + ALIGN_SIZE) {
            split_block(block, total_size);
        }
    }
    if (current_strat == NEXT_FIT) {
        last_block = block;
    }
    LOG("Allocated block at %p (size: %zu) with user addr at %p\n", block, block->size,block +1);
    return(block + 1);
}

void free(void *ptr) {
    if (!ptr) return;
    struct block_meta *block = (struct block_meta*)ptr - 1;
    LOG("free(%p), block size: (%zu) and beginning at %p \n", ptr, block->size, block);
    if(block->free == 1) return;
    block->free = 1;
    LOG("Freed block %p  with user addr at %p\n" ,block, block + 1);
    merge_blocks(block);
}

void *calloc(size_t nelem, size_t elsize) {
    size_t size;
    if (nelem == 0 || elsize == 0) return NULL;
    size = nelem * elsize;
    if (elsize != 0 && size / elsize != nelem) {
        errno = ENOMEM; // int overflow
        return NULL;
    }
    void *ptr = malloc(size);
    // zero out data
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
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    struct block_meta *block = (struct block_meta*)ptr - 1;
    size_t new_aligned_size = ALIGN(size);
    size_t new_total_size = META_SIZE + new_aligned_size;

    if (block->free) return NULL;

    // check if shrinking
    if (block->size >= new_total_size) {
        LOG("shrinking block(%p, %zu)\n",ptr, size);
        if (block->size - new_total_size >= META_SIZE + ALIGN_SIZE) {
            split_block(block, new_total_size);
        }
        return ptr;
    }

    // check if merging ahead is sufficient
    struct block_meta *prev = block->prev;
    if (prev && prev->free) {
        size_t combined_size = block->size + prev->size;
        if (combined_size >= new_total_size) {
            LOG("merging block(%p, %zu) with prev(%p,%zu)\n",ptr, size,prev + 1,prev->size);
            block->size += prev->size;
            remove_from_list(prev);
            if (block->size - new_total_size >= META_SIZE + ALIGN_SIZE) {
                split_block(block, new_total_size);
            }
            return ptr;
        }
    }

    // allocate new block
    LOG("Trying to move the block \n");
    void *new_ptr = malloc(size);
    if (!new_ptr) {
        // coudnt so try last possibl. of merging with block ahead and before
        struct block_meta *next = block->next;
        size_t combined_size = block->size;

        if (next && next->free) combined_size += next->size;
        if (prev && prev->free) combined_size += prev->size;

        if (combined_size >= new_total_size) {
            LOG("merging block(%p, %zu) with prev(%p,%zu)\n and next(%p,%zu)",ptr, size,prev + 1,prev->size,next+1,next->size);
            if (prev && prev->free) {
                prev->size += block->size;
                remove_from_list(block);
                block = prev; 
            }
            if (next && next->free) {
                block->size += next->size;
                remove_from_list(next);
            }
            void *merged_user_ptr = block + 1;
            memmove(merged_user_ptr, ptr, block->size - META_SIZE - (prev ? prev->size : 0));
            block->free = 0;
            if (block->size - new_total_size >= META_SIZE + ALIGN_SIZE) {
                split_block(block, new_total_size);
            }

            return merged_user_ptr;
        } else {
            // no space even after merging
            return NULL;
        }
    }
    // copy into new block
    size_t old_user_size = block->size - META_SIZE;
    size_t copy_size = (size < old_user_size) ? size : old_user_size;
    memcpy(new_ptr, ptr, copy_size);
    free(ptr);

    return new_ptr;
}
