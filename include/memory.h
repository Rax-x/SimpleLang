#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stddef.h>

void* allocate(size_t size);
void free_all();

#define MALLOC(type, size) (type)allocate((size))

#endif
