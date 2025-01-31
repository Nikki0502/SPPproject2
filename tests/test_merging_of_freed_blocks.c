#include "my_allocation.h"
#include <assert.h>

void test_merging_of_freed_blocks() {
    void *p1 = malloc(16);
    void *p2 = malloc(16);
    void *p3 = malloc(16);
    assert(p1 && p2 && p3 && "Initial allocations failed");
    free(p3);
    free(p1);
    free(p2);
    void *p4 = malloc(112);
    assert(p4 == p1 && "Merging failed or heap full");
    free(p4);
    printf("test_merging_of_freed_blocks passed!\n");
}

int main(){
    test_merging_of_freed_blocks();
    return 0;
}