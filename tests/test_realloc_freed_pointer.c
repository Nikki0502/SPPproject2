#include "my_allocation.h"
#include <assert.h>

void test_realloc_freed_pointer() {
    void *ptr = malloc(16);
    assert(ptr != NULL && "Initial malloc failed");
    free(ptr);
    void *result = realloc(ptr, 32);
    assert(result == NULL && "realloc on freed pointer should return NULL");
    printf("test_realloc_freed_pointer passed!\n");
}

int main(){
    test_realloc_freed_pointer();
    return 0;
}
