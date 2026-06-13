#include "structures/queue.h"

#include <stdlib.h>

 void queue_init(Queue *queue) {
	 if (!queue) {
		 return;
	 }
	 queue->front = NULL;
	 queue->rear = NULL;
	 queue->size = 0;
 }
 
 bool queue_is_empty(const Queue *queue) {
	 return !queue || queue->size == 0;
 }
 
 bool enqueue(Queue *queue, void *data) {
	 QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
	 if (!node) {
		 return false;
	 }
 
	 node->data = data;
	 node->next = NULL;
 
	 if (queue_is_empty(queue)) {
		 queue->front = node;
		 queue->rear = node;
	 } else {
		 queue->rear->next = node;
		 queue->rear = node;
	 }
 
	 queue->size += 1;
	 return true;
 }
 
 void *dequeue(Queue *queue) {
	 QueueNode *front = NULL;
	 void *data = NULL;
 
	 if (queue_is_empty(queue)) {
		 return NULL;
	 }
 
	 front = queue->front;
	 data = front->data;
	 queue->front = front->next;
 
	 if (!queue->front) {
		 queue->rear = NULL;
	 }
 
	 free(front);
	 queue->size -= 1;
	 return data;
 }
 
 void *queue_peek(const Queue *queue) {
	 if (queue_is_empty(queue)) {
		 return NULL;
	 }
	 return queue->front->data;
 }
 
 void queue_clear(Queue *queue, void (*free_fn)(void *)) {
	 if (!queue) {
		 return;
	 }
 
	 while (!queue_is_empty(queue)) {
		 void *data = dequeue(queue);
		 if (free_fn) {
			 free_fn(data);
		 }
	 }
 }
