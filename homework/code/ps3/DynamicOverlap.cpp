#include "DynamicOverlap.h"

#include <random>
#include <iostream>
#include <iomanip>

namespace {
/* C++'s random generation facilities require both a random number source
 * and a distribution to turn those bits into useful information. Here, we
 * use a Mersenne Twister for a random source, and a random distribution that
 * maps those random bits to real numbers in the range [0, 1), which we use
 * as weights.
 */
std::mt19937 theGenerator;
std::uniform_real_distribution<double> theDistribution;
}

DynamicOverlap::DynamicOverlap() {}

DynamicOverlap::~DynamicOverlap() {
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

/* Two-pass insertion algorithm: first, insert as usual, then walk up the treap
 * and perform rotations to place the new element.
 */
void DynamicOverlap::insert(int start, int end) {
  /* Step one: Find the insertion point. */
  Node* prev = nullptr;
  Node* curr = root;
  const Interval interval(start, end);

  while (curr != nullptr) {
    prev = curr;

    // Already present, so increment the count.
    if (interval == curr->interval) {
      curr->count++;
      return;
    }
    else if (interval <  curr->interval) {
      curr = curr->left;
    }
    else { /*  interval >  curr->interval */
      curr = curr->right;
    }
  }

  /* Step two: Do the actual insertion. */
  auto* node = new Node(
    start, end,
    /*weight=*/theDistribution(theGenerator),
    /*parent=*/prev
  );
  /* Wire this node into the tree. */
  if (prev == nullptr) {
    root = node;
  } else if (interval < prev->interval) {
    prev->left = node;
  } else { /*  interval > prev->interval */
    prev->right = node;
  }

  /* Step three: Rotate the node into position. */
  while (node->parent != nullptr && node->parent->weight > node->weight) {
    rotateWithParent(node);
  }

  /* Step four: Follow the parent pointers and update their maxEndPoint
   * now that the tree rooted at our new node has changed
   */
  while (node->parent != nullptr) {
    node->parent->maxEndpoint = std::max(node->parent->maxEndpoint, node->maxEndpoint);
    node = node->parent;
  }
}

/* Deletion algorithm works by finding the node, then rotating it down to a
 * leaf position and removing it.
 */
void DynamicOverlap::remove(int start, int end) {
  /* Find the node to remove. */
  Interval interval(start, end);
  Node* toRemove = root;
  while (toRemove != nullptr) {
    if (interval == toRemove->interval) {
      // If we had just one interval so we need to delete the node.
      if (toRemove->count == 1) break;
      // Otherwise, just decrement the count and we're done.
      toRemove->count--;
      return;
    }
    else if (interval <  toRemove->interval) {
      toRemove = toRemove->left;
    }
    else { /*  interval >  curr->interval */
      toRemove = toRemove->right;
    }
  }

  /* If there's no node to remove, we're done. */
  if (toRemove == nullptr) return;

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
}

std::size_t DynamicOverlap::intervalsContainingRec(
  const Node* node, const int key) const {
  // No node, so no intervals containing this point. Also if the point is larger
  // than maxEndpoint.
  if (node == nullptr || key > node->maxEndpoint) return 0;

  // If the point is strictly smaller then the start, any interval containing it
  // are to the left.
  if (key < node->interval.start) {
    return intervalsContainingRec(node->left, key);
  }
  // At this point, all we know is that the 'key' is in [interval->start, maxEndpoint].
  // This means it maybe to the left [smaller, >key] or to the right [interval->start, maxEndpoint]
  const std::size_t count = (intervalsContainingRec(node->left, key) +
                             intervalsContainingRec(node->right, key));
  // We also check if our current node contains the value.
  if (node->interval.start <= key && key <= node->interval.end) {
    return count + node->count;
  }
  return count;
}

/* Modified tree search to find all intervals containing this key. */
std::size_t DynamicOverlap::intervalsContaining(int key) const {
  return intervalsContainingRec(root, key);
}

/* Standard rotation logic. We just have to remember to adjust the root and
 * parent pointers as needed.
 */
void DynamicOverlap::rotateWithParent(Node* node) {
  /* If we're the root, something terrible has happened. */
  if (node->parent == nullptr) {
    throw std::runtime_error("Rotating node with no parent?");
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

  /* Fix the invariant for for maxEndpoint for the parent and myself.
   * Fix the parent first (it is now lower on the tree) and myself after
   */
  node->parent->maxEndpoint = getMaxEndpoint(*node->parent);
  node->maxEndpoint = getMaxEndpoint(*node);

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

int DynamicOverlap::getMaxEndpoint(const Node& node) {
  int max = node.interval.end;
  if (node.left != nullptr) {
    max = std::max(max, node.left->maxEndpoint);
  }
  if (node.right != nullptr) {
    max = std::max(max, node.right->maxEndpoint);
  }
  return max;
}

/* Prints debugging information. This is just to make testing a bit easier. */
void DynamicOverlap::printDebugInfo() const {
  printDebugInfoRec(root, 0);
  std::cout << std::flush;
}

void DynamicOverlap::printDebugInfoRec(Node* root, unsigned indent) const {
  if (root == nullptr) {
    std::cout << std::setw(indent) << "" << "null" << '\n';
  } else {
    std::cout << std::setw(indent) << "" << "Node " << root << '\n';
    std::cout << std::setw(indent) << "" << "Interval: (" << root->interval.start << ", " << root->interval.end << ")\n";
    std::cout << std::setw(indent) << "" << "maxEndPoint: " << root->maxEndpoint << '\n';
    std::cout << std::setw(indent) << "" << "count: " << root->count << '\n';
    std::cout << std::setw(indent) << "" << "weight: " << root->weight << '\n';
    std::cout << std::setw(indent) << "" << "Left Child:" << '\n';
    printDebugInfoRec(root->left,  indent + 4);
    std::cout << std::setw(indent) << "" << "Right Child:" << '\n';
    printDebugInfoRec(root->right, indent + 4);
  }
}
