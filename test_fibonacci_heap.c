#include "fibonacci_heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

// Test result tracking
static int tests_run = 0;
static int tests_passed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("PASS: %s\n", message); \
        } else { \
            printf("FAIL: %s\n", message); \
        } \
    } while(0)

// Test basic heap creation and destruction
void test_heap_creation() {
    printf("=== Testing Heap Creation and Destruction ===\n");

    fib_heap_t* heap = fib_heap_create();
    TEST_ASSERT(heap != NULL, "Heap creation");
    TEST_ASSERT(fib_heap_empty(heap), "New heap is empty");
    TEST_ASSERT(fib_heap_size(heap) == 0, "New heap size is 0");
    TEST_ASSERT(fib_heap_minimum(heap) == NULL, "Minimum of empty heap is NULL");

    fib_heap_destroy(heap);
    printf("\n");
}

// Test basic insertion
void test_insertion() {
    printf("=== Testing Insertion ===\n");

    fib_heap_t* heap = fib_heap_create();

    // Insert single element
    fib_node_t* node1 = fib_heap_insert(heap, 10, NULL);
    TEST_ASSERT(node1 != NULL, "Insert single element");
    TEST_ASSERT(!fib_heap_empty(heap), "Heap not empty after insertion");
    TEST_ASSERT(fib_heap_size(heap) == 1, "Heap size is 1 after single insertion");
    TEST_ASSERT(fib_heap_minimum(heap) == node1, "Minimum is the inserted node");
    TEST_ASSERT(fib_node_get_key(fib_heap_minimum(heap)) == 10, "Minimum key is correct");

    // Insert multiple elements
    fib_node_t* node2 = fib_heap_insert(heap, 5, NULL);
    fib_node_t* node3 = fib_heap_insert(heap, 15, NULL);
    fib_node_t* node4 = fib_heap_insert(heap, 3, NULL);

    TEST_ASSERT(fib_heap_size(heap) == 4, "Heap size is 4 after multiple insertions");
    TEST_ASSERT(fib_node_get_key(fib_heap_minimum(heap)) == 3, "Minimum is smallest inserted key");

    fib_heap_destroy(heap);
    printf("\n");
}

// Test extract minimum
void test_extract_min() {
    printf("=== Testing Extract Minimum ===\n");

    fib_heap_t* heap = fib_heap_create();

    // Test extract from empty heap
    fib_node_t* extracted = fib_heap_extract_min(heap);
    TEST_ASSERT(extracted == NULL, "Extract from empty heap returns NULL");

    // Insert elements and extract
    int keys[] = {10, 5, 15, 3, 8, 12};
    int num_keys = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < num_keys; i++) {
        fib_heap_insert(heap, keys[i], NULL);
    }

    // Extract all elements - they should come out in sorted order
    int extracted_keys[6];
    for (int i = 0; i < num_keys; i++) {
        fib_node_t* min_node = fib_heap_extract_min(heap);
        TEST_ASSERT(min_node != NULL, "Extract minimum returns valid node");
        extracted_keys[i] = min_node->key;
        free(min_node);
    }

    // Verify sorted order
    bool sorted = true;
    for (int i = 1; i < num_keys; i++) {
        if (extracted_keys[i] < extracted_keys[i-1]) {
            sorted = false;
            break;
        }
    }
    TEST_ASSERT(sorted, "Extracted elements are in sorted order");
    TEST_ASSERT(fib_heap_empty(heap), "Heap is empty after extracting all elements");

    fib_heap_destroy(heap);
    printf("\n");
}

// Test decrease key operation
void test_decrease_key() {
    printf("=== Testing Decrease Key ===\n");

    fib_heap_t* heap = fib_heap_create();

    // Insert elements
    fib_node_t* node1 = fib_heap_insert(heap, 10, NULL);
    fib_node_t* node2 = fib_heap_insert(heap, 5, NULL);
    fib_node_t* node3 = fib_heap_insert(heap, 15, NULL);

    // Test invalid decrease key (new key > current key)
    fib_heap_error_t result = fib_heap_decrease_key(heap, node1, 20);
    TEST_ASSERT(result == FIB_HEAP_ERROR_INVALID_KEY, "Decrease key with larger value fails");

    // Valid decrease key
    result = fib_heap_decrease_key(heap, node3, 2);
    TEST_ASSERT(result == FIB_HEAP_SUCCESS, "Decrease key succeeds");
    TEST_ASSERT(fib_node_get_key(fib_heap_minimum(heap)) == 2, "New minimum after decrease key");

    // Test decrease key on minimum
    result = fib_heap_decrease_key(heap, node2, 1);
    TEST_ASSERT(result == FIB_HEAP_SUCCESS, "Decrease key on minimum succeeds");
    TEST_ASSERT(fib_node_get_key(fib_heap_minimum(heap)) == 1, "Minimum updated correctly");

    fib_heap_destroy(heap);
    printf("\n");
}

// Test delete operation
void test_delete() {
    printf("=== Testing Delete Operation ===\n");

    fib_heap_t* heap = fib_heap_create();

    // Insert elements
    fib_node_t* node1 = fib_heap_insert(heap, 10, NULL);
    fib_node_t* node2 = fib_heap_insert(heap, 5, NULL);
    fib_node_t* node3 = fib_heap_insert(heap, 15, NULL);

    size_t initial_size = fib_heap_size(heap);

    // Delete middle element
    fib_heap_error_t result = fib_heap_delete_node(heap, node1);
    TEST_ASSERT(result == FIB_HEAP_SUCCESS, "Delete node succeeds");
    TEST_ASSERT(fib_heap_size(heap) == initial_size - 1, "Heap size decreases after delete");

    // Verify heap still works correctly
    fib_node_t* min_node = fib_heap_extract_min(heap);
    TEST_ASSERT(min_node != NULL, "Can extract minimum after delete");
    TEST_ASSERT(min_node->key == 5, "Correct minimum after delete");
    free(min_node);

    fib_heap_destroy(heap);
    printf("\n");
}

// Test heap union operation
void test_union() {
    printf("=== Testing Heap Union ===\n");

    fib_heap_t* heap1 = fib_heap_create();
    fib_heap_t* heap2 = fib_heap_create();

    // Insert elements into both heaps
    fib_heap_insert(heap1, 10, NULL);
    fib_heap_insert(heap1, 5, NULL);

    fib_heap_insert(heap2, 15, NULL);
    fib_heap_insert(heap2, 3, NULL);

    size_t size1 = fib_heap_size(heap1);
    size_t size2 = fib_heap_size(heap2);

    // Union heap2 into heap1
    fib_heap_error_t result = fib_heap_union(heap1, heap2);
    TEST_ASSERT(result == FIB_HEAP_SUCCESS, "Heap union succeeds");
    TEST_ASSERT(fib_heap_size(heap1) == size1 + size2, "Combined heap has correct size");
    TEST_ASSERT(fib_heap_size(heap2) == 0, "Source heap is empty after union");
    TEST_ASSERT(fib_node_get_key(fib_heap_minimum(heap1)) == 3, "Minimum is correct after union");

    fib_heap_destroy(heap1);
    fib_heap_destroy(heap2);
    printf("\n");
}

// Test with user data
void test_user_data() {
    printf("=== Testing User Data ===\n");

    fib_heap_t* heap = fib_heap_create();

    int data1 = 100, data2 = 200, data3 = 300;

    fib_node_t* node1 = fib_heap_insert(heap, 10, &data1);
    fib_node_t* node2 = fib_heap_insert(heap, 5, &data2);
    fib_node_t* node3 = fib_heap_insert(heap, 15, &data3);

    TEST_ASSERT(fib_node_get_data(node1) == &data1, "Node 1 data is correct");
    TEST_ASSERT(fib_node_get_data(node2) == &data2, "Node 2 data is correct");
    TEST_ASSERT(fib_node_get_data(node3) == &data3, "Node 3 data is correct");

    // Extract minimum and check data
    fib_node_t* min_node = fib_heap_extract_min(heap);
    TEST_ASSERT(fib_node_get_data(min_node) == &data2, "Minimum node has correct data");
    free(min_node);

    fib_heap_destroy(heap);
    printf("\n");
}

// Test statistics
void test_statistics() {
    printf("=== Testing Statistics ===\n");

    fib_heap_t* heap = fib_heap_create();

    // Insert some elements
    for (int i = 0; i < 10; i++) {
        fib_heap_insert(heap, i, NULL);
    }

    fib_heap_statistics_t stats = fib_heap_get_statistics(heap);
    TEST_ASSERT(stats.total_nodes == 10, "Statistics show correct total nodes");
    TEST_ASSERT(stats.root_nodes == 10, "Statistics show correct root nodes (before consolidation)");

    // Extract minimum to force consolidation
    fib_node_t* min_node = fib_heap_extract_min(heap);
    free(min_node);

    stats = fib_heap_get_statistics(heap);
    TEST_ASSERT(stats.total_nodes == 9, "Statistics updated after extraction");

    fib_heap_destroy(heap);
    printf("\n");
}

// Performance test
void test_performance() {
    printf("=== Performance Test ===\n");

    const int num_operations = 10000;
    fib_heap_t* heap = fib_heap_create();
    fib_node_t** nodes = malloc(num_operations * sizeof(fib_node_t*));

    clock_t start = clock();

    // Insert many elements
    for (int i = 0; i < num_operations; i++) {
        nodes[i] = fib_heap_insert(heap, rand() % 1000, NULL);
    }

    // Perform some decrease key operations
    for (int i = 0; i < num_operations / 10; i++) {
        int idx = rand() % num_operations;
        fib_heap_decrease_key(heap, nodes[idx], rand() % 100);
    }

    // Extract all elements
    while (!fib_heap_empty(heap)) {
        fib_node_t* min_node = fib_heap_extract_min(heap);
        free(min_node);
    }

    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Performance test with %d operations completed in %f seconds\n",
           num_operations, cpu_time_used);

    free(nodes);
    fib_heap_destroy(heap);
    printf("\n");
}

// Run all tests
int main() {
    printf("Starting Fibonacci Heap Tests...\n\n");

    srand(time(NULL));

    test_heap_creation();
    test_insertion();
    test_extract_min();
    test_decrease_key();
    test_delete();
    test_union();
    test_user_data();
    test_statistics();
    test_performance();

    printf("=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    printf("Success rate: %.1f%%\n", (double)tests_passed / tests_run * 100.0);

    return (tests_passed == tests_run) ? 0 : 1;
}