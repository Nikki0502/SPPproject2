#include "my_allocation.h"
#include <assert.h>

void test_realloc_basic() {
    // Allocate initial block and write data
    int *arr = (int *)malloc(4 * sizeof(int));
    assert(arr != NULL && "Initial malloc failed");
    for (int i = 0; i < 4; i++) arr[i] = i; // [0, 1, 2, 3]

    int *arr2 = (int *)malloc(4 * sizeof(int));
    assert(arr2 != NULL && "Initial malloc failed");

    free(arr2);

    // Expand the block
    int *new_arr = (int *)realloc(arr, 8 * sizeof(int));
    assert(new_arr != NULL && "Realloc failed to expand");
    assert(new_arr == arr && "Block should have expanded in-place");
    for (int i = 0; i < 4; i++) assert(new_arr[i] == i && "Data corrupted after expand");

    // Shrink the block
    new_arr = (int *)realloc(new_arr, 2 * sizeof(int));
    assert(new_arr != NULL && "Realloc failed to shrink");
    assert(new_arr == arr && "Block should stay in-place when shrinking");
    assert(new_arr[0] == 0 && "Data corrupted after shrink");

    free(new_arr);
    printf("test_realloc_basic passed!\n");
}

int main(){
    test_realloc_basic();
    return 0;
}