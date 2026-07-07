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

/*
 * Will make static after testing phase is over.
 */
Pointer makePointer(void *currentPtr, void *previousPtr, size_t bytesOccupying);

/*
 * Will make static after testing phase is over.
 */
bool validateMemoryBufferInit(MemoryBuffer *buffer);

/*
 * Will make static after testing phase is over.
 */
size_t getAlignmentPadding(MemoryBuffer *buffer, size_t alignment);

/*
 * Will make static after testing phase is over.
 */
bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount);

/*
 * Will make static after testing phase is over.
 */
bool validateParamsOfSalloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment);

/*
 * Will make static after testing phase is over.
 */
bool pushPointer(MemoryBuffer *buffer, Pointer pointer);

// void *salloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment);

#endif
