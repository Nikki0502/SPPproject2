#include "my_allocation.h"
#include <assert.h>

void test_reuse_of_freed_block() {
    void *p1 = malloc(64);
    assert(p1 != NULL && "Allocation failed for p1");
    free(p1);
    void *p2 = malloc(64);
    assert(p2 == p1 && "Did not reuse freed block");
    free(p2);
    printf("test_reuse_of_freed_block passed!\n");
}

int main(){
    test_reuse_of_freed_block();
    return 0;
}