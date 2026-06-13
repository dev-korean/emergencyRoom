#ifndef STRUCTURES_QUEUE_H
#define STRUCTURES_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct QueueNode {
    void *data;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *front;
    QueueNode *rear;
    size_t size;
} Queue;

void queue_init(Queue *queue);

bool queue_is_empty(const Queue *queue);

bool enqueue(Queue *queue, void *data);

void *dequeue(Queue *queue);

void *queue_peek(const Queue *queue);

void queue_clear(Queue *queue, void (*free_fn)(void *));

#endif
