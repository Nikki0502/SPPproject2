#include "my_allocation.h"
#include <assert.h>

void test_calloc_initialization() {
    size_t nelem = 10;
    size_t elsize = sizeof(int);
    size_t total_size = nelem * elsize;

    // Test normal allocation
    int *arr = (int *)calloc(nelem, elsize);
    assert(arr != NULL && "calloc failed to allocate memory");

    // Check every byte in the user-visible memory is zero
    unsigned char *byte_ptr = (unsigned char *)arr;
    for (size_t i = 0; i < total_size; i++) {
        assert(byte_ptr[i] == 0 && "calloc did not zero-initialize memory");
    }

    free(arr);

    printf("test_calloc_initialization passed!\n");
}

int main(){
    test_calloc_initialization();
    return 0;
}