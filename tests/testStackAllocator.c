#include "unity.h"
#include "stackAllocator.h"
#include <sys/mman.h>
#include <unistd.h>

void setUp(void) {

}

void tearDown(void) {

}

void testInitMemoryBuffer(void) {
   
    /*
     * Test 1: Testing for correct initialisation behaviour.
     */
    MemoryBuffer buffer;
    
    TEST_ASSERT_TRUE(initMemoryBuffer(&buffer));
    TEST_ASSERT_EQUAL_size_t(0, buffer.bufferOffset);
    TEST_ASSERT_TRUE(buffer.ptrToVirtualAddressSpace != NULL);
    TEST_ASSERT_TRUE(buffer.lastPtr == NULL);

    munmap(buffer.ptrToVirtualAddressSpace, 4096 * 10);
  
    /*
     * Cleaning up buffer after test one was completed.
     */
    buffer.bufferOffset = 0;
    buffer.ptrToVirtualAddressSpace = NULL;
    buffer.lastPtr = NULL;
}

void testMakePointer(void) {
    MemoryBuffer buffer;
    
    initMemoryBuffer(&buffer);
    
    /*
     * Test 1: Testing for correct initialisation behaviour.
     */
    size_t integerOne = 666;
    size_t integerTwo = 999;

    void *currentPtr = &integerOne;
    void *previousPtr = &integerTwo;
    size_t bytesOccupying = 30;

    Pointer pointer = makePointer(currentPtr, previousPtr, bytesOccupying);
    
    TEST_ASSERT_TRUE(pointer.currentPtr == &integerOne);
    TEST_ASSERT_TRUE(pointer.previousPtr == &integerTwo);
    TEST_ASSERT_EQUAL_size_t(bytesOccupying, pointer.bytesOccupying);

    /*
     * Cleaning up buffer after test one was completed.
     */
    buffer.bufferOffset = 0;
    buffer.ptrToVirtualAddressSpace = NULL;
    buffer.lastPtr = NULL;
}

void testValidateMemoryBuffer(void) {
    /*
     * Test 1: Testing if normally initialised memory buffer
     * passes the validation test -- it is expected to.
     */
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    TEST_ASSERT_TRUE(validateMemoryBufferInit(&buffer));

    /*
     * Cleaning up buffer after test one was completed.
     */
    buffer.bufferOffset = 0;
    buffer.ptrToVirtualAddressSpace = NULL;
    buffer.lastPtr = NULL;

    /*
     * Test 2: Testing if validation test returns false
     * if memory buffer was poorly initialised.
     */
    
    MemoryBuffer bufferTwo;

    TEST_ASSERT_FALSE(validateMemoryBufferInit(&buffer));
}

void testGetAlignmentPadding(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    /*
     * Test 1: Testing alignment padding if alignment == 0.
     */
    TEST_ASSERT_EQUAL_size_t((size_t) -1, getAlignmentPadding(&buffer, 0));

    /*
     * Test 2: Testing for expected alignment.
     */
    TEST_ASSERT_EQUAL_size_t(0, getAlignmentPadding(&buffer, _Alignof(size_t)));

    // More tests need to be conducted her for various cases.
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testInitMemoryBuffer);
    RUN_TEST(testMakePointer);
    RUN_TEST(testValidateMemoryBuffer);
    RUN_TEST(testGetAlignmentPadding);

    return UNITY_END();
}
