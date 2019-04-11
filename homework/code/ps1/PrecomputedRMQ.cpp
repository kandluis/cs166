#include "PrecomputedRMQ.h"

PrecomputedRMQ::PrecomputedRMQ(const RMQEntry* elems, std::size_t numElems)
  : elems_(elems) {
  // As discussed in class, we note that RMO(i,j) = min{ RMQ(i+1, j), RMQ(i, j-1)}.
  // By being a little clever, we can fill these values iteratively, which is a
  // little faster than recursion.

  // Fill the base-case (ranges of size 1), as well as size the vectors.
  precomputed_rmq_.resize(numElems);
  for (std::size_t i = 0; i < numElems; ++i) {
    precomputed_rmq_[i].resize(numElems);
    precomputed_rmq_[i][i] = (elems + i);
  }

  // Fill the rest of the cases iteratively by size.
  for (std::size_t size = 2; size <= numElems; ++size) {
    for (std::size_t i = 0; i < (numElems - size + 1); ++i) {
      const RMQEntry* left_range = precomputed_rmq_[i][i + size - 2];
      const RMQEntry* right_range = precomputed_rmq_[i+1][i + size - 1];
      const RMQEntry* minimum = (*left_range < *right_range) ? left_range : right_range;
      precomputed_rmq_[i][i + size - 1] = minimum;
    }
  }
}

PrecomputedRMQ::~PrecomputedRMQ() {
  // Member variable deletion is taken care of by default destructor.
}

std::size_t PrecomputedRMQ::rmq(std::size_t low, std::size_t high) const {
  // [low, high) is half-open, unlike the answers we stored in our data-structure.
  return precomputed_rmq_[low][high - 1] - elems_;
}
