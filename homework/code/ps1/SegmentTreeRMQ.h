/******************************************************************************
 * File: SegmentTreeRMQ.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of an RMQ data structure backed by a segment tree. This is
 * another popular <O(n), O(log n)> approach to implementing RMQ that's based
 * on a different intuition than the hybrid structure. It works by building up
 * a hierarchical tree structure representing different ranges in the tree.
 * Specifically, the root node represents the entire range, and the left and
 * right subtrees recursively represent segment trees for the first and second
 * halves of the range, respectively.
 *
 * To construct a segment tree, we recursively propagage information up about
 * the minimum value in each range, along the lines of what we'd do for the
 * sparse table or precomputed RMQ structure. This takes time O(n) because we
 * have the nice recurrence relation T(n) = 2T(n / 2) + O(1) that arises from
 * processing two subproblems of roughly half the size (2T(n / 2)) and then
 * doing O(1) work to compute the minimum value of each range.
 *
 * To perform a range minimum query, we compare the range in question to the
 * range stored within a node. If the range perfectly equals the range in the
 * node, we just return the cached minimum value. Otherwise, if the range is
 * purely in the left or right subtree, we recursively perform the RMQ in that
 * subrange.
 *
 * There's one more case, and that's what happens if the range we want to query
 * for happens to span across both children. In that case, we make to recursive
 * queries, one for the left and right subtrees, and then take the minimum
 * value that comes back.
 *
 * At first glance, it might seem like this runs in time O(n), since the
 * recursion might branch at each level. However, it turns out that with a more
 * precise analysis, we can show that the runtime is actually only O(log n).
 * Here's why. For starters, if we perform a search and we don't have any
 * branching recursion at all, then the runtime will be O(log n) because we're
 * just doing a standard, top-down tree search.
 *
 * Now, suppose that we do reach a point where we make a branching recursive
 * call. Think about what happens in the subsequent recursive calls. If we
 * have a branching recursion, then it means that we have a range that spans
 * from the left-hand side of the range to the right-hand side of the range,
 * which, diagrammatically, looks like this:
 *
 *                        +------------------------------+
 *                        |     range being searched     |
 *                        +------------------------------+
 *                   +-------------------+ +-------------------+
 *                   |    left half      | |     right half    |
 *                   +-------------------+ +-------------------+
 *
 * When we chop the range being searched into two ranges, it will look like
 * this:
 *
 *                        +--------------+ +-------------+
 *                        |              | |             |
 *                        +--------------+ +-------------+
 *                   +-------------------+ +-------------------+
 *                   |    left half      | |     right half    |
 *                   +-------------------+ +-------------------+
 *
 * Notice that the two smaller ranges being searched in the left and right
 * halves of the array are flush up against the sides of those arrays. That's
 * significant because it influences the shapes of the recursive calls that
 * can arise in the future. For the purposes of this discussion, we'll focus on
 * the right half of the array, but this same reasoning is equally applicable
 * to the left half of the array as well.
 *
 * Because the range we're searching in the right half of the array is flush
 * against the left side of the array, each future recursive call can take on
 * one of three different shapes:
 *
 *    Case 1: Searching the whole range
 *
 *                 +------------------+
 *                 |   query range    |
 *                 +------------------+
 *                 +------------------+
 *                 |  range in node   |
 *                 +------------------+
 *
 *            This case is easy. We just return the cached minimum value in the
 *            node.
 *
 *    Case 2: Searching purely in the first half of the range:
 *
 *                 +------------------+
 *                 |   query range    |
 *                 +------------------+
 *                 +--------------------------------------------------+
 *                 |                  range in node                   |
 *                 +--------------------------------------------------+
 *
 *            Here, we make a single recursive call to search the left half of
 *            the overall range.
 *
 *    Case 3: Searching that crosses the midpoint:
 *
 *                 +----------------------------------+
 *                 |            query range           |
 *                 +----------------------------------+
 *                 +--------------------------------------------------+
 *                 |                  range in node                   |
 *                 +--------------------------------------------------+
 *
 *            Here, the recursion branches. But think about how that branching
 *            works. The recursive call in the left-hand side of the array will
 *            be a query for the full contents of that half, since we'll flush
 *            against the left side. That recursive call will complete
 *            immediately and without any further recursion. The second
 *            recursive call will then be for a range that's flush against the
 *            left side of the right half of the array.
 *
 * In all three cases, we see that we make at most one "effective" recursive
 * call, and so after the branch occurs no further "effective" branching can
 * occur.
 *
 * Overall, this means that once the first branch occurs, we're left with two
 * recursive paths that never branch again, and so the overall runtime is
 * O(log n) for this sort of query.
 *
 * Segment trees have a bunch of other fun and nifty properties. You're
 * encouraged to look into them in more detail if you'd like to learn more!
 */

#ifndef SegmentTreeRMQ_Included
#define SegmentTreeRMQ_Included

#include "RMQEntry.h"

class SegmentTreeRMQ {
public:
  /* Constructs an RMQ structure from the specified array of elements. That
   * array may be empty.
   *
   * You aren't responsible for managing the memory of the elements array
   * provided to you here. You can assume that the array will remain valid
   * throughout the lifetime of this data structure. You should not modify the
   * contents of this array, as it might be shared across multiple RMQ
   * structures, nor should you delete it.
   */
  SegmentTreeRMQ(const RMQEntry* elems, std::size_t numElems);
  
  /* Frees all memory associated with this RMQ structure. */
  ~SegmentTreeRMQ();

  /* Performs an RMQ over the specified range. You can assume that low < high
   * and that the bounds are in range and don't need to do any error-handling
   * if this is not the case.
   *
   * The interval here is half-open. That is, the range in question here is
   * [low, high). Note that this follows the C++ convention, but is slightly
   * different from how we presented things in lecture.
   *
   * This function should return the *index* at which the minimum value occurs,
   * rather than the minimum value itself.
   */
  std::size_t rmq(std::size_t low, std::size_t high) const;

private:
  struct Node {
    std::size_t low, high; // Bounds, which are half-open
    std::size_t minIndex;  // Index of the smallest value
    
    Node* left;
    Node* right;
  };
  
  Node* root;
  const RMQEntry* elems; // Pointer to the master elements array.
  
  /* Builds a segment tree. */
  Node* makeTree(std::size_t low, std::size_t high) const;
  
  /* Performs a query on the tree. */
  std::size_t rmqRec(Node* root, std::size_t low, std::size_t high) const;
  
  /* Cleans things up. */
  void freeTree(Node* root);
  
  /* Copying is disabled. */
  SegmentTreeRMQ(const SegmentTreeRMQ &) = delete;
  void operator= (SegmentTreeRMQ) = delete;
};


#endif
