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

struct block_meta *nextFit(size_t size, struct block_meta *list_head, struct block_meta *last_block) {
    if (list_head == NULL) {
        return NULL;
    }

    struct block_meta *current = NULL;

    // Determine the starting block
    if (last_block != NULL) {
        current = last_block->prev;
    } else {
        current = list_head;
    }

    // Wrap around to the beginning if current is NULL
    if (current == NULL) {
        current = list_head;
    }

    struct block_meta *start = current;

    do {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->prev;
        if (current == NULL) {
            current = list_head; // Wrap around
        }
    }while (current != start);

    return NULL; // No suitable block found
}