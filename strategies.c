#include "strategies.h"


struct block_meta *firstFit(size_t size,struct block_meta* list_head) {
    struct block_meta *current = list_head;
    while (current) {
        if (current->size >= size && current->free) return current;
        current = current->next;
    }
    return NULL;
}

struct block_meta* bestFit(size_t size, struct block_meta* list_head) {
    struct block_meta *best_block = NULL;
    struct block_meta *current = list_head;

    while (current != NULL) {
        if (current->free && current->size >= size) {
            if (best_block == NULL || current->size < best_block->size) {
                best_block = current;
            }
        }
        current = current->next;
    }
    return best_block;
}