#define _GNU_SOURCE

#include "stackAllocator.h"
#include "unity.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void setUp(void) {

}

void tearDown(void) {

}

void testInitMemoryBuffer(void) {
   
    /*
     * Test One: Testing basic functionality.
     */
    MemoryBuffer buffer;
    
    TEST_ASSERT_TRUE(initMemoryBuffer(&buffer));
    TEST_ASSERT_EQUAL_size_t(0, buffer.bufferOffset);
    TEST_ASSERT_EQUAL_size_t(0, buffer.sizeOfTracker); 
    TEST_ASSERT_TRUE(buffer.ptrToVirtualAddressSpace != NULL || buffer.ptrToVirtualAddressSpace == MAP_FAILED);
 
    buffer.allocationSizeTracker[0] = 3;
    TEST_ASSERT_EQUAL_size_t(3, buffer.allocationSizeTracker[0]);
   
    /*
     * Clean up.
     */
    munmap(buffer.ptrToVirtualAddressSpace, MAX_MEMORY_BUFFER_SIZE);
}

void testSalloc(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    /*
     * Test One: Testing basic functionality.
     */
    size_t *ptrOne = (size_t*) salloc(&buffer, sizeof(size_t) * 3, _Alignof(size_t));

    TEST_ASSERT_TRUE(ptrOne != NULL);
    TEST_ASSERT_EQUAL_size_t(24, buffer.allocationSizeTracker[0]);
    TEST_ASSERT_EQUAL_size_t(24, buffer.bufferOffset);
    
    ptrOne[0] = 3;
    ptrOne[1] = 6;
    ptrOne[2] = 9;

    TEST_ASSERT_EQUAL_size_t(3, ptrOne[0]);
    TEST_ASSERT_EQUAL_size_t(6, ptrOne[1]);
    TEST_ASSERT_EQUAL_size_t(9, ptrOne[2]);
    
    /*
     * Test Two: Testing alignment.
     */
    char *ptrTwo = (char*) salloc(&buffer, sizeof(char), _Alignof(char));
    size_t *ptrThree = (size_t*) salloc(&buffer, sizeof(size_t), _Alignof(size_t));
    TEST_ASSERT_TRUE(ptrTwo != NULL && ptrThree != NULL);
    TEST_ASSERT_EQUAL_size_t(40, buffer.bufferOffset);
    TEST_ASSERT_EQUAL_size_t(15, buffer.allocationSizeTracker[2]);

    /*
     * Clean up.
     */
    munmap(buffer.ptrToVirtualAddressSpace, MAX_MEMORY_BUFFER_SIZE);
}

void testPsalloc(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    /*
     * Test One: Testing basic functionality.
     */
    size_t *ptrOne = (size_t*) salloc(&buffer, sizeof(size_t) * 3, _Alignof(size_t));
    size_t *ptrTwo = (size_t*) salloc(&buffer, sizeof(size_t), _Alignof(size_t));

    TEST_ASSERT_TRUE(ptrOne != NULL && ptrTwo != NULL);
    
    TEST_ASSERT_EQUAL_size_t(8, buffer.allocationSizeTracker[buffer.sizeOfTracker - 1]);
    TEST_ASSERT_EQUAL_size_t(24, buffer.allocationSizeTracker[buffer.sizeOfTracker - 2]);

    TEST_ASSERT_TRUE(psalloc(&buffer));
    
    TEST_ASSERT_EQUAL_size_t(24, buffer.allocationSizeTracker[buffer.sizeOfTracker - 1]);
    
    /*
     * Test Two: Testing handling of edge cases.
     */
    TEST_ASSERT_TRUE(psalloc(&buffer));
    //TEST_ASSERT_FALSE(psalloc(&buffer));

    /*
     * Clean up.
     */
    munmap(buffer.ptrToVirtualAddressSpace, MAX_MEMORY_BUFFER_SIZE);
}

void testDsalloc(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    /*
     * Test One: Testing basic functionality.
     */
    size_t *ptrOne = (size_t*) salloc(&buffer, sizeof(size_t) * 3, _Alignof(size_t));
    size_t *ptrTwo = (size_t*) salloc(&buffer, sizeof(size_t), _Alignof(size_t));
    
    TEST_ASSERT_TRUE(ptrOne != NULL && ptrTwo != NULL);
  
    size_t oldSizeOfTracker = buffer.sizeOfTracker;

    TEST_ASSERT_TRUE(dsalloc(&buffer));
    
    for (size_t i = 0; i < oldSizeOfTracker; i++) {
        TEST_ASSERT_EQUAL_size_t(0, buffer.allocationSizeTracker[i]);
    }

    TEST_ASSERT_TRUE(buffer.ptrToVirtualAddressSpace == NULL);
    TEST_ASSERT_TRUE(buffer.bufferOffset == 0);
    TEST_ASSERT_TRUE(buffer.sizeOfTracker == 0);
}

void testRsalloc(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    /*
     * Test One: Testing basic functionality.
     */
    size_t *ptrOne = (size_t*) salloc(&buffer, sizeof(size_t) * 3, _Alignof(size_t));
    size_t *ptrTwo = (size_t*) salloc(&buffer, sizeof(size_t), _Alignof(size_t));

    TEST_ASSERT_TRUE(ptrOne != NULL && ptrTwo != NULL);

    size_t oldSizeOfTracker = buffer.sizeOfTracker;

    TEST_ASSERT_TRUE(rsalloc(&buffer));

    for (size_t i = 0; i < oldSizeOfTracker; i++) {
        TEST_ASSERT_EQUAL_size_t(0, buffer.allocationSizeTracker[i]);
    }

    TEST_ASSERT_TRUE(buffer.ptrToVirtualAddressSpace != NULL);
    TEST_ASSERT_TRUE(buffer.bufferOffset == 0);
    TEST_ASSERT_TRUE(buffer.sizeOfTracker == 0);

    /*
     * Clean up.
     */
    munmap(buffer.ptrToVirtualAddressSpace, MAX_MEMORY_BUFFER_SIZE);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testInitMemoryBuffer);
    RUN_TEST(testSalloc);
    RUN_TEST(testPsalloc);
    RUN_TEST(testDsalloc); 
    RUN_TEST(testRsalloc);

    return UNITY_END();
}


