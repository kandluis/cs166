#include "Treap.h"
#include <random>
#include <iostream>
#include <iomanip>
using namespace std;

namespace {
  /* C++'s random generation facilities require both a random number source
   * and a distribution to turn those bits into useful information. Here, we
   * use a Mersenne Twister for a random source, and a random distribution that
   * maps those random bits to real numbers in the range [0, 1), which we use
   * as weights.
   */
  mt19937 theGenerator;
  uniform_real_distribution<double> theDistribution;
}

Treap::~Treap() {
  /* Deallocates all memory used by the tree. This algorithm uses O(1) auxiliary
   * storage space and is not recursive. It's due to a friend of mine, Leo
   * Shamis, who mentioned it to me after I told him that I wasn't sure whether
   * such an algorithm even existed!
   *
   * The algorithm is very simple. If the root has no left child, we just delete
   * it and move on. Otherwise, it has a left child, so we do a right rotation
   * to reduce the size of the right subtree a bit.
   */
  while (root != nullptr) {
    /* Case 1: The root has no left child. */
    if (root->left == nullptr) {
      Node* next = root->right;
      delete root;
      root = next;
    }
    /* Case 2: There is a left child, so do a right rotation. */
    else {
      /* We could go through the rotateWithParent function, but that's
       * unnecessary given that we're destroying the tree.
       */
      Node* leftChild = root->left;
      root->left = leftChild->right;
      leftChild->right = root;
      root = leftChild;
    }
  }
}

/* Standard tree search. */
bool Treap::contains(int key) const {
  Node* curr = root;
  while (curr != nullptr) {
    if      (key == curr->key)   return true;
    else if (key <  curr->key)   curr = curr->left;
    else /*  key >  curr->key */ curr = curr->right;
  }
  return false;
}

/* Two-pass insertion algorithm: first, insert as usual, then walk up the treap
 * and perform rotations to place the new element.
 */
bool Treap::insert(int key) {
  /* Step one: Find the insertion point. */
  Node* prev = nullptr;
  Node* curr = root;
  
  while (curr != nullptr) {
    prev = curr;
    
    if      (key == curr->key)   return false;       // Already present
    else if (key <  curr->key)   curr = curr->left;
    else /*  key >  curr->key */ curr = curr->right;
  }
  
  /* Step two: Do the actual insertion. */
  Node* node = new Node {
    key,                           // Store the key
    theDistribution(theGenerator), // Random weight between 0 and 1
    nullptr, nullptr,              // No children
    prev                           // Parent is last node seen.
  };
  
  /* Wire this node into the tree. */
  if (prev == nullptr) {
    root = node;
  } else if (key < prev->key) {
    prev->left = node;
  } else /*  key > prev->key */ {
    prev->right = node;
  }
  
  /* Step three: Rotate the node into position. */
  while (node->parent != nullptr && node->parent->weight > node->weight) {
    rotateWithParent(node);
  }
  
  /* Yes, we did insert something. */
  return true;
}

/* Deletion algorithm works by finding the node, then rotating it down to a
 * leaf position and removing it.
 */
bool Treap::remove(int key) {
  /* Find the node to remove. */
  Node* toRemove = root;
  while (toRemove != nullptr) {
    if      (key == toRemove->key) break;
    else if (key <  toRemove->key) toRemove = toRemove->left;
    else /*  key >  curr->key */   toRemove = toRemove->right;
  }
  
  /* If there's no node to remove, we're done. */
  if (toRemove == nullptr) return false;
  
  /* Keep rotating this node down until it has no children. */
  while (true) {
    /* Assume we rotate with the left as a default. */
    Node* child = toRemove->left;
    
    /* Switch to the right if (1) there is no left child or (2) there is a
     * right child and it has lower weight.
     */
    if (child == nullptr ||
        (toRemove->right != nullptr &&
         child->weight > toRemove->right->weight)) {
      child = toRemove->right;  
    }
    
    /* If there is no child, we're done! */
    if (child == nullptr) break;
    
    /* Otherwise, rotate the child up. */
    rotateWithParent(child);
  }
  
  /* Remove the node. */
  if (toRemove->parent != nullptr) {
    if (toRemove->parent->left == toRemove) {
      toRemove->parent->left = nullptr;
    } else {
      toRemove->parent->right = nullptr;
    }
  } else {
    root = nullptr;
  }
  
  /* Free the node. */
  delete toRemove;
  
  /* Yep, we removed something. */
  return true;
}

/* Standard rotation logic. We just have to remember to adjust the root and
 * parent pointers as needed.
 */
void Treap::rotateWithParent(Node* node) {
  /* If we're the root, something terrible has happened. */
  if (node->parent == nullptr) {
    throw runtime_error("Rotating node with no parent?");
  }
  
  /* Do the logic to "locally" rotate the nodes. This repositions the node, its
   * parent, and the middle child. However, it leaves the parent pointers of
   * these nodes unmodified; we'll handle that later.
   */
  Node* child;
  if (node == node->parent->left) {
    child = node->right;
    node->right = node->parent;
    node->parent->left = child;
  } else {
    child = node->left;
    node->left = node->parent;
    node->parent->right = child;
  }
  
  /* The node's old grandparent now needs to point at it. */
  Node* grandparent = node->parent->parent;
  
  if (grandparent != nullptr) {
    if (grandparent->left == node->parent) grandparent->left = node;
    else grandparent->right = node;
  } else {
    root = node;
  }
  
  /* At this point we now need to adjust the parent pointers. Three parents
   * need to change:
   *
   *  1. The child node that got swapped needs its parent updated.
   *  2. The node we rotated now has a new parent.
   *  3. The node's old parent now points to the node we rotated.
   *
   * We have to be super careful about this, though, because some of these
   * nodes might not exist and we need to not lose any pointers.
   */
  if (child != nullptr) child->parent = node->parent;

  Node* oldParent = node->parent;
  node->parent = oldParent->parent;
  oldParent->parent = node;
}

/* Prints debugging information. This is just to make testing a bit easier. */
void Treap::printDebugInfo() const {
  printDebugInfoRec(root, 0);
  cout << flush;
}

void Treap::printDebugInfoRec(Node* root, unsigned indent) const {
  if (root == nullptr) {
    cout << setw(indent) << "" << "null" << '\n';
  } else {
    cout << setw(indent) << "" << "Node " << root << '\n';
    cout << setw(indent) << "" << "Key: " << root->key << '\n';
    cout << setw(indent) << "" << "Left Child:" << '\n';
    printDebugInfoRec(root->left,  indent + 4);
    cout << setw(indent) << "" << "Right Child:" << '\n';
    printDebugInfoRec(root->right, indent + 4);
  }
}
