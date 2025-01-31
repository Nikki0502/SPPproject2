#include "my_allocation.h"
#include <assert.h>

void test_zero_alloc() {
    void *p1 = malloc(0);
    assert(p1 == NULL && "Zero allocation did not return NULL");
    printf("test_zero_alloc passed!\n");
}

int main(){
    test_zero_alloc();
    return 0;
}