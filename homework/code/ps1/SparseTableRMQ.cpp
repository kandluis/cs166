#include "SparseTableRMQ.h"

#include <math.h>  // Needed for log, pow function.

SparseTableRMQ::SparseTableRMQ(const RMQEntry* elems, std::size_t numElems) : elems_(elems) {
  // As discussed in class, for the sparse matrix representation, we note that
  // RMQ(i, 2^j) can be decomposed as min { RMQ(i, 2^{j-1}), RMQ(2^{j-1} + 1, 2^j)}
  // By being a little clever, we can fill these values iteratively, which is a
  // little faster than recursion, and we will need only O(n*logn) space.

  // Fill the base-case (ranges of size 2^0 = 1), as well as size the vectors.
  precomputed_rmq_.resize(numElems);
  for (std::size_t i = 0; i < numElems; ++i) {
    const std::size_t max_power_of_2 = floor(log2(numElems - i + 1));
    precomputed_rmq_[i].resize(max_power_of_2);
    precomputed_rmq_[i][0] = (elems + i);
  }

  // Fill the rest of the cases iteratively by powers of 2 sizes.
  // We keep track of both the exponenet and the actual value.
  for (std::size_t size = 2, exponent = 1; size <= numElems ;  ++exponent, size *= 2) {
    for (std::size_t i = 0; i < (numElems - size + 1); ++i) {
      const RMQEntry* left_range = precomputed_rmq_[i][exponent-1];
      // The division here is integer division.
      const RMQEntry* right_range = precomputed_rmq_[(size / 2) + 1][exponent];
      const RMQEntry* minimum = (*left_range < *right_range) ? left_range : right_range;
      precomputed_rmq_[i][exponent] = minimum;
    }
  }
}

SparseTableRMQ::~SparseTableRMQ() {
  // Memory manamgement is done by the std::vector class so we don't have to worry about it here.
}

std::size_t SparseTableRMQ::rmq(std::size_t low, std::size_t high) const {
  // No need for +1 because this is [low, high).
  const std::size_t range_size = high - low;
  const std::size_t exponent = floor(log2(range_size));
  const std::size_t subrange_size = pow(2, exponent);
  const RMQEntry* left_range = precomputed_rmq_[low][exponent];
  const RMQEntry* right_range = precomputed_rmq_[high - subrange_size][exponent];
  const RMQEntry* minimum = (*left_range < *right_range) ? left_range : right_range;
  return minimum - elems_;
}
