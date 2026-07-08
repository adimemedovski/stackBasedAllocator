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

Pointer makePointer(void *currentPtr, Pointer *previousPointer, size_t bytesOccupying) {
    Pointer pointer;

    pointer.currentPtr = currentPtr; 
    pointer.previousPtr = previousPointer;
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

bool validateMemoryBuffer(MemoryBuffer *buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "Error: Buffer is not valid. Initialsed poorly.\n");
        return false;
    }

    if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED || buffer -> ptrToVirtualAddressSpace == NULL) {
        fprintf(stderr, "Error: Buffer is not valid as ptrToVirtualAddressSpace is invalid.\n ");
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
    if (!validateMemoryBuffer(buffer)) {
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

bool validateParamsOfSalloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment) {
    if (!validateMemoryBuffer(buffer)) {
        fprintf(stderr, "Error: Cannot call salloc due to memory buffer failing validation check.\n");
        return false;
    } 
    
    if (blockSize == 0) {
        fprintf(stderr, "Error: Cannot call salloc due to block size being 0.\n");
        return false;
    }

    if (alignment == 0) {
        fprintf(stderr, "Error: Cannot call salloc due to alignment being 0.\n");
        return false;
    }

    if (buffer -> bufferOffset + getAlignmentPadding(buffer, alignment) > MAX_MEMORY_BUFFER_SIZE - blockSize) {
        fprintf(stderr, "Error: Cannot call salloc due to buffer overflow.\n");
        return false;
    }

    return true;
}

bool pushPointer(MemoryBuffer *buffer, Pointer *pointer) {
    if (!validateMemoryBuffer(buffer)) {
        fprintf(stderr, "Error: Failed to push pointer as memory buffer failed validation check.\n");
        return false;
    } 

    pointer -> previousPtr = buffer -> lastPtr;
    buffer -> lastPtr = pointer;
    
    return true;
}

bool popPointer(MemoryBuffer *buffer) {
    if (buffer -> lastPtr == (Pointer*) NULL) {
        fprintf(stderr, "Error: Cannot pop pointer from buffer, as buffer has no last ptr.\n");
        return false;
    }

    buffer -> bufferOffset -= buffer -> lastPtr -> bytesOccupying; 
    buffer -> lastPtr = buffer -> lastPtr -> previousPtr;
    
    return true;
}

void *salloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment) {
    if (!validateParamsOfSalloc(buffer, blockSize, alignment)) {
        fprintf(stderr, "Error: Failed to call salloc as validation failed.\n");
        return NULL;
    }

    size_t theAlignment = getAlignmentPadding(buffer, alignment);
    size_t bytesOccupying = blockSize + theAlignment; 


}










