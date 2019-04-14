#include "SparseTableRMQ.h"

#include <math.h>  // Needed for log, pow function.

// See the notes in PrecomputedRMQ.cpp for notation definition.
// We borrow the CRMQ(s, i) syntax from there.
//
// We define a sparse, cache-friendly recursion for RMQ as follows.
// Normally, a sparse CRMQ(s, j) would be filled-in using the following
// recursion:
// 
// CRMQ(2^{j}, i) <- f(CRMQ(2^{j-1}, i), RMQ(2^{j-1}, i + 2^{j-1}))
//
// Note that the above should be somewhat cache-friendly since we're fixing
// the first index. Though the last index does jump from i -> i + 2^{j-1} + i.
//
// As a further optimization, we lay out the above recurrance as a flat
// memory block, which we allocate directly (but don't initialize). The trickly
// part is calculating how much memory we'll need for the above structure.
// However, we can do this directly in code in linear time. See below.
//
// For our mapping, function, we have the following, using the same logic
// as we did in PrecomputedRMQ.
// (p, i) -> sum_{l=0}^{p-1} (n - 2^l + 1) + i = (n+1)*p - 2^p + 1 + i
//
// The above function is mostly useful when looking up values in our table.


inline std::size_t SparseTableRMQ::index(std::size_t p, std::size_t i) const {
  return (numElems_+1)*p - powersOfTwo_[p] + 1 + i;
}

SparseTableRMQ::SparseTableRMQ(const RMQEntry* elems, std::size_t numElems) : elems_(elems), numElems_(numElems) {
  // We pre-compute all powers of 2 <= to numElems. This is basically pow(2, i).
  // This also helps us know how much space we'll need.
  std::size_t spaceNeeded = 0;
  for (std::size_t powerOfTwo = 1; powerOfTwo <= numElems; powerOfTwo *= 2) {
    spaceNeeded += (numElems - powerOfTwo + 1);
    powersOfTwo_.push_back(powerOfTwo);
  }
  // We pre-compute all largestPowerOfTwoExponent_[k-1] = p such that 2^p <= k < 2^{p+1}
  largestPowerOfTwo_.reserve(numElems);
  for (std::size_t k = 1, exponent = 0, nextPowerOfTwo = 2; k <= numElems; k++) {
    if (k > nextPowerOfTwo) {
      exponent += 1;
      nextPowerOfTwo *= 2;
    }
    largestPowerOfTwo_[k-1] = exponent;
  }

  // Pre-allocate a big block required to hold all of our values.
  precomputed_rmq_ = (const RMQEntry**) malloc(sizeof(RMQEntry*) * spaceNeeded);

  // Fill-in the base case. This is p = 0, s= 2^p = 1. We fill-manually.
  // This takes up the first n spaces.
  for (std::size_t i = 0; i < numElems; ++i) {
    precomputed_rmq_[i] = (elems + i);
  }

  // We keep track of the starting location of the previous size entries in our
  // flat array. This avoids use having to do math.
  std::size_t prevSizeStartIndex = 0;
  std::size_t currSizeStartIndex = numElems;
  // Now we just directly fill-in our values, iterating over powers of two.
  for (std::size_t prevSize = 1, size = 2; size <= numElems; prevSize *= 2, size *= 2) {
    for (std::size_t i = 0; i < (numElems - size + 1); i++ ) {
      // TODO(luis): This access is not cache friendly. Can we improve it?
      const RMQEntry* right_range = precomputed_rmq_[prevSizeStartIndex + i + prevSize];
      const RMQEntry* left_range = precomputed_rmq_[prevSizeStartIndex + i];
      const RMQEntry* minimum = (*left_range < *right_range) ? left_range : right_range;
      precomputed_rmq_[currSizeStartIndex + i] = minimum;
    }
    prevSizeStartIndex = currSizeStartIndex;
    currSizeStartIndex += (numElems - size + 1);
  }
}

SparseTableRMQ::~SparseTableRMQ() {
  // We only need to clear our main data structure. The others are vectors.
  free(precomputed_rmq_);
}

std::size_t SparseTableRMQ::rmq(std::size_t low, std::size_t high) const {
  // No need for +1 because this is [low, high).
  const std::size_t rangeSize = high - low;
  // We have 2^{exponent} <= rangeSize < 2^{exponent + 1}
  const std::size_t exponent = largestPowerOfTwo_[rangeSize - 1];
  const std::size_t subrange_size = powersOfTwo_[exponent];
  const RMQEntry* left_range = precomputed_rmq_[index(exponent, low)];
  const RMQEntry* right_range = precomputed_rmq_[index(exponent, high - subrange_size)];
  const RMQEntry* minimum = (*left_range < *right_range) ? left_range : right_range;
  return minimum - elems_;
}
