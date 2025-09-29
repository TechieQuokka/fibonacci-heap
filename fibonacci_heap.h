#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct fib_node fib_node_t;
typedef struct fib_heap fib_heap_t;

// Error codes
typedef enum {
    FIB_HEAP_SUCCESS = 0,
    FIB_HEAP_ERROR_NULL_POINTER,
    FIB_HEAP_ERROR_INVALID_HANDLE,
    FIB_HEAP_ERROR_OUT_OF_MEMORY,
    FIB_HEAP_ERROR_EMPTY_HEAP,
    FIB_HEAP_ERROR_INVALID_KEY,
    FIB_HEAP_ERROR_HEAP_CORRUPTION
} fib_heap_error_t;

// Node structure
struct fib_node {
    int key;                    // Node's key value
    void* data;                 // User data pointer

    struct fib_node* parent;    // Parent node
    struct fib_node* child;     // One of the child nodes
    struct fib_node* left;      // Left sibling
    struct fib_node* right;     // Right sibling

    int degree;                 // Number of children
    bool marked;                // Mark for cascading cut
};

// Heap structure
struct fib_heap {
    fib_node_t* min_node;       // Pointer to minimum node
    size_t node_count;          // Total number of nodes
};

// Statistics structure
typedef struct {
    size_t total_nodes;
    size_t marked_nodes;
    size_t root_nodes;
    int max_degree;
    int tree_count;
    double average_degree;
} fib_heap_statistics_t;

// Function prototypes

// Heap creation and destruction
fib_heap_t* fib_heap_create(void);
void fib_heap_destroy(fib_heap_t* heap);

// Basic operations
fib_node_t* fib_heap_insert(fib_heap_t* heap, int key, void* data);
fib_node_t* fib_heap_minimum(fib_heap_t* heap);
fib_node_t* fib_heap_extract_min(fib_heap_t* heap);
fib_heap_error_t fib_heap_decrease_key(fib_heap_t* heap, fib_node_t* node, int new_key);
fib_heap_error_t fib_heap_delete_node(fib_heap_t* heap, fib_node_t* node);
fib_heap_error_t fib_heap_union(fib_heap_t* heap1, fib_heap_t* heap2);

// Status inquiry
bool fib_heap_empty(fib_heap_t* heap);
size_t fib_heap_size(fib_heap_t* heap);

// Utility functions
fib_heap_error_t fib_heap_validate(fib_heap_t* heap);
const char* fib_heap_error_string(fib_heap_error_t error);
fib_heap_statistics_t fib_heap_get_statistics(fib_heap_t* heap);
void fib_heap_print_structure(fib_heap_t* heap);

// Node utility functions
int fib_node_get_key(fib_node_t* node);
void* fib_node_get_data(fib_node_t* node);
bool fib_node_is_root(fib_node_t* node);

#ifdef __cplusplus
}
#endif

#endif // FIBONACCI_HEAP_H