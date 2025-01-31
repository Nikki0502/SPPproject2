#include "strategies.h"

// Enable debug logs with -DDEBUG during compilation
#ifdef DEBUG
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...)
#endif

#define ALIGN_SIZE 8
#define ALIGN(size) (((size) + (ALIGN_SIZE-1)) & ~(ALIGN_SIZE-1))

#define META_SIZE sizeof(struct block_meta) //32 btw


void add_to_list(struct block_meta *block);
void remove_from_list(struct block_meta *block);
struct block_meta *find_free_block(size_t size);
struct block_meta *request_space(size_t size);
void split_block(struct block_meta *block, size_t size);
void merge_blocks(struct block_meta *block);
void print_heap();
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nelem, size_t elsize);
void *realloc(void *ptr, size_t size);