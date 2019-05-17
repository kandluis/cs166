#include "WeightBalancedTree.h"
#include <cmath>   // For fabs
#include <numeric> // For accumulate  
#include <iostream>
using namespace std;

namespace { // Private helper functions
  /**
   * Given an index into a cumulative sum, returns the weight up to but not
   * including that index.
   *
   * @param weightSum The cumulative weights.
   * @param index The index in question.
   * @return The weight up to but not including that index.
   */
  double weightUpTo(const vector<double>& weightSum, size_t index) {
    return index != 0? weightSum[index - 1] : 0;
  }

  /**
   * Given a range [low, high) and the cumulative weights on the items, along
   * with a candidate index of the root, returns the cost of the split associated
   * with choosing the key at that index as the root of the tree built for
   * the range [low, high).
   *
   * Complexity: O(1)
   *
   * @param low The low end of the range (inclusive).
   * @param high The high end of the range (exclusive).
   * @param weightSum The cumulative weights of the keys.
   * @param index The index of the item that should be placed into the root.
   * @return The cost of making that split.
   */
  double costOf(size_t low, size_t high, const vector<double>& weightSum,
                size_t index) {
    double leftWeight  = weightUpTo(weightSum, index) - weightUpTo(weightSum, low);
    double rightWeight = weightUpTo(weightSum, high)  - weightUpTo(weightSum, index + 1);
    return fabs(leftWeight - rightWeight);
  }
}

/**
 * Constructs a weight-balanced tree for the elements in the range [low, high),
 * using the provided weights.
 *
 * Complexity: O(n log n)
 *
 * @param weightSums The cumulative weights of the keys
 * @param low The low end of the range to build the tree for (inclusive).
 * @param high The high end of the range to build the tree for (exclusive).
 * @return A weight-balanced tree for the given range.
 */
WeightBalancedTree::Node*
WeightBalancedTree::treeFor(const vector<double>& weightSums,
                            size_t low, size_t high) {
  /* Base case 1: if there are no nodes, the desired tree is empty. */
  if (low == high) return nullptr;
    
  /* Base case 2: if there is just one node, make the tree for that
   * node by itself.
   */
  if (low + 1 == high) {
    return new Node { low, nullptr, nullptr };
  }
    
  /* Recursive case: binary search in the appropriate range to find the
   * element that goes into the root.
   *
   * Since at each stage in the tree construction we do a binary search
   * (time O(log n)) and each node is processed once, the overall runtime
   * of this construction algorithm is O(n log n). This can be improved
   * to time O(n) by using a technique similar to that of the finger
   * search in an array: run two simultaneously exponential searches from
   * the ends of the array inward looking for the crossover point, then do
   * a binary search over the first range to overshoot. For simplicity,
   * we didn't implement that here, but you're welcome to do so if you'd
   * like!
   */
  size_t lhs = low, rhs = high;
  while (lhs + 1 < rhs) {
    /* This isn't the index of the mid that you'd typically see in a binary
     * search. The reason we're doing this is that we need to be able to
     * inspect two consecutive elements. The extra -1 term here is a bias
     * such that if the range has an even number of elements in it, we round
     * down with our probe (ensuring we look at the two most central elements)
     * rather than rounding up (probing two elements off-center.)
     */
    size_t mid = lhs + (rhs - 1 - lhs) / 2;
    
    /* Look at the goodness of the split at this position and the next. */
    double midSplit  = costOf(low, high, weightSums, mid);
    double nextSplit = costOf(low, high, weightSums, mid + 1);
                            
    /* Throw away half the array. If the values are increasing (or, at least, 
     * not decreasing) as we look from the midpoint to the element after it,
     * we know that the optimal split is either at or before the midpoint.
     */
    if (nextSplit >= midSplit) {
      rhs = mid + 1;
    }
    /* Otherwise, the midpoint and everything below it is too early. */
    else {
      lhs = mid + 1;
    }
  }

  /* We're now left with a range of one element. That's the root! */
  return new Node {
    lhs,
    treeFor(weightSums, low,     lhs),
    treeFor(weightSums, lhs + 1, high)
  }; 
}

/**
 * Given a list of the future access probabilities of the elements 0, 1, 2,
 * ..., weights.size() - 1, constructs a new, optimally-balanced BST for the
 * elements 0, 1, 2, ..., weights.size() - 1.
 *
 * Notice that the keys are not given explicitly, but the assumption is that
 * the keys are 0, 1, 2, ..., weights.size() - 1 with weights
 * weights[0], weights[1], weights[2], ..., weights[weights.size() - 1].
 */
WeightBalancedTree::WeightBalancedTree(const vector<double>& weights) {
  /* Compute the prefix sums of all the weights. */
  vector<double> weightSums(weights.size());
  partial_sum(weights.begin(), weights.end(), weightSums.begin());
  
  root = treeFor(weightSums, 0, weights.size());
}

/**
 * Frees all memory used by this tree.
 */
WeightBalancedTree::~WeightBalancedTree() {
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
 * Determines whether the specified key is present in the weight-balanced
 * tree.
 *
 * @param key The key to look up.
 * @return Whether it's present in the tree.
 */
bool WeightBalancedTree::contains(size_t key) const {
  Node* curr = root;
  while (true) {
    if (curr == nullptr)  return false;
    if (curr->key == key) return true;
    
    curr = (key < curr->key? curr->left : curr->right);
  }
}
