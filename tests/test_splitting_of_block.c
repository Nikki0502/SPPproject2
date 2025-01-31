#include "my_allocation.h"
#include <assert.h>

void test_splitting_of_block() {
    void *p1 = malloc(64);
    assert(p1 != NULL && "Allocation failed for p1");
    free(p1);
    void *p2 = malloc(16);
    void *p3 = malloc(16);
    assert(p2 && p3 && "Split allocation failed");
    free(p2);
    free(p3);
    printf("test_splitting_of_block passed!\n");
}

int main(){
    test_splitting_of_block();
    return 0;
}