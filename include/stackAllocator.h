#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    void *thisPtr; 
    void *previousPtr;
    size_t bytesOccupying; // block size + padding.
} Pointer;

typedef struct {
    size_t bufferOffset;
    void *ptrToVirtualAddressSpace;
    Pointer *lastPtr;
} MemoryBuffer;


#endif
