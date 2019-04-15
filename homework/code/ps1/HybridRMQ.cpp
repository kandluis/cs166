#include "HybridRMQ.h"

#include "SparseTableRMQ.h"  // Needed for SparseTableRMQ.

#include <math.h>  // Needed for log, ceil functions.

HybridRMQ::HybridRMQ(const RMQEntry* elems, std::size_t numElems) : elems_(elems), numElems_(numElems)
  // To avoid crashes. Though we only do non-trivial work when numElems >= 2.
  , blockSize_(ceil(sqrt(numElems)))
  // We have an extra block when numElems is not divisible by block-size.
  ,numBlocks_(numElems / blockSize_ + (numElems % blockSize_ != 0)) {
  // No work to be done.  
  if (numElems_ < 2) return;

  blockMinimums_.reserve(numBlocks_);
  blockMinimumsIndex_.reserve(numBlocks_);
  for (std::size_t i = 0; i < numElems; i++) {
    // Starting a new block.
    if (i % blockSize_ == 0) {
      blockMinimums_.push_back(elems[i]);
      blockMinimumsIndex_.push_back(elems + i);
    }
    if (elems[i] < blockMinimums_.back()) {
      blockMinimums_.back() = elems[i];
      blockMinimumsIndex_.back() = (elems + i);
    }
  }
  rootRMQ_ = std::make_unique<SparseTableRMQ>(&blockMinimums_[0], blockMinimums_.size());
}

HybridRMQ::~HybridRMQ() {
  // Everything is managed using smart-pointers. Not needed.
}

std::size_t HybridRMQ::rmq(std::size_t low, std::size_t high) const {
  if (numElems_ < 2) return 0;

  const std::size_t lowBlock = (low / blockSize_);
  const std::size_t highBlock = (high - 1) / blockSize_;
  if (highBlock - lowBlock < 2) {
    // Either one or two adjacent blocks, so we're just doing a linear scan.
    const RMQEntry* min = (elems_ + low);
    for (std::size_t i = low + 1; i < high; i++) {
      if (elems_[i] < *min) {
        min = (elems_ + i);
      }
    }
    return min - elems_;
  }

  // The query spans at least 3 large blocks. Find the minimum over the inner
  // blocks (inner blocks are low + 1 and high, since high is non-inclusive.)
  const RMQEntry* rootIndex = blockMinimumsIndex_[rootRMQ_->rmq(lowBlock + 1, highBlock)];

  // Now do a linear scan over the left block.
  const RMQEntry* leftBlock = (elems_ + low);
  for (std::size_t i = low; i < (lowBlock + 1) * blockSize_; i++) {
    if (elems_[i] < *leftBlock) {
      leftBlock = (elems_ + i);
    }
  }
  // Now a linear scan over the right block (distinct from left-block).
  const RMQEntry* rightBlock = elems_ + highBlock * blockSize_;
  for (std::size_t i = highBlock * blockSize_ + 1; i < high; i++) {
    if (elems_[i] < *rightBlock) {
      rightBlock = (elems_ + i);
    }
  }
  if (*rootIndex < *leftBlock && *rootIndex < *rightBlock) {
    return rootIndex - elems_;
  }
  if (*leftBlock < *rightBlock) {
    return leftBlock - elems_;
  }
  return rightBlock - elems_;
}
