#define _GNU_SOURCE

#include "stackAllocator.h"
#include <stddef.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

bool initMemoryBuffer(MemoryBuffer *buffer) {
    buffer -> bufferOffset = 0;
    buffer -> sizeOfTracker = 0;
    buffer -> ptrToVirtualAddressSpace = (void*) mmap(NULL, MAX_MEMORY_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED) {
        fprintf(stderr, "Error: Failed to initialise memory buffer as mmap failed.\n");
        buffer -> ptrToVirtualAddressSpace == NULL;
        return false;
    }

    return true;
}

static bool validateMemoryBufferInit(MemoryBuffer *buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "Error: Validation of the initialisation of memory buffer failed -- buffer == NULL.\n");
        return false;
    }

    if (buffer -> bufferOffset != 0) {
        fprintf(stderr, "Error: Validation of the initialisation of memory buffer failed -- bufferOffset != 0.\n");
        return false;
    }

    if (buffer -> sizeOfTracker != 0) {
        fprintf(stderr, "Error: Validation of the initialisation of memory buffer failed -- sizeOfTracker != 0.\n");
        return false; 
    } 

    if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED || buffer -> ptrToVirtualAddressSpace == NULL) {
        fprintf(stderr, "error: validation of the initialisation of memory buffer failed -- ptrtovirtualaddressspace failed to map correctly.\n");
        return false;
    }

    return true;
}

static bool validateMemoryBuffer(MemoryBuffer *buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "Error: Failed to validate memory buffer as buffer == NULL.\n");
        return false;
    } 
    if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED || buffer -> ptrToVirtualAddressSpace == NULL) {
        fprintf(stderr, "Error: Validation of memory buffer failed -- ptrToVirtualAddressSpace is invalid.\n");
        return false;
    }

    return true;
}

static bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount) {
    if (offsetAmount > MAX_MEMORY_BUFFER_SIZE) {
        fprintf(stderr, "Error: Failed to increment buffer due to buffer overflow.\n");
        return false;
    } 

    if (buffer -> bufferOffset >= MAX_MEMORY_BUFFER_SIZE) {
        fprintf(stderr, "Error: Failed to increment buffer due to buffer overflow.\n");
        return false;
    }

    size_t remainder = MAX_MEMORY_BUFFER_SIZE - buffer -> bufferOffset;

    if (offsetAmount > remainder) {
        fprintf(stderr, "Error: Failed to call increment buffer due to buffer overflow.\n");
        return false;
    }

    buffer -> bufferOffset += offsetAmount;
    return true;
}

static bool addAllocationAmount(MemoryBuffer *buffer, size_t allocationAmount) {
    if (allocationAmount > MAX_MEMORY_BUFFER_SIZE - buffer -> bufferOffset) {
        fprintf(stderr, "Error: Failed to add allocation amount due to buffer overflow.\n");

        return false;
    } 
    
    if (MAX_ALLOCATIONS == 0) {
        fprintf(stderr, "Error: Failed to getAllocationAmount as MAX_ALLOCATIONS == 0.\n");
        return false;
    }

    if (buffer -> sizeOfTracker > MAX_ALLOCATIONS - 1) {
        fprintf(stderr, "Error: Failed to add alloaction due to maximum allocation amount being exceeded.\n");
        return false;
    }
    
    buffer -> allocationSizeTracker[buffer -> sizeOfTracker] = allocationAmount;
    buffer -> sizeOfTracker += 1; 
    return true;
}

static int getAlignmentPadding(MemoryBuffer *buffer, size_t alignment) {
    if (!validateMemoryBuffer(buffer)) {
        fprintf(stderr, "Error: Failed to call performBufferAlignment as buffer failed validation.\n");
        return -1;
    }

    if (alignment == 0) {
        fprintf(stderr, "Error: Failed to call performBufferAlignment due to division by zero error -- alignment cannot be zero.\n");
        return -1;
    }

    size_t address = (size_t) buffer -> ptrToVirtualAddressSpace + buffer -> bufferOffset;
    
    if (address % alignment == 0) {
        return 0;
    }

    size_t alignmentPadding = alignment - (address % alignment); 
    
    return alignmentPadding;
}

static bool validateParamsOfSalloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment) {
    if (!validateMemoryBuffer(buffer)) {
        fprintf(stderr, "Error: Failed to call salloc as buffer failed validation check.\n");
        return false;
    }

    if (alignment == 0) {
        fprintf(stderr, "Error: Failed to call salloc as alignment cannot be zero.\n");
        return false;
    }

    if (blockSize == 0) {
        fprintf(stderr, "Error: Failed to call salloc as blockSize cannot be zero.\n");
        return false;
    }

    return true;
}

void *salloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment) {
    if (!validateParamsOfSalloc(buffer, blockSize, alignment)) {
        fprintf(stderr, "Error: Failed to call salloc as validation of params failed.\n");
        return NULL;
    }
    
    size_t alignmentPadding = getAlignmentPadding(buffer, alignment);
    char *ptr = (char*) buffer -> ptrToVirtualAddressSpace;
    size_t totalAllocationSize = alignmentPadding + blockSize;
    ptr += buffer -> bufferOffset + alignmentPadding;

    if (!incrementBufferOffset(buffer, totalAllocationSize)) {
        fprintf(stderr, "Error: Failed to call salloc due to buffer overflow.\n");
        return NULL;
    }

    if (!addAllocationAmount(buffer, totalAllocationSize)) {
        fprintf(stderr, "Error: Failed to call salloc due to buffer overflow.\n");
        return NULL;
    }

    return (void*) ptr;
}

static bool validateParamsOfPopAllocation(MemoryBuffer *buffer) {
    if (!validateMemoryBuffer(buffer)) {
        fprintf(stderr, "Error: Failed to call psalloc as buffer is invalid.\n");
        return false;
    } 
    
    if (buffer -> sizeOfTracker == 0) {
        fprintf(stderr, "Error: Failed to call psalloc as there is no allocation to pop.\n");
        return false;
    }

    if (buffer -> allocationSizeTracker[buffer -> sizeOfTracker - 1] == 0) {
        fprintf(stderr, "Error: Failed to call psalloc as you can not free memory which has a size of zero.\n");
        return false;
    }

    return true;
}

static bool handlePopAllocation(MemoryBuffer *buffer) {
    /*
     * Pretty pointless check; size_t types cannot product negative results.
     * Refactoring needed.
     */
    if (buffer -> bufferOffset - (buffer -> allocationSizeTracker[buffer -> sizeOfTracker - 1]) < 0) {
        fprintf(stderr, "Error: Failed to decrement bufferOffset due to negative value.\n");
        return false;
    }
    
    buffer -> bufferOffset -= buffer -> allocationSizeTracker[buffer -> sizeOfTracker - 1];
    buffer -> allocationSizeTracker[buffer -> sizeOfTracker - 1] = 0;
    buffer -> sizeOfTracker -= 1;
    return true;
}

bool psalloc(MemoryBuffer *buffer) {
    if (!validateParamsOfPopAllocation(buffer)) {
        fprintf(stderr, "Error: Failed to call psalloc as params failed a validation test.\n");
        return false;
    } 
    
    if (!handlePopAllocation(buffer)) {
        fprintf(stderr, "Error: Failed to call psalloc as handlePopAllocation failed.\n");
        return false;
    }

    return true;
}

bool dsalloc(MemoryBuffer *buffer) {
    if (!validateMemoryBuffer(buffer)) {
        fprintf(stderr, "Error: Failed to call dsalloc as memory buffer failed validation.\n");
        return false;
    }

    munmap(buffer -> ptrToVirtualAddressSpace, MAX_MEMORY_BUFFER_SIZE);
    buffer -> ptrToVirtualAddressSpace = NULL;

    for (size_t i = 0; i < buffer -> sizeOfTracker; i++) {
        buffer -> allocationSizeTracker[i] = 0;
    }

    buffer -> sizeOfTracker = 0;
    buffer -> bufferOffset = 0;

    return true;
}

bool rsalloc(MemoryBuffer *buffer) {
    if (!validateMemoryBuffer(buffer)) {
        fprintf(stderr, "Error: Failed to call dsalloc as memory buffer failed validation.\n");
        return false;
    }
    
    for (size_t i = 0; i < buffer -> sizeOfTracker; i++) {
        buffer -> allocationSizeTracker[i] = 0;
    }

    buffer -> sizeOfTracker = 0;
    buffer -> bufferOffset = 0;

    return true;
}

