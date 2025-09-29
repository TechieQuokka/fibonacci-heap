#include "fibonacci_heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example 1: Basic Priority Queue Usage
void example_priority_queue() {
    printf("=== Example 1: Basic Priority Queue ===\n");

    fib_heap_t* heap = fib_heap_create();

    // Insert tasks with priorities
    fib_heap_insert(heap, 3, "Low priority task");
    fib_heap_insert(heap, 1, "High priority task");
    fib_heap_insert(heap, 2, "Medium priority task");
    fib_heap_insert(heap, 0, "Critical task");

    printf("Processing tasks by priority:\n");
    while (!fib_heap_empty(heap)) {
        fib_node_t* min_node = fib_heap_extract_min(heap);
        printf("Priority %d: %s\n", min_node->key, (char*)min_node->data);
        free(min_node);
    }

    fib_heap_destroy(heap);
    printf("\n");
}

// Example 2: Dijkstra's Algorithm Simulation
typedef struct {
    int vertex;
    int distance;
} vertex_data_t;

void example_dijkstra_simulation() {
    printf("=== Example 2: Dijkstra's Algorithm Simulation ===\n");

    fib_heap_t* heap = fib_heap_create();
    const int num_vertices = 5;

    // Initialize distances (simulate graph with 5 vertices)
    vertex_data_t vertices[5];
    fib_node_t* handles[5];

    for (int i = 0; i < num_vertices; i++) {
        vertices[i].vertex = i;
        vertices[i].distance = (i == 0) ? 0 : 1000; // Source vertex 0 has distance 0
        handles[i] = fib_heap_insert(heap, vertices[i].distance, &vertices[i]);
    }

    printf("Initial state:\n");
    for (int i = 0; i < num_vertices; i++) {
        printf("Vertex %d: distance = %d\n", i, vertices[i].distance);
    }

    // Simulate distance updates (decrease key operations)
    printf("\nSimulating edge relaxations:\n");

    // Update distance to vertex 1
    vertices[1].distance = 10;
    fib_heap_decrease_key(heap, handles[1], 10);
    printf("Updated vertex 1 distance to 10\n");

    // Update distance to vertex 2
    vertices[2].distance = 5;
    fib_heap_decrease_key(heap, handles[2], 5);
    printf("Updated vertex 2 distance to 5\n");

    // Update distance to vertex 3
    vertices[3].distance = 15;
    fib_heap_decrease_key(heap, handles[3], 15);
    printf("Updated vertex 3 distance to 15\n");

    printf("\nProcessing vertices in order of shortest distance:\n");
    while (!fib_heap_empty(heap)) {
        fib_node_t* min_node = fib_heap_extract_min(heap);
        vertex_data_t* vertex = (vertex_data_t*)min_node->data;
        printf("Visit vertex %d (distance: %d)\n", vertex->vertex, vertex->distance);
        free(min_node);
    }

    fib_heap_destroy(heap);
    printf("\n");
}

// Example 3: Dynamic Priority Adjustment
typedef struct {
    char name[32];
    int priority;
    int id;
} task_t;

void example_dynamic_priorities() {
    printf("=== Example 3: Dynamic Priority Adjustment ===\n");

    fib_heap_t* heap = fib_heap_create();

    // Create some tasks
    task_t tasks[] = {
        {"Email processing", 5, 1},
        {"Database backup", 3, 2},
        {"User request", 7, 3},
        {"System update", 2, 4},
        {"Log cleanup", 8, 5}
    };

    fib_node_t* handles[5];
    int num_tasks = sizeof(tasks) / sizeof(tasks[0]);

    // Insert all tasks
    for (int i = 0; i < num_tasks; i++) {
        handles[i] = fib_heap_insert(heap, tasks[i].priority, &tasks[i]);
        printf("Added task: %s (priority: %d)\n", tasks[i].name, tasks[i].priority);
    }

    printf("\nAdjusting priorities dynamically:\n");

    // Urgent email came in - increase priority of email processing
    tasks[0].priority = 1;
    fib_heap_decrease_key(heap, handles[0], 1);
    printf("Email processing priority increased to 1 (urgent!)\n");

    // System update becomes less critical
    tasks[3].priority = 6;
    // Note: Fibonacci heap doesn't support increase_key directly
    // In practice, you'd delete and re-insert
    printf("System update priority would be decreased to 6\n");

    printf("\nProcessing tasks with updated priorities:\n");
    while (!fib_heap_empty(heap)) {
        fib_node_t* min_node = fib_heap_extract_min(heap);
        task_t* task = (task_t*)min_node->data;
        printf("Executing: %s (priority: %d, id: %d)\n",
               task->name, task->priority, task->id);
        free(min_node);
    }

    fib_heap_destroy(heap);
    printf("\n");
}

// Example 4: Heap Union and Merging
void example_heap_union() {
    printf("=== Example 4: Heap Union ===\n");

    // Create two separate heaps
    fib_heap_t* heap1 = fib_heap_create();
    fib_heap_t* heap2 = fib_heap_create();

    // Add elements to first heap
    printf("Adding elements to heap1: ");
    int heap1_values[] = {10, 5, 15};
    for (int i = 0; i < 3; i++) {
        fib_heap_insert(heap1, heap1_values[i], NULL);
        printf("%d ", heap1_values[i]);
    }
    printf("\n");

    // Add elements to second heap
    printf("Adding elements to heap2: ");
    int heap2_values[] = {3, 8, 12};
    for (int i = 0; i < 3; i++) {
        fib_heap_insert(heap2, heap2_values[i], NULL);
        printf("%d ", heap2_values[i]);
    }
    printf("\n");

    printf("Heap1 minimum: %d\n", fib_node_get_key(fib_heap_minimum(heap1)));
    printf("Heap2 minimum: %d\n", fib_node_get_key(fib_heap_minimum(heap2)));

    // Union heap2 into heap1
    fib_heap_union(heap1, heap2);
    printf("After union, combined heap minimum: %d\n",
           fib_node_get_key(fib_heap_minimum(heap1)));
    printf("Combined heap size: %zu\n", fib_heap_size(heap1));

    printf("Extracting all elements from combined heap: ");
    while (!fib_heap_empty(heap1)) {
        fib_node_t* min_node = fib_heap_extract_min(heap1);
        printf("%d ", min_node->key);
        free(min_node);
    }
    printf("\n");

    fib_heap_destroy(heap1);
    fib_heap_destroy(heap2);
    printf("\n");
}

// Example 5: Heap Statistics and Analysis
void example_heap_statistics() {
    printf("=== Example 5: Heap Statistics ===\n");

    fib_heap_t* heap = fib_heap_create();

    // Insert elements to create a more complex structure
    printf("Inserting elements: ");
    for (int i = 20; i >= 1; i--) {
        fib_heap_insert(heap, i, NULL);
        printf("%d ", i);
    }
    printf("\n");

    // Get initial statistics
    fib_heap_statistics_t stats = fib_heap_get_statistics(heap);
    printf("\nInitial heap statistics:\n");
    printf("Total nodes: %zu\n", stats.total_nodes);
    printf("Root nodes: %zu\n", stats.root_nodes);
    printf("Tree count: %d\n", stats.tree_count);
    printf("Max degree: %d\n", stats.max_degree);
    printf("Average degree: %.2f\n", stats.average_degree);

    // Extract a few elements to trigger consolidation
    printf("\nExtracting minimum elements to trigger consolidation:\n");
    for (int i = 0; i < 5; i++) {
        fib_node_t* min_node = fib_heap_extract_min(heap);
        printf("Extracted: %d\n", min_node->key);
        free(min_node);
    }

    // Get updated statistics
    stats = fib_heap_get_statistics(heap);
    printf("\nHeap statistics after consolidation:\n");
    printf("Total nodes: %zu\n", stats.total_nodes);
    printf("Root nodes: %zu\n", stats.root_nodes);
    printf("Tree count: %d\n", stats.tree_count);
    printf("Max degree: %d\n", stats.max_degree);
    printf("Average degree: %.2f\n", stats.average_degree);

    // Print heap structure
    printf("\nHeap structure:\n");
    fib_heap_print_structure(heap);

    fib_heap_destroy(heap);
    printf("\n");
}

int main() {
    printf("Fibonacci Heap Usage Examples\n");
    printf("==============================\n\n");

    example_priority_queue();
    example_dijkstra_simulation();
    example_dynamic_priorities();
    example_heap_union();
    example_heap_statistics();

    printf("All examples completed successfully!\n");
    return 0;
}