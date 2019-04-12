#include "PrecomputedRMQ.h"


// Normally, a precomputed RMQ(i, j) would be filled-in using the following
// recursion:
// 
// RMQ(i,j) <- f(RMQ(i + 1, j), RMQ(i, j - 1))
// 
// However, the above is not very cache friendly, since we jump around in
// two indexes (so we'll be thrasing L3 cache).
//
// Instead, we can rephrase the problems as follows, where l is the
// length of the query, and i is the starting index.
//
// RMQ(i, j) = CRMQ(j - i + 1, i)
//
// with the following re-currence:
//
// CRMQ(s, i) = f(CRMQ(s - 1, i), CRMQ(s - 1, i + 1))
//
// Note that the above is far friendlier, since we can imagine fixing
// a value of s (and s - 1), and then zipping over the i values.
//
// As a further optimization, we lay out the above recurrance as a flat
// memory block, which we allocate directly (but don't initialize). The
// tricky part is that we don't want to allocate n^2 memory, but more
// of n(n+1)/2. We therefore need a mapping from (s, i) -> k.
// Note that we don't care about the reverse mapping.
// We define one as follows, so as to keep our cache-friendliness.
//
// (s, i) -> sum_{l=1}^{s-1} (n - l + 1) + i = (s-1)*(2*n - s + 2) / 2 + i
// 
// The above function is mostly useful only when looking up values.

inline std::size_t PrecomputedRMQ::index(std::size_t i, std::size_t j) const {
  const std::size_t size = j - i + 1;
  return (size - 1) * (2 * numElems_ - size + 2) / 2 + i;
}

PrecomputedRMQ::PrecomputedRMQ(const RMQEntry* elems, std::size_t numElems)
  : elems_(elems), numElems_(numElems) {
  // Pre-allocate a big block of size n(n+1)/2.
  const std::size_t precomputedSize = numElems * (numElems + 1) / 2;
  precomputed_rmq_ = (const RMQEntry**) malloc(sizeof(RMQEntry*) * precomputedSize);

  // Fill-in the base case. This is s = 1. We fill-manually.
  for (std::size_t i = 0; i < numElems; ++i) {
    precomputed_rmq_[i] = (elems + i);
  }

  // We keep track of the starting location of the previous size entries.
  std::size_t prevSizeStartIndex = 0;
  std::size_t currSizeStartIndex = numElems;
  // Now we fill the rest. Note that this is extremely cache-friendly, and that
  // the for-loop is written as such on purpose.
  for (std::size_t size = 2; size <= numElems; ++size) {
    for (std::size_t i = 0; i < (numElems - size + 1); ++i) {
      const RMQEntry* left_range = precomputed_rmq_[prevSizeStartIndex + i];
      const RMQEntry* right_range = precomputed_rmq_[prevSizeStartIndex + i + 1];
      const RMQEntry* minimum = (*left_range < *right_range) ? left_range : right_range;
      precomputed_rmq_[currSizeStartIndex + i] = minimum;
    }
    prevSizeStartIndex = currSizeStartIndex;
    currSizeStartIndex += (numElems - size + 1);
  }
}

PrecomputedRMQ::~PrecomputedRMQ() {
  free(precomputed_rmq_);
}

std::size_t PrecomputedRMQ::rmq(std::size_t low, std::size_t high) const {
  // [low, high) is half-open, unlike the answers we stored in our data-structure.
  return precomputed_rmq_[index(low, high - 1)] - elems_;
}
