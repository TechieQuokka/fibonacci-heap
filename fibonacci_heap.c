#include "fibonacci_heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

// Constants
#define GOLDEN_RATIO 1.618033988749895

// Helper function prototypes
static void fib_node_link(fib_node_t* child, fib_node_t* parent);
static void fib_heap_consolidate(fib_heap_t* heap);
static void fib_heap_cut(fib_heap_t* heap, fib_node_t* x, fib_node_t* y);
static void fib_heap_cascading_cut(fib_heap_t* heap, fib_node_t* y);
static void fib_node_add_to_root_list(fib_heap_t* heap, fib_node_t* node);
static void fib_node_remove_from_list(fib_node_t* node);
static void fib_node_destroy_recursive(fib_node_t* node);
static int fib_heap_calculate_max_degree(size_t node_count);

// Create a new Fibonacci heap
fib_heap_t* fib_heap_create(void) {
    fib_heap_t* heap = (fib_heap_t*)malloc(sizeof(fib_heap_t));
    if (!heap) {
        return NULL;
    }

    heap->min_node = NULL;
    heap->node_count = 0;

    return heap;
}

// Destroy the Fibonacci heap
void fib_heap_destroy(fib_heap_t* heap) {
    if (!heap) {
        return;
    }

    if (heap->min_node) {
        // Destroy all nodes starting from root list
        fib_node_t* current = heap->min_node;
        do {
            fib_node_t* next = current->right;
            fib_node_destroy_recursive(current);
            current = next;
        } while (current != heap->min_node);
    }

    free(heap);
}

// Recursively destroy nodes
static void fib_node_destroy_recursive(fib_node_t* node) {
    if (!node) {
        return;
    }

    // First destroy all children
    if (node->child) {
        fib_node_t* child = node->child;
        do {
            fib_node_t* next_child = child->right;
            fib_node_destroy_recursive(child);
            child = next_child;
        } while (child != node->child);
    }

    free(node);
}

// Insert a new node into the heap
fib_node_t* fib_heap_insert(fib_heap_t* heap, int key, void* data) {
    if (!heap) {
        return NULL;
    }

    // Create new node
    fib_node_t* new_node = (fib_node_t*)malloc(sizeof(fib_node_t));
    if (!new_node) {
        return NULL;
    }

    // Initialize node
    new_node->key = key;
    new_node->data = data;
    new_node->parent = NULL;
    new_node->child = NULL;
    new_node->degree = 0;
    new_node->marked = false;

    // Add to root list
    if (!heap->min_node) {
        // First node in heap
        new_node->left = new_node->right = new_node;
        heap->min_node = new_node;
    } else {
        // Insert into root list next to min_node
        new_node->right = heap->min_node->right;
        new_node->left = heap->min_node;
        heap->min_node->right->left = new_node;
        heap->min_node->right = new_node;

        // Update minimum if necessary
        if (key < heap->min_node->key) {
            heap->min_node = new_node;
        }
    }

    heap->node_count++;
    return new_node;
}

// Get minimum node
fib_node_t* fib_heap_minimum(fib_heap_t* heap) {
    if (!heap) {
        return NULL;
    }
    return heap->min_node;
}

// Extract minimum node
fib_node_t* fib_heap_extract_min(fib_heap_t* heap) {
    if (!heap || !heap->min_node) {
        return NULL;
    }

    fib_node_t* z = heap->min_node;

    // Add all children of min_node to root list
    if (z->child) {
        fib_node_t* child = z->child;
        do {
            fib_node_t* next_child = child->right;
            child->parent = NULL;
            fib_node_add_to_root_list(heap, child);
            child = next_child;
        } while (child != z->child);
    }

    // Remove z from root list
    fib_node_remove_from_list(z);

    if (z == z->right) {
        // z was the only node
        heap->min_node = NULL;
    } else {
        heap->min_node = z->right;
        fib_heap_consolidate(heap);
    }

    heap->node_count--;
    return z;
}

// Decrease key operation
fib_heap_error_t fib_heap_decrease_key(fib_heap_t* heap, fib_node_t* node, int new_key) {
    if (!heap || !node) {
        return FIB_HEAP_ERROR_NULL_POINTER;
    }

    if (new_key > node->key) {
        return FIB_HEAP_ERROR_INVALID_KEY;
    }

    node->key = new_key;
    fib_node_t* y = node->parent;

    if (y && node->key < y->key) {
        fib_heap_cut(heap, node, y);
        fib_heap_cascading_cut(heap, y);
    }

    if (node->key < heap->min_node->key) {
        heap->min_node = node;
    }

    return FIB_HEAP_SUCCESS;
}

// Delete a node
fib_heap_error_t fib_heap_delete_node(fib_heap_t* heap, fib_node_t* node) {
    if (!heap || !node) {
        return FIB_HEAP_ERROR_NULL_POINTER;
    }

    // Decrease key to negative infinity
    fib_heap_error_t result = fib_heap_decrease_key(heap, node, INT_MIN);
    if (result != FIB_HEAP_SUCCESS) {
        return result;
    }

    // Extract minimum (which should now be this node)
    fib_node_t* extracted = fib_heap_extract_min(heap);
    if (extracted != node) {
        return FIB_HEAP_ERROR_HEAP_CORRUPTION;
    }

    free(extracted);
    return FIB_HEAP_SUCCESS;
}

// Union two heaps
fib_heap_error_t fib_heap_union(fib_heap_t* heap1, fib_heap_t* heap2) {
    if (!heap1 || !heap2) {
        return FIB_HEAP_ERROR_NULL_POINTER;
    }

    if (!heap2->min_node) {
        // heap2 is empty, nothing to do
        return FIB_HEAP_SUCCESS;
    }

    if (!heap1->min_node) {
        // heap1 is empty, copy heap2
        heap1->min_node = heap2->min_node;
        heap1->node_count = heap2->node_count;
    } else {
        // Concatenate root lists
        fib_node_t* h1_last = heap1->min_node->left;
        fib_node_t* h2_last = heap2->min_node->left;

        h1_last->right = heap2->min_node;
        heap2->min_node->left = h1_last;

        h2_last->right = heap1->min_node;
        heap1->min_node->left = h2_last;

        // Update minimum
        if (heap2->min_node->key < heap1->min_node->key) {
            heap1->min_node = heap2->min_node;
        }

        heap1->node_count += heap2->node_count;
    }

    // Clear heap2
    heap2->min_node = NULL;
    heap2->node_count = 0;

    return FIB_HEAP_SUCCESS;
}

// Check if heap is empty
bool fib_heap_empty(fib_heap_t* heap) {
    return !heap || heap->node_count == 0;
}

// Get heap size
size_t fib_heap_size(fib_heap_t* heap) {
    return heap ? heap->node_count : 0;
}

// Helper function: Link child under parent
static void fib_node_link(fib_node_t* child, fib_node_t* parent) {
    // Remove child from root list
    fib_node_remove_from_list(child);

    // Make child a child of parent
    child->parent = parent;
    if (!parent->child) {
        parent->child = child;
        child->left = child->right = child;
    } else {
        child->right = parent->child->right;
        child->left = parent->child;
        parent->child->right->left = child;
        parent->child->right = child;
    }

    parent->degree++;
    child->marked = false;
}

// Helper function: Consolidate the heap
static void fib_heap_consolidate(fib_heap_t* heap) {
    int max_degree = fib_heap_calculate_max_degree(heap->node_count);
    fib_node_t** degree_table = (fib_node_t**)calloc(max_degree + 1, sizeof(fib_node_t*));

    if (!degree_table) {
        return; // Memory allocation failed
    }

    // Create list of root nodes
    fib_node_t** root_list = (fib_node_t**)malloc(heap->node_count * sizeof(fib_node_t*));
    if (!root_list) {
        free(degree_table);
        return;
    }

    int root_count = 0;
    fib_node_t* current = heap->min_node;
    if (current) {
        do {
            root_list[root_count++] = current;
            current = current->right;
        } while (current != heap->min_node);
    }

    // Process each root
    for (int i = 0; i < root_count; i++) {
        fib_node_t* x = root_list[i];
        int d = x->degree;

        while (degree_table[d]) {
            fib_node_t* y = degree_table[d];
            if (x->key > y->key) {
                fib_node_t* temp = x;
                x = y;
                y = temp;
            }
            fib_node_link(y, x);
            degree_table[d] = NULL;
            d++;
        }
        degree_table[d] = x;
    }

    // Rebuild root list and find new minimum
    heap->min_node = NULL;
    for (int i = 0; i <= max_degree; i++) {
        if (degree_table[i]) {
            if (!heap->min_node) {
                heap->min_node = degree_table[i];
                heap->min_node->left = heap->min_node->right = heap->min_node;
            } else {
                fib_node_add_to_root_list(heap, degree_table[i]);
                if (degree_table[i]->key < heap->min_node->key) {
                    heap->min_node = degree_table[i];
                }
            }
        }
    }

    free(degree_table);
    free(root_list);
}

// Helper function: Cut operation
static void fib_heap_cut(fib_heap_t* heap, fib_node_t* x, fib_node_t* y) {
    // Remove x from child list of y
    if (y->child == x) {
        if (x->right == x) {
            y->child = NULL;
        } else {
            y->child = x->right;
        }
    }
    fib_node_remove_from_list(x);
    y->degree--;

    // Add x to root list
    fib_node_add_to_root_list(heap, x);
    x->parent = NULL;
    x->marked = false;
}

// Helper function: Cascading cut operation
static void fib_heap_cascading_cut(fib_heap_t* heap, fib_node_t* y) {
    fib_node_t* z = y->parent;
    if (z) {
        if (!y->marked) {
            y->marked = true;
        } else {
            fib_heap_cut(heap, y, z);
            fib_heap_cascading_cut(heap, z);
        }
    }
}

// Helper function: Add node to root list
static void fib_node_add_to_root_list(fib_heap_t* heap, fib_node_t* node) {
    if (!heap->min_node) {
        heap->min_node = node;
        node->left = node->right = node;
    } else {
        node->right = heap->min_node->right;
        node->left = heap->min_node;
        heap->min_node->right->left = node;
        heap->min_node->right = node;
    }
}

// Helper function: Remove node from its list
static void fib_node_remove_from_list(fib_node_t* node) {
    node->left->right = node->right;
    node->right->left = node->left;
}

// Helper function: Calculate maximum degree
static int fib_heap_calculate_max_degree(size_t node_count) {
    if (node_count == 0) return 0;
    return (int)floor(log(node_count) / log(GOLDEN_RATIO)) + 1;
}

// Validate heap properties
fib_heap_error_t fib_heap_validate(fib_heap_t* heap) {
    if (!heap) {
        return FIB_HEAP_ERROR_NULL_POINTER;
    }

    // TODO: Implement heap property validation
    // This would check min-heap property, circular lists, parent-child relationships, etc.

    return FIB_HEAP_SUCCESS;
}

// Get error string
const char* fib_heap_error_string(fib_heap_error_t error) {
    switch (error) {
        case FIB_HEAP_SUCCESS:
            return "Success";
        case FIB_HEAP_ERROR_NULL_POINTER:
            return "Null pointer error";
        case FIB_HEAP_ERROR_INVALID_HANDLE:
            return "Invalid handle";
        case FIB_HEAP_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case FIB_HEAP_ERROR_EMPTY_HEAP:
            return "Empty heap";
        case FIB_HEAP_ERROR_INVALID_KEY:
            return "Invalid key";
        case FIB_HEAP_ERROR_HEAP_CORRUPTION:
            return "Heap corruption";
        default:
            return "Unknown error";
    }
}

// Get heap statistics
fib_heap_statistics_t fib_heap_get_statistics(fib_heap_t* heap) {
    fib_heap_statistics_t stats = {0};

    if (!heap || !heap->min_node) {
        return stats;
    }

    stats.total_nodes = heap->node_count;

    // Count root nodes and calculate other statistics
    fib_node_t* current = heap->min_node;
    int max_degree = 0;
    int total_degree = 0;

    do {
        stats.root_nodes++;
        if (current->degree > max_degree) {
            max_degree = current->degree;
        }
        total_degree += current->degree;
        current = current->right;
    } while (current != heap->min_node);

    stats.max_degree = max_degree;
    stats.tree_count = stats.root_nodes;
    stats.average_degree = stats.total_nodes > 0 ? (double)total_degree / stats.total_nodes : 0.0;

    return stats;
}

// Print heap structure (for debugging)
void fib_heap_print_structure(fib_heap_t* heap) {
    if (!heap || !heap->min_node) {
        printf("Empty heap\n");
        return;
    }

    printf("Fibonacci Heap Structure:\n");
    printf("Node count: %zu\n", heap->node_count);
    printf("Minimum key: %d\n", heap->min_node->key);

    // Print root list
    printf("Root list: ");
    fib_node_t* current = heap->min_node;
    do {
        printf("%d(d:%d%s) ", current->key, current->degree, current->marked ? ",m" : "");
        current = current->right;
    } while (current != heap->min_node);
    printf("\n");
}

// Node utility functions
int fib_node_get_key(fib_node_t* node) {
    return node ? node->key : INT_MAX;
}

void* fib_node_get_data(fib_node_t* node) {
    return node ? node->data : NULL;
}

bool fib_node_is_root(fib_node_t* node) {
    return node ? (node->parent == NULL) : false;
}