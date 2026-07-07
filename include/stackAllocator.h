#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_MEMORY_BUFFER_SIZE ((size_t) 4096 * 10)

typedef struct {
    void *currentPtr; 
    void *previousPtr;
    size_t bytesOccupying; // block size + padding.
} Pointer;

typedef struct {
    size_t bufferOffset;
    void *ptrToVirtualAddressSpace;
    Pointer *lastPtr;
} MemoryBuffer;

bool initMemoryBuffer(MemoryBuffer *buffer);

Pointer makePointer(void *currentPtr, void *previousPtr, size_t bytesOccupying);

#endif
