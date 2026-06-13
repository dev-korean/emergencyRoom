#include "structures/priority_queue.h"

#include <stdlib.h>

void priority_queue_init(PriorityQueue *queue) {
    if (!queue) {
        return;
    }
    queue->front = NULL;
    queue->size = 0;
}

bool priority_queue_is_empty(const PriorityQueue *queue) {
    return !queue || queue->size == 0;
}

bool priority_enqueue(PriorityQueue *queue, void *data, int priority) {
    PriorityQueueNode *node = NULL;
    PriorityQueueNode *current = NULL;
    PriorityQueueNode *prev = NULL;

    if (!queue) {
        return false;
    }

    node = (PriorityQueueNode *)malloc(sizeof(PriorityQueueNode));
    if (!node) {
        return false;
    }

    node->data = data;
    node->priority = priority;
    node->next = NULL;

    if (!queue->front || priority < queue->front->priority) {
        node->next = queue->front;
        queue->front = node;
        queue->size += 1;
        return true;
    }

    prev = queue->front;
    current = queue->front->next;
    while (current && priority >= current->priority) {
        prev = current;
        current = current->next;
    }

    node->next = current;
    prev->next = node;
    queue->size += 1;
    return true;
}

void *priority_dequeue(PriorityQueue *queue) {
    PriorityQueueNode *node = NULL;
    void *data = NULL;

    if (priority_queue_is_empty(queue)) {
        return NULL;
    }

    node = queue->front;
    data = node->data;
    queue->front = node->next;
    free(node);
    queue->size -= 1;
    return data;
}

void *priority_queue_peek(const PriorityQueue *queue) {
    if (priority_queue_is_empty(queue)) {
        return NULL;
    }
    return queue->front->data;
}

void priority_queue_clear(PriorityQueue *queue, void (*free_fn)(void *)) {
    if (!queue) {
        return;
    }

    while (!priority_queue_is_empty(queue)) {
        void *data = priority_dequeue(queue);
        if (free_fn) {
            free_fn(data);
        }
    }
}
