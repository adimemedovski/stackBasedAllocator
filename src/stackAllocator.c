#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "stackAllocator.h"

bool initMemoryBuffer(MemoryBuffer *buffer) {
    buffer -> bufferOffset = 0;
    buffer -> lastPtr = NULL;

    buffer -> ptrToVirtualAddressSpace = (void*) mmap(NULL, MAX_MEMORY_BUFFER_SIZE,
            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);
    
    if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED) {
        fprintf(stderr, "Error: Failed to initialise memory buffer as buffer -> ptrToVirtualAddressSpace == NULL.\n");
        return false; 
    }

    return true;
}

Pointer makePointer(void *currentPtr, void *previousPtr, size_t bytesOccupying) {
    Pointer pointer;

    pointer.currentPtr = currentPtr; 
    pointer.previousPtr = previousPtr;
    pointer.bytesOccupying = bytesOccupying;

    return pointer;
}
