#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

struct block_meta {
    size_t size;
    int free; // 1 free 0 not 
    struct block_meta *next;
    struct block_meta *prev;
};

typedef enum {
    FIRST_FIT = 0,
    BEST_FIT = 1,
    NEXT_FIT = 2
} allocation_strategy;

struct block_meta* bestFit(size_t size, struct block_meta* list_head);
struct block_meta* firstFit(size_t size,struct block_meta* list_head);
struct block_meta* nextFit(size_t size,struct block_meta* list_head,struct block_meta *last_block);