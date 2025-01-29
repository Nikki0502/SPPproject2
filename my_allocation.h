#include "strategies.h"

size_t align(size_t size);
void add_to_free_list(struct block_meta *block);
void remove_from_free_list(struct block_meta *block);
struct block_meta *find_free_block(size_t size);
struct block_meta *request_space(size_t size);
void *malloc(size_t size);
void free(void *ptr);