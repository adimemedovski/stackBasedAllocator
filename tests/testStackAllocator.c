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

    void *integerOnePtr = &integerOne;
    void *integerTwoPtr = &integerTwo; 
    size_t bytesOccupyingOne = 666;
    size_t bytesOccupyingTwo = 888;
    
    Pointer previousPointer = makePointer(integerOnePtr, (Pointer*) NULL, bytesOccupyingOne);
    Pointer pointer = makePointer(integerTwoPtr, &previousPointer, bytesOccupyingTwo);

    TEST_ASSERT_TRUE(pointer.currentPtr == &integerTwo);
    TEST_ASSERT_TRUE(pointer.previousPtr == &previousPointer);
    TEST_ASSERT_EQUAL_size_t(bytesOccupyingTwo, pointer.bytesOccupying);
    
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

    /*
     * Cleaning up buffer after test one was completed.
     */
    buffer.bufferOffset = 0;
    buffer.ptrToVirtualAddressSpace = NULL;
    buffer.lastPtr = NULL;
}

void testIncrementBufferOffset(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    /*
     * Test One: Testing under expected situations.
     */
    TEST_ASSERT_TRUE(incrementBufferOffset(&buffer, 66));
    TEST_ASSERT_EQUAL_size_t(66, buffer.bufferOffset);
    
    /*
     * Test Two: Testing if buffer overflows are handled.
     */
    
    size_t overFlowAmount = MAX_MEMORY_BUFFER_SIZE - buffer.bufferOffset + 1;
    TEST_ASSERT_FALSE(incrementBufferOffset(&buffer, overFlowAmount));
    TEST_ASSERT_EQUAL_size_t(66, buffer.bufferOffset);

    /*
     * Test Three: Testing if buffer overflows when bufferOffset is
     * at the max memory size.
     */
    TEST_ASSERT_TRUE(incrementBufferOffset(&buffer, overFlowAmount -1));
    TEST_ASSERT_EQUAL_size_t(MAX_MEMORY_BUFFER_SIZE, buffer.bufferOffset);

    /*
     * Cleaning up buffer after test one was completed.
     */
    buffer.bufferOffset = 0;
    buffer.ptrToVirtualAddressSpace = NULL;
    buffer.lastPtr = NULL;
}

void testValidateParamsOfSalloc(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    /*
     * Test One: Expected conditions.
     */
    TEST_ASSERT_TRUE(validateParamsOfSalloc(&buffer, 30, 8));

    /*
     * Test Two: Unexpected conditions.
     */
    TEST_ASSERT_FALSE(validateParamsOfSalloc(&buffer, 0, 30));
    TEST_ASSERT_FALSE(validateParamsOfSalloc(&buffer, 30, 0));
    TEST_ASSERT_FALSE(validateParamsOfSalloc(&buffer, 0, 0));

    /*
     * Cleaning up buffer after test one was completed.
     */
    buffer.bufferOffset = 0;
    buffer.ptrToVirtualAddressSpace = NULL;
    buffer.lastPtr = NULL;
}

void testPushPointer(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);

    /*
     * Test One: Testing for expected functionality.
     */
    size_t integerOne = 33;
    size_t integerTwo = 44;
    size_t integerThree = 666;
    
    void *ptrOne = (void*) &integerOne;
    void *ptrTwo = (void*) &integerTwo;
    void *ptrThree = (void*) &integerThree;

    Pointer pointerOne = makePointer(ptrOne, ptrTwo, 8);
    
    TEST_ASSERT_TRUE(pushPointer(&buffer, &pointerOne));
    TEST_ASSERT_EQUAL_size_t(8, buffer.lastPtr -> bytesOccupying);
    
    Pointer pointerTwo = makePointer(ptrTwo, ptrThree, 666);

    TEST_ASSERT_TRUE(pushPointer(&buffer, &pointerTwo));
    TEST_ASSERT_EQUAL_size_t(666, buffer.lastPtr -> bytesOccupying);
    

    TEST_ASSERT_EQUAL_size_t(8, buffer.lastPtr -> previousPtr -> bytesOccupying);

    /*
     * Cleaning up buffer after test one was completed.
     */
    buffer.bufferOffset = 0;
    buffer.ptrToVirtualAddressSpace = NULL;
    buffer.lastPtr = NULL;
}

void testPopPointer(void) {
    MemoryBuffer buffer;
    initMemoryBuffer(&buffer);
   
    /*
     * Test One: Testing expected functionality.
     */
    size_t integerOne = 666;
    size_t integerTwo = 777; 
    
    void *ptrOne = &integerOne;
    void *ptrTwo = &integerTwo;

    Pointer pointerOne = makePointer(ptrOne, (Pointer*) NULL, 111);
    Pointer pointerTwo = makePointer(ptrTwo, &pointerOne, 222);
    
    pushPointer(&buffer, &pointerOne);
    pushPointer(&buffer, &pointerTwo);
    
    TEST_ASSERT_TRUE(popPointer(&buffer));
    TEST_ASSERT_TRUE(buffer.lastPtr == &pointerOne);
    
    /*
     * Test Two: Edge case tests.
     */
    
    TEST_ASSERT_TRUE(popPointer(&buffer));
    
    TEST_ASSERT_FALSE(popPointer(&buffer));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testInitMemoryBuffer);
    RUN_TEST(testMakePointer);
    RUN_TEST(testValidateMemoryBuffer);
    RUN_TEST(testGetAlignmentPadding);
    RUN_TEST(testIncrementBufferOffset);
    RUN_TEST(testPushPointer);
    RUN_TEST(testPopPointer);

    return UNITY_END();
}
