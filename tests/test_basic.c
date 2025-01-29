#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    // Test 1: Simple allocation
    int *ptr = malloc(sizeof(int));
    assert(ptr != NULL);
    *ptr = 42;
    assert(*ptr == 42);
    free(ptr);

    // Test 2: Multiple allocations
    void *p1 = malloc(100);
    void *p2 = malloc(200);
    assert(p1 != NULL && p2 != NULL);
    free(p1);
    free(p2);

    // Test 3: Zero-size allocation
    void *p3 = malloc(0);
    assert(p3 == NULL);

    printf("All Basic tests passed!\n");
    return 0;
}