#include "strategies.h"


struct block_meta *firstFit(size_t size,struct block_meta* list_head) {
    struct block_meta *current = list_head;
    while (current) {
        if (current->size >= size && current->free) return current;
        current = current->next;
    }
    return NULL;
}