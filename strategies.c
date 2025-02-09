#include "strategies.h"

// start list head go throug list
struct block_meta *firstFit(size_t size,struct block_meta* list_head) {
    struct block_meta *current = list_head;
    while (current) {
        if (current->size >= size && current->free) return current;
        current = current->next;
    }
    return NULL;
}
// start list head go through and keep smallest fitting block in mind
struct block_meta* bestFit(size_t size, struct block_meta* list_head) {
    struct block_meta *best_block = NULL;
    struct block_meta *current = list_head;

    while (current != NULL) {
        if (current->free && current->size >= size) {
            if(current->size == size){
                best_block = current;
                return best_block;
            }
            if (best_block == NULL || current->size < best_block->size) {
                best_block = current;
            }
        }
        current = current->next;
    }
    return best_block;
}
// start at last block and loop trough list till there again
struct block_meta *nextFit(size_t size, struct block_meta *list_head, struct block_meta *last_block) {
    if (list_head == NULL) {
        return NULL;
    }

    struct block_meta *current = NULL;

    // det strarting block
    if (last_block != NULL) {
        current = last_block->next;
    } else {
        current = list_head;
    }

    // wrap around if necc
    if (current == NULL) {
        current = list_head;
    }

    struct block_meta *start = current;

    // do while so atleast once 
    do {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
        if (current == NULL) {
            current = list_head; // wrap around
        }
    }while (current != start);

    return NULL; 
}