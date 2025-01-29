#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ALIGNMENT 8

struct block_meta {
    size_t size;
    size_t prev_size;
    int free;
    struct block_meta *next_free;
    struct block_meta *prev_free;
};

static struct block_meta *free_list_head = NULL;
//static struct block_meta *last_block = NULL; // for first fit later

struct block_meta* bestFit(size_t size);
struct block_meta* firstFit(size_t size);
struct block_meta* nextFit(size_t size);