#ifndef STRUCTURES_PRIORITY_QUEUE_H
#define STRUCTURES_PRIORITY_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct PriorityQueueNode {
    void* data;
    int priority;
    struct PriorityQueueNode* next;
} PriorityQueueNode;

typedef struct PriorityQueue {
    PriorityQueueNode* front;
    size_t size;
} PriorityQueue;

void priority_queue_init(PriorityQueue* queue);

bool priority_queue_is_empty(const PriorityQueue* queue);

bool priority_enqueue(PriorityQueue* queue, void* data, int priority);

void* priority_dequeue(PriorityQueue* queue);

void* priority_queue_peek(const PriorityQueue* queue);

void priority_queue_clear(PriorityQueue* queue, void (*free_fn)(void*));

#endif
