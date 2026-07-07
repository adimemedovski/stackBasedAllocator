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

bool validateMemoryBufferInit(MemoryBuffer *buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "Error: Buffer is not valid. Initialsed poorly.\n");
        return false;
    }

    if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED || buffer -> ptrToVirtualAddressSpace == NULL) {
        fprintf(stderr, "Error: Buffer is not valid as ptrToVirtualAddressSpace is invalid.\n ");
        return false;
    }
    
    if (buffer -> lastPtr != NULL) {
        fprintf(stderr, "Error: Buffer is not valid as lastPtr is not NULL.\n");
        return false;
    }

    return true;
}

/*
 * Be cautious when using this function, as an alignment being 0
 * can return a value which hides a division by zero error.
 *
 * Ensure to validate the parameter alignment before use.
 *
 * @author Adi Memedovski
 */
size_t getAlignmentPadding(MemoryBuffer *buffer, size_t alignment) {
    size_t address = (size_t) buffer -> ptrToVirtualAddressSpace;
    
    if (alignment == 0) {
        fprintf(stderr, "Error: Alignment padding failed as alignment is 0 -- division by zero error.\n");
        return -1;
    } else if (address % alignment == 0) {
        return 0;
    } else {
        return alignment - (address % alignment);
    }
}

bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount) {
    if (!validateMemoryBufferInit(buffer)) {
        fprintf(stderr, "Error: Failed to increment buffer offset as memory buffer failed validation.\n");
        return false;
    }

    if (buffer -> bufferOffset > MAX_MEMORY_BUFFER_SIZE - offsetAmount) {
        fprintf(stderr, "Error: Cannot increment buffer offset due to buffer overflow.\n");
        return false;
    }

    buffer -> bufferOffset += offsetAmount;
    
    return true;
}

