#ifndef STRUCTURES_LIST_H
#define STRUCTURES_LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ListNode {
    void *data;
    struct ListNode *next;
} ListNode;

typedef struct LinkedList {
    ListNode *head;
    ListNode *tail;
    size_t size;
} LinkedList;

typedef int (*list_compare_fn)(const void *item, const void *key);

void list_init(LinkedList *list);

bool list_is_empty(const LinkedList *list);

bool list_push_front(LinkedList *list, void *data);

bool list_push_back(LinkedList *list, void *data);

void *list_pop_front(LinkedList *list);

void *list_find(const LinkedList *list, list_compare_fn compare, const void *key);

bool list_remove(LinkedList *list, list_compare_fn compare, const void *key, void (*free_fn)(void *));

void list_clear(LinkedList *list, void (*free_fn)(void *));

#endif
