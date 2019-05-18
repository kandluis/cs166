#include "SplayTree.h"

#include <sstream>

SplayTree::SplayTree(size_t nElems) {
  root = treeFor(0, nElems);
}

/**
 * Constructs a perfectly balanced tree for the values in the range
 * [low, high).
 */
SplayTree::Node*
SplayTree::treeFor(size_t low, size_t high) {
  /* Base Case: The empty range is represented by an empty tree. */
  if (low == high) return nullptr;

  /* Otherwise, pull out the middle, then recursively construct trees for the
   * left and right ranges.
   */
  size_t mid = low + (high - low) / 2;
  return new Node {
    mid,
    treeFor(low, mid),
    treeFor(mid + 1, high)
  };
}

/* Prints the given tree. Each line is a node */
std::string SplayTree::toString(const Node* const root) {
  if (root == nullptr) {
    return "";
  }
  std::ostringstream builder;
  const auto nodeName = [](const Node * node) {
    return (node == nullptr) ? "None" : std::to_string(node->key);
  };
  builder << "Node: " << std::to_string(root->key)
            << " LeftChild: " << nodeName(root->left)
            << " RightChild: " << nodeName(root->right);
  if (root->left != nullptr) {
    builder << std::endl;
    builder << toString(root->left);
  }
  if (root->right != nullptr) {
    builder << std::endl;
    builder << toString(root->right);
  }
  return builder.str();
}


/**
 * Frees all memory used by this tree.
 */
SplayTree::~SplayTree() {
  /* This is the same algorithm we used in PS3. It takes time O(n) and uses
   * only O(1) auxiliary memory.
   */
  while (root != nullptr) {
    /* If the root has no left subtree, just delete the root. */
    if (root->left == nullptr) {
      Node* next = root->right;
      delete root;
      root = next;
    }
    /* Otherwise, the root has a left subtree. Do a right rotation to move
     * that child to the left.
     */
    else {
      Node* child = root->left;
      root->left = child->right;
      child->right = root;
      root = child;
    }
  }
}

/**
 * Determines whether the specified key is present in the splay tree.
 */
bool SplayTree::contains(std::size_t key) const {
  // Handle edge-case where we're querying an empty tree.
  if (root == nullptr) {
    return false;
  }
  // leftTree is a tree containing all items so far known to be less than key.
  Node* leftTree = nullptr;
  // The largest node on leftTree.
  Node* largestOnLeft = nullptr;
  // rightTree is a tree containing all items so far known to be larger than key.
  Node* rightTree = nullptr;
  // The smallest node on rightTree.
  Node* smallestOnRight = nullptr;
  // middleTree points to the subtree rooted at the node we're currently testing.
  Node* middleTree = root;
  // The original key we were searching for. 'key' might be modified in
  // the while loop if we fall off the tree.
  const std::size_t originalKey = key;
  while (middleTree->key != key) {
    if (middleTree->key < key) {
      // Falling of here is the same as if 'middleTree' were the node, so search
      // for that instead.
      if (middleTree->right == nullptr) {
        key = middleTree->key;
        continue;
      }
      /* Found the value on the right child.
      * Basically, we want to take a tree like this:
      *
      * L    x    R
      *     / \
      *    A   y
      *        |
      *        B
      *
      * into:
      *
      * L    y   R
      *  \   |
      *   x  B
      *  /
      * A
      */
      if (middleTree->right->key == key) {
        Node* const x = middleTree;
        middleTree = x->right;
        x->right = nullptr;
        if (largestOnLeft == nullptr) {
          leftTree = x;
        }
        else /* largestOnLeft != nullptr */ {
          largestOnLeft->right = x;
        }
        largestOnLeft = x;
      }
      /* The value is somewhere right -> right.
      * Basically, we want to take a tree like this:
      *
      * L    x    R
      *     / \
      *    A   y
      *       / \
      *      B   z
      *          |
      *          C
      *
      * into:
      *
      *   L    z    R
      *    \   |
      *     y  C
      *    /
      *   x
      *  / \
      * A   B
      */
      else if (middleTree->right->key < key) {
        // We fell off. This is the same as if we had been looking
        // for y this whole time.
        if (middleTree->right->right == nullptr) {
          key = middleTree->right->key;
          continue;
        }
        Node* const x = middleTree;
        middleTree = x->right->right;
        x->right->right = nullptr;
        Node* const y = x->right;
        x->right = x->right->left;
        y->left = x;
        if (largestOnLeft == nullptr) {
          leftTree = y;
        }
        else { /* largestOnLeft != nullptr */
          largestOnLeft->right = y;
        }
        largestOnLeft = y;
      }
      /* The value is somewhere right -> left.
      * Basically, we want to take a tree like this:
      *
      * L    x    R
      *     / \
      *    A   y
      *       / \
      *      z   C
      *      |
      *      B
      *
      * into:
      *
      * L    z    R
      *  \   |   /
      *   x  B  y
      *  /       \
      * A         C
      */
      else { /* key < middleTree->right->key */
        // We fell off. This is the same as if we had been looking
        // for y this whole time.
        if (middleTree->right->left == nullptr) {
          key = middleTree->right->key;
          continue;
        }
        Node* const x = middleTree;
        middleTree = x->right->left;
        x->right->left = nullptr;
        if (smallestOnRight == nullptr) {
          rightTree = x->right;
        }
        else { /* smallestOnRight != nullptr */
          smallestOnRight->left = x->right;
        }
        smallestOnRight = x->right;
        x->right = nullptr;
        if (largestOnLeft == nullptr) {
          leftTree = x;
        } else { /* largestOnLeft != nullptr */
          largestOnLeft->right = x;
        }
        largestOnLeft = x;
      }
    }
    else { /* key < middleTree->key */
      // See symetric comment above for right child.
      if (middleTree->left == nullptr) {
        key = middleTree->key;
        continue;
      }
      /* Found the value on the left child.
      * Basically, we want to take a tree like this:
      *
      * L    x    R
      *     / \
      *    y   B
      *    |
      *    A
      *
      * into:
      *
      * L   y    R
      *     |   /
      *     A  x
      *         \
      *          B
      */
      if (middleTree->left->key == key) {
        Node* const x = middleTree;
        middleTree = x->left;
        x->left = nullptr;
        if (smallestOnRight == nullptr) {
          rightTree = x;
        }
        else { /* smallestOnRight != nullptr */
          smallestOnRight->left = x;
        }
        smallestOnRight = x;
      }
      /* The value is somewhere left -> right.
      * Basically, we want to take a tree like this:
      *
      * L    x    R
      *     / \
      *    y   C
      *   / \
      *  A   z
      *      |
      *      B
      *
      * into:
      *
      * L    z    R
      *  \   |   /
      *   y  A  x
      *  /       \
      * A         C
      */
      else if (middleTree->left->key < key) {
        // See comments on symmetric operation above.
        if (middleTree->left->right == nullptr) {
          key = middleTree->left->key;
          continue;
        }
        Node* const x = middleTree;
        middleTree = x->left->right;
        x->left->right = nullptr;
        if (largestOnLeft == nullptr) {
          leftTree = x->left;
        }
        else { /* largestOnLeft != nullptr */
          largestOnLeft->right = x->left;
        }
        largestOnLeft = x->left;
        x->left = nullptr;
        if (smallestOnRight == nullptr) {
          rightTree = x;
        } else { /* smallestOnRight != nullptr */
          smallestOnRight->left = x;
        }
        smallestOnRight = x;
      }
      /* The value is somewhere left -> left.
      * Basically, we want to take a tree like this:
      *
      * L    x    R
      *     / \
      *    y   C
      *   / \
      *  z   B
      *  |
      *  A
      *
      * into:
      *
      * L   z    R
      *     |   /
      *     A  y
      *         \
      *          x
      *         / \
      *        B   C
      */
      else { /* key < middleTree->left->key */
        if (middleTree->left->left == nullptr) {
          key = middleTree->left->key;
          continue;
        }
        Node* const x = middleTree;
        middleTree = x->left->left;
        x->left->left = nullptr;
        Node* const y = x->left;
        x->left = x->left->right;
        y->right = x;
        if (smallestOnRight == nullptr) {
          rightTree = y;
        }
        else { /* smallestOnRight != nullptr */
          smallestOnRight->left = y;
        }
        smallestOnRight = y;
      }
    }
  }
  /* Complete the splaying assembling back the middle/left/right
  * trees. We take a tree like the below:
  *
  * L    x    R
  *     / \
  *    A   B
  *
  * into:
  *
  *       x
  *      / \
  *     L   R
  *      \ /
  *      A B
  */
  if (leftTree != nullptr) {
    Node* const A = middleTree->left;
    middleTree->left = leftTree;
    largestOnLeft->right = A;
    leftTree = nullptr;
    largestOnLeft = nullptr;

  }
  if (rightTree != nullptr) {
    Node* const B = middleTree->right;
    middleTree->right = rightTree;
    smallestOnRight->left = B;
    rightTree = nullptr;
    smallestOnRight = nullptr;
  }
  // Mutate the root to point to our new tree.
  root = middleTree;

  // If we splayed the original key, then we found it.
  // Otherwise, we fell off the tree and splayed a different
  // key.
  return key == originalKey;
}
