#include "../include/memory.h"

#include <stdio.h>
#include <stdlib.h>

#define EXIT_IF_NULL(ptr)                       \
    if((ptr) == NULL) {                         \
        perror(__FILE__);                       \
        exit(EXIT_FAILURE);                     \
    }


typedef struct _allocated_block {
    void* ref;
    struct _allocated_block* next;
} allocated_block_t;

static allocated_block_t* head = NULL;

void* allocate(size_t size) {
    void* ptr = calloc(1, size);
    EXIT_IF_NULL(ptr);

    allocated_block_t* const block = (allocated_block_t*)malloc(sizeof(allocated_block_t));
    EXIT_IF_NULL(block);

    block->ref = ptr;
    block->next = head;

    head = block;

    return ptr;
}

#undef EXIT_IF_NULL

void free_all() {
    allocated_block_t* it = head;
    allocated_block_t* tmp;

    while(it != NULL) {
        tmp = it;
        it = it->next;

        free(tmp->ref);
        free(tmp);
    }
}
