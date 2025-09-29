# Fibonacci Heap Implementation in C

A complete, efficient implementation of the Fibonacci Heap data structure in C, based on the specifications from "Introduction to Algorithms" (CLRS).

## Overview

This project provides a full-featured Fibonacci Heap implementation with all standard operations:

- **INSERT**: O(1) amortized
- **MINIMUM**: O(1) worst-case
- **EXTRACT-MIN**: O(log n) amortized
- **DECREASE-KEY**: O(1) amortized
- **DELETE**: O(log n) amortized
- **UNION**: O(1) worst-case

## Quick Start

### Building

```bash
# Build everything
make

# Build and run tests
make test

# Build and run examples
make examples
```

### Basic Usage

```c
#include "fibonacci_heap.h"

// Create a new heap
fib_heap_t* heap = fib_heap_create();

// Insert elements
fib_node_t* node1 = fib_heap_insert(heap, 10, "data1");
fib_node_t* node2 = fib_heap_insert(heap, 5, "data2");

// Get minimum
fib_node_t* min = fib_heap_minimum(heap);
printf("Minimum key: %d\n", fib_node_get_key(min));

// Extract minimum
fib_node_t* extracted = fib_heap_extract_min(heap);
printf("Extracted: %d\n", extracted->key);
free(extracted);

// Clean up
fib_heap_destroy(heap);
```

## API Reference

### Core Functions

- `fib_heap_t* fib_heap_create(void)` - Create new heap
- `void fib_heap_destroy(fib_heap_t* heap)` - Destroy heap
- `fib_node_t* fib_heap_insert(fib_heap_t* heap, int key, void* data)` - Insert element
- `fib_node_t* fib_heap_extract_min(fib_heap_t* heap)` - Extract minimum
- `fib_heap_error_t fib_heap_decrease_key(fib_heap_t* heap, fib_node_t* node, int new_key)` - Decrease key
- `bool fib_heap_empty(fib_heap_t* heap)` - Check if empty
- `size_t fib_heap_size(fib_heap_t* heap)` - Get size

## Performance

| Operation | Time Complexity |
|-----------|----------------|
| INSERT | O(1) amortized |
| MINIMUM | O(1) worst-case |
| EXTRACT-MIN | O(log n) amortized |
| DECREASE-KEY | O(1) amortized |
| DELETE | O(log n) amortized |
| UNION | O(1) worst-case |

## Build Targets

```bash
make all       # Build library and executables
make test      # Build and run tests
make examples  # Build and run examples
make clean     # Clean build artifacts
```

## References

- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction to Algorithms* (3rd ed.). MIT Press.