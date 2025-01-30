#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct block_meta {
    size_t size;
    int free; // 1 free 0 not 
    struct block_meta *next_free;
    struct block_meta *prev_free;
};

#define META_SIZE sizeof(struct block_meta)

static struct block_meta *free_list_head = NULL;
//static struct block_meta *last_block = NULL; // for first fit later

typedef enum {
    FIRST_FIT,
    BEST_FIT,
    NEXT_FIT
} allocation_strategy;



struct block_meta* bestFit(size_t size);
struct block_meta* firstFit(size_t size);
struct block_meta* nextFit(size_t size);