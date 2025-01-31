#include "my_allocation.h"
#include <assert.h>

void test_realloc_data_integrity() {
    char *str = (char *)malloc(10);
    assert(str != NULL && "Initial malloc failed");
    strcpy(str, "hello");

    // Realloc to larger size
    char *new_str = (char *)realloc(str, 20);
    assert(new_str != NULL && "Realloc failed");
    assert(strcmp(new_str, "hello") == 0 && "Data corrupted after realloc");

    // Realloc to smaller size (truncate)
    new_str = (char *)realloc(new_str, 5);
    assert(new_str != NULL && "Realloc shrink failed");
    assert(strncmp(new_str, "hello", 5) == 0 && "Data corrupted after shrink");

    free(new_str);
    printf("test_realloc_data_integrity passed!\n");
}

int main(){
    test_realloc_data_integrity();
    return 0;
}