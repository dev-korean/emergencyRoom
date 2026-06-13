#include "structures/list.h"

#include <stdlib.h>

void list_init(LinkedList *list) {
	if (!list) {
		return;
	}
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

bool list_is_empty(const LinkedList *list) {
	return !list || list->size == 0;
}

bool list_push_front(LinkedList *list, void *data) {
	ListNode *node = NULL;

	if (!list) {
		return false;
	}

	node = (ListNode *)malloc(sizeof(ListNode));
	if (!node) {
		return false;
	}

	node->data = data;
	node->next = list->head;

	list->head = node;
	if (!list->tail) {
		list->tail = node;
	}
	list->size += 1;
	return true;
}

bool list_push_back(LinkedList *list, void *data) {
	ListNode *node = NULL;

	if (!list) {
		return false;
	}

	node = (ListNode *)malloc(sizeof(ListNode));
	if (!node) {
		return false;
	}

	node->data = data;
	node->next = NULL;

	if (!list->tail) {
		list->head = node;
		list->tail = node;
	} else {
		list->tail->next = node;
		list->tail = node;
	}

	list->size += 1;
	return true;
}

void *list_pop_front(LinkedList *list) {
	ListNode *node = NULL;
	void *data = NULL;

	if (list_is_empty(list)) {
		return NULL;
	}

	node = list->head;
	data = node->data;
	list->head = node->next;
	if (!list->head) {
		list->tail = NULL;
	}

	free(node);
	list->size -= 1;
	return data;
}

void *list_find(const LinkedList *list, list_compare_fn compare, const void *key) {
	ListNode *current = NULL;

	if (!list || !compare) {
		return NULL;
	}

	current = list->head;
	while (current) {
		if (compare(current->data, key) == 0) {
			return current->data;
		}
		current = current->next;
	}

	return NULL;
}

bool list_remove(LinkedList *list, list_compare_fn compare, const void *key, void (*free_fn)(void *)) {
	ListNode *prev = NULL;
	ListNode *current = NULL;

	if (!list || !compare) {
		return false;
	}

	current = list->head;
	while (current) {
		if (compare(current->data, key) == 0) {
			if (prev) {
				prev->next = current->next;
			} else {
				list->head = current->next;
			}

			if (current == list->tail) {
				list->tail = prev;
			}

			if (free_fn) {
				free_fn(current->data);
			}
			free(current);
			list->size -= 1;
			return true;
		}

		prev = current;
		current = current->next;
	}

	return false;
}

void list_clear(LinkedList *list, void (*free_fn)(void *)) {
	if (!list) {
		return;
	}

	while (!list_is_empty(list)) {
		void *data = list_pop_front(list);
		if (free_fn) {
			free_fn(data);
		}
	}
}
