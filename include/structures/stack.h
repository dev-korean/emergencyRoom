#ifndef STRUCTURES_STACK_H
#define STRUCTURES_STACK_H

#include <stdbool.h>
#include <stddef.h>

typedef struct StackNode {
    void* data;
    struct StackNode* next;
} StackNode;

typedef struct Stack {
    StackNode* top;
    size_t size;
} Stack;

void stack_init(Stack* stack);

bool stack_is_empty(const Stack* stack);

bool stack_push(Stack* stack, void* data);

void* stack_pop(Stack* stack);

void* stack_peek(const Stack* stack);

void stack_clear(Stack* stack, void (*free_fn)(void*));

#endif
