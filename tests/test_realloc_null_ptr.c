#include "my_allocation.h"
#include <assert.h>

void test_realloc_null_ptr() {
    char *ptr = (char *)realloc(NULL, 64);
    assert(ptr != NULL && "realloc(NULL, size) failed");
    free(ptr);
    printf("test_realloc_null_ptr passed!\n");
}

int main(){
    test_realloc_null_ptr();
    return 0;
}