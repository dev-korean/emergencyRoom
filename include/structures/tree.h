#ifndef STRUCTURES_TREE_H
#define STRUCTURES_TREE_H

#include "systems/patient.h"

typedef struct BSTNode {
    int id;
    Patient* patient;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

BSTNode* bst_insert(BSTNode* root, Patient* patient);
Patient* bst_search(BSTNode* root, int id);
BSTNode* bst_delete(BSTNode* root, int id);
void bst_clear(BSTNode* root);

#endif // STRUCTURES_TREE_H
