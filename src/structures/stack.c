#include "structures/stack.h"

#include <stdlib.h>

void stack_init(Stack* stack) {
    if (!stack) return;
    stack->top = NULL;
    stack->size = 0;
}

bool stack_is_empty(const Stack* stack) {
    return !stack || stack->size == 0;
}

bool stack_push(Stack* stack, void* data) {
    if (!stack) return false;
    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    if (!node) return false;
    node->data = data;
    node->next = stack->top;
    stack->top = node;
    stack->size += 1;
    return true;
}

void* stack_pop(Stack* stack) {
    if (stack_is_empty(stack)) return NULL;
    StackNode* node = stack->top;
    void* data = node->data;
    stack->top = node->next;
    free(node);
    stack->size -= 1;
    return data;
}

void* stack_peek(const Stack* stack) {
    if (stack_is_empty(stack)) return NULL;
    return stack->top->data;
}

void stack_clear(Stack* stack, void (*free_fn)(void*)) {
    if (!stack) return;
    while (!stack_is_empty(stack)) {
        void* data = stack_pop(stack);
        if (free_fn) free_fn(data);
    }
}
