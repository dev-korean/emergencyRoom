#include "structures/tree.h"
#include <stdlib.h>

BSTNode* bst_insert(BSTNode* root, Patient* patient) {
    if (!patient) return root;
    if (!root) {
        BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
        if (node) {
            node->id = patient->id;
            node->patient = patient;
            node->left = NULL;
            node->right = NULL;
        }
        return node;
    }
    if (patient->id < root->id) {
        root->left = bst_insert(root->left, patient);
    } else if (patient->id > root->id) {
        root->right = bst_insert(root->right, patient);
    }
    return root;
}

Patient* bst_search(BSTNode* root, int id) {
    if (!root) return NULL;
    if (root->id == id) return root->patient;
    if (id < root->id) return bst_search(root->left, id);
    return bst_search(root->right, id);
}

static BSTNode* find_min(BSTNode* node) {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

BSTNode* bst_delete(BSTNode* root, int id) {
    if (!root) return NULL;
    if (id < root->id) {
        root->left = bst_delete(root->left, id);
    } else if (id > root->id) {
        root->right = bst_delete(root->right, id);
    } else {
        // Found the node
        if (!root->left) {
            BSTNode* temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            BSTNode* temp = root->left;
            free(root);
            return temp;
        }
        // Node with two children: Get the inorder successor (min in right subtree)
        BSTNode* temp = find_min(root->right);
        root->id = temp->id;
        root->patient = temp->patient;
        root->right = bst_delete(root->right, temp->id);
    }
    return root;
}

void bst_clear(BSTNode* root) {
    if (!root) return;
    bst_clear(root->left);
    bst_clear(root->right);
    free(root);
}
