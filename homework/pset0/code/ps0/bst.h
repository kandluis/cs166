#ifndef BST_Included
#define BST_Included

#include <stddef.h> // For size_t

/* Type: struct Node
 *
 * A type representing a node in a binary search tree.
 * The keys are integers.
 */
struct Node {
  int value;
  struct Node* left;
  struct Node* right;
};

/**
 * Inserts a new value into the binary search tree whose root pointer is pointed
 * at by root. The behavior of this function is unspecified in the case
 * where memory for a new node can't be allocated or if root is NULL.
 * <p>
 * This function does not make any attempt to balance the tree. It simply inserts
 * the element in to the BST following the standard insertion algorithm. This may
 * lead to significantly imbalanced trees.
 * <p>
 * If the specified value already exists in the given BST, this function has no
 * effect and does not change the underlying BST.
 *
 * @param root  The root of the tree, which may be updated.
 * @param value The value to insert.
 */
void insert_into(struct Node** root, int value);

/**
 * Deallocates all memory associated with the indicated binary search tree.
 *
 * @param root The root of the tree to free.
 */
void free_tree(struct Node* root);

/**
 * Returns the number of elements in the binary search tree pointed at by root.
 *
 * @param root The root of the tree in question.
 * @return The number of elements in that tree.
 */
size_t size_of(const struct Node* root);

/**
 * Returns a dynamically-allocated array containing the contents of the tree
 * in sorted order. The behavior of this function is unspecified in the case
 * where the appropriate memory can't be obtained.
 *
 * @param root A pointer to the root of the tree.
 * @return An array of the tree's elements, in sorted order.
 */
int* contents_of(const struct Node* root);

/**
 * Returns a pointer to the node in the specified tree containing the second-
 * smallest element in the tree. If the tree has zero or one elements, this
 * function returns NULL.
 *
 * @param root The root of the tree in question.
 * @return A pointer to the node holding the second-smallest element in the
 *         tree, or NULL if such a node doesn't exist.
 */
const struct Node* second_min_in(const struct Node* root);

#endif
