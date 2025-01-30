#include <stdio.h>
#include <stdlib.h>

int main() {
    void* ptr=malloc(1);
    free(ptr);
    printf("succ");
    return 0;
}

//gdb --args env LD_PRELOAD=./liballoc.so ./test_prog   