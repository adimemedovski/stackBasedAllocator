#define _GNU_SOURCE

#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_ALLOCATIONS ((size_t) 1000)
#define MAX_MEMORY_BUFFER_SIZE ((size_t) 4096)

typedef struct {
    size_t bufferOffset;
    size_t allocationSizeTracker[MAX_ALLOCATIONS];
    size_t sizeOfTracker; 
    void *ptrToVirtualAddressSpace;
} MemoryBuffer;

bool initMemoryBuffer(MemoryBuffer *buffer);

bool validateMemoryBufferInit(MemoryBuffer *buffer);

bool validateMemoryBuffer(MemoryBuffer *buffer);

bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount);

/*
 * alocationAmount = blockSize + alignment.
 */
bool addAllocationAmount(MemoryBuffer *buffer, size_t allocationAmount);

int getAlignmentPadding(MemoryBuffer *buffer, size_t alignment);

void *salloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment);

bool psalloc(MemoryBuffer *buffer);

bool dsalloc(MemoryBuffer *buffer);

bool rsalloc(MemoryBuffer *buffer);
