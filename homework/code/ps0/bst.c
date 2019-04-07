#include "bst.h"
#include <stddef.h>  // For NULL
#include <stdlib.h>  // For malloc, free

void insert_into(struct Node** root, int value) {
  // We assume root is not NULL. Undefined behavior if it is.
  if (*root == NULL) {
    // Initialize values to appropriate defaults for root node.
    *root = malloc(sizeof(struct Node));
    (*root)->value = value;
    (*root)->left = NULL;
    (*root)->right = NULL;
    return;
  }
  struct Node* head = *root;
  if (value < head->value) {
    insert_into(&head->left, value);
  }
  else if (value > head->value) {
    insert_into(&head->right, value);
  } else {
    // Value is equal. Do nothing.
  }
}

void free_tree(struct Node* root) {
  if (root == NULL) {
    return;
  }
  free_tree(root->left);
  free_tree(root->right);
  free(root);
}

size_t size_of(const struct Node* root) {
  if (root == NULL) {
    return 0;
  }
  return size_of(root->left) + size_of(root->right) + 1;
}

// Fills array with the contents of the tree rooted at root in
// sorted order. Array must already be allocated with sufficient
// space to hold the contents.
// Returns a pointer to just past the final inserted element.
int* fill_with_contants_of(const struct Node* root, int* array) {
  if (root == NULL) {
    return array;
  }
  int* head_location = fill_with_contants_of(root->left, array);
  *head_location = root->value;
  return fill_with_contants_of(root->right, head_location + 1);
}

int* contents_of(const struct Node* root) {
  if (root == NULL) {
    // malloc(0) is implementation-specific, but we want to guarantee that
    // we always return NULL if the root does not exists.
    return NULL;
  }
  const size_t size = size_of(root);
  int* contents = malloc(size * sizeof(int));
  (void) fill_with_contants_of(root, contents);
  return contents;
}

// If a minimum exists, returns it.
const struct Node* min_in(const struct Node* root) {
  if (root == NULL) {
    // Empty tree has no-min.
    return NULL;
  }
  if (root->left == NULL) {
    // We're the smallest value (no left-child).
    return root;
  }
  // Smalles value is in the left.
  return min_in(root->left);
}

const struct Node* second_min_in(const struct Node* root) {
  if (root == NULL) {
    // Empty tree has no second_min.
    return NULL;
  }
  if (root->left == NULL) {
    // The minimum is us, so the second min is the minimum in the right side.
    return min_in(root->right);
  }
  // The second-minimum is either us or is in the left side.
  const struct Node* second_min_in_left = second_min_in(root->left);
  if (second_min_in_left == NULL) {
    // Only one element in the left tree, so we're the true second-min.t
    return root;
  }
  return second_min_in_left;
}

