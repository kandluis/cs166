#include "SegmentTreeRMQ.h"
#include <algorithm> // For min
using namespace std;

/* Constructor recursively assembles the tree. */
SegmentTreeRMQ::SegmentTreeRMQ(const RMQEntry* elems, size_t numElems) : elems(elems) {
  root = makeTree(0, numElems);
}

/* Destructor puts away our toys. */
SegmentTreeRMQ::~SegmentTreeRMQ() {
  freeTree(root);
}

/* RMQ search fires off a recursive search. */
size_t SegmentTreeRMQ::rmq(size_t low, size_t high) const {
  return rmqRec(root, low, high);
}

/**** Actual Implementation Details ****/

/* Recursively builds a segment tree over the specified range. */
SegmentTreeRMQ::Node* SegmentTreeRMQ::makeTree(size_t low, size_t high) const {
  /* Base case: Empty range maps to an empty tree. */
  if (low == high) return nullptr;
  
  /* Base case: Single element maps to a singleton node. */
  if (low + 1 == high) return new Node {
    low, high, // The range
    low,       // Only element is the minimum
    nullptr,   // No children
    nullptr
  };
  
  /* Recursive case: Split the range in half, build trees for each half, and
   * update the minimum based on the min of the two ranges.
   */
  size_t mid = low + (high - low) / 2;
  Node* left  = makeTree(low, mid);
  Node* right = makeTree(mid, high);
  
  return new Node {
    low, high,  // The range
    elems[left->minIndex] < elems[right->minIndex]? // Min of two children
      left->minIndex : right->minIndex,
    left, right
  };
}

/* Recursively performs an RMQ search on a range of nodes. */
size_t SegmentTreeRMQ::rmqRec(Node* tree, size_t low, size_t high) const {
  /* Base case: If the whole range is being searched, return the minimum index
   * we cached earlier on.
   */
  if (tree->low == low && tree->high == high) return tree->minIndex;
  
  /* Recursive case: Need to search one, or maybe two, ranges. */
  size_t mid = tree->low + (tree->high - tree->low) / 2;
  
  /* See if we're purely to one side or the other. */
  if (high <= mid) return rmqRec(tree->left,  low, high);
  if (low  >= mid) return rmqRec(tree->right, low, high);
  
  /* We span the two ranges. Make two recursive calls and take whichever gives
   * the lower value.
   */
  size_t left  = rmqRec(tree->left,  low, mid);
  size_t right = rmqRec(tree->right, mid, high);
  
  return elems[left] < elems[right]? left : right;
}

/* Cleans up the given tree. */
void SegmentTreeRMQ::freeTree(Node* tree) {
  if (tree == nullptr) return;
  
  freeTree(tree->left);
  freeTree(tree->right);
  delete tree;
}
