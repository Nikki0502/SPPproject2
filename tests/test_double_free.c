#include "my_allocation.h"
#include <assert.h>

void test_double_free() {
    void *p1 = malloc(16);
    assert(p1 != NULL && "Allocation failed for p1");
    free(p1);
    free(p1); // Should handle gracefully (no crash)
    printf("test_double_free passed!\n");
}

int main(){
    test_double_free();
    return 0;
}