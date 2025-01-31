#include "my_allocation.h"
#include <assert.h>

void test_realloc_size_zero() {
    void *ptr = malloc(32);
    assert(ptr != NULL && "Initial malloc failed");
    void *result = realloc(ptr, 0);
    assert(result == NULL && "realloc(ptr, 0) should return NULL");
    printf("test_realloc_size_zero passed!\n");
}

int main(){
    test_realloc_size_zero();
    return 0;
}