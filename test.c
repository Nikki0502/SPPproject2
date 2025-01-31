#include "my_allocation.h"
#include <assert.h>

void test_basic_allocation() {
    void *p1 = malloc(16);
    void *p2 = malloc(32);
    free(p1);
    free(p2);
}

void test_reuse_of_freed_block() {
    void *p1 = malloc(64);
    free(p1);
    void *p2 = malloc(64);
    free(p2);
}

void test_merging_of_freed_blocks(){
    void *p1 = malloc(16);
    void *p2 = malloc(16);
    void *p3 = malloc(16);
    free(p3);
    free(p1);
    free(p2);
    void *p4 = malloc(112);
    free(p4);
}

void test_splitting_of_block(){
    void *p1 = malloc(64);
    free(p1);
    void *p2 = malloc(16);
    void *p3 = malloc(16);
    free(p2);
    free(p3);
}

void test_double_free(){
    void *p1 = malloc(16);
    free(p1);
    free(p1);
}

void test_zero_alloc(){
    void *p1 = malloc(0);
    if(p1){
        printf("not null");
    }
}


int main() {
    test_basic_allocation();
    test_reuse_of_freed_block();
    test_merging_of_freed_blocks();
    test_splitting_of_block();
    test_double_free();
    test_zero_alloc();

    return 0;
}
