#include "my_allocation.h"
#include <assert.h>

void test_basic_allocation() {
    void *p1 = malloc(16);
    assert(p1 != NULL && "Allocation failed for p1");
    void *p2 = malloc(32);
    assert(p2 != NULL && "Allocation failed for p2");
    free(p1);
    free(p2);
    printf("test_basic_allocation passed!\n");
}

int main(){
    test_basic_allocation();
    return 0;
}