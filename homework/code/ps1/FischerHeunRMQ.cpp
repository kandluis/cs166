#include "FischerHeunRMQ.h"

#include "SparseTableRMQ.h"
#include "PrecomputedRMQ.h"

#include <math.h>  // Needed for log, ceil functions.

namespace {
// Computes the Cartesian number 
std::size_t computeCartesianNumber(const RMQEntry* elems, const std::size_t numElems) {
  std::vector<const RMQEntry*> stack;
  std::size_t number = 0;
  // Mask is a one-hot encoded bit. It is used to flip bits in number as we move.
  // 1 is push, 0 is pop.
  std::size_t mask = 1;
  for (std::size_t i = 0; i < numElems; i++) {
    while (!stack.empty() && elems[i] < *stack.back()) {
      // Popping from the stack. Shift the mask to keep these as 0s.
      stack.pop_back();
      mask = mask << 1;
    }
    // Eventually we push the element on the stack.
    stack.push_back(elems + i);
    number &= mask;
    mask = mask << 1;
  }
  // All remaining pops are just 0.
  return number;
}
}  // namespace


// The below implements FischerHeun pre-processing using Cartesian Numbers.
//
// We note that we need 2b bits to represent our cartesian numbers. We also
// note that set set b = floor(log2(n) / 4). This means that the maximum
// number we can represent is 2^{2b + 1} - 1 = 2^{log2(n)/2 + 1} - 1 = 2*sqrt(n) - 1.
// 
// As such, instead of using std::unordered_map or std::map, we simply allocate 
// an array of pointers to RMQs whoch indeces will be the Cartesian numbers.
// This array is O(sqrt(n)), so it doesn't affect our O(n) pre-processing time.
FischerHeunRMQ::FischerHeunRMQ(const RMQEntry* elems, std::size_t numElems)
  : elems_(elems)
  ,numElems_(numElems)
  // To avoid crashes. Though we only do non-trivial work when numElems > 16.
  , blockSize_(std::max(1.0, floor(log2(std::max(numElems, 1UL)) / 4)))
  // We have an extra block when numElems is not divisible by block-size.
  ,numBlocks_(numElems / blockSize_ + (numElems % blockSize_ != 0)) {
  // No work to be done.  
  if (numElems_ < 2) return;

  // Don't do any work for trivially small blocks.
  if (blockSize_ == 1) {
    rootRMQ_ = std::make_unique<SparseTableRMQ>(elems, numElems);
    return;
  }

  blockMinimums_.reserve(numBlocks_);
  blockMinimumsIndex_.reserve(numBlocks_);
  leafRMQs_.reserve(numBlocks_);
  // The largest possible Cartesian number is 2 * sqrt(n) - 1.
  sharedRMQs_.resize(2 * sqrt(numElems) - 1);
  const RMQEntry* blockStart = elems_;
  for (std::size_t i = 0; i < numElems; i++) {
    // Starting a new block.
    if (i % blockSize_ == 0) {
      const std::size_t elemsInBlock = std::min(blockSize_, numElems - i);
      const std::size_t index = computeCartesianNumber(blockStart, elemsInBlock);
      if (sharedRMQs_[index] == nullptr) {
        sharedRMQs_[index] = std::make_unique<PrecomputedRMQ>(blockStart, elemsInBlock);
      }
      blockStart = (elems + elemsInBlock);
      blockMinimums_.push_back(elems[i]);
      blockMinimumsIndex_.push_back(elems + i);
      leafRMQs_.push_back(sharedRMQs_[index].get());
    }
    if (elems[i] < blockMinimums_.back()) {
      blockMinimums_.back() = elems[i];
      blockMinimumsIndex_.back() = (elems + i);
    }
  }
  // Handle edge-case for final block which was not blockSize_.
  rootRMQ_ = std::make_unique<SparseTableRMQ>(&blockMinimums_[0], blockMinimums_.size());
}

FischerHeunRMQ::~FischerHeunRMQ() {
  // All memory is managed by smart pointers. Nothing to do.
}

std::size_t FischerHeunRMQ::rmq(std::size_t low, std::size_t high) const {
  if (numElems_ < 2) return 0;

  if (blockSize_ == 1) { 
    return rootRMQ_->rmq(low, high);
  }

  const std::size_t lowBlock = (low / blockSize_);
  const std::size_t highBlock = (high - 1) / blockSize_;
  const std::size_t lowBlockStart = lowBlock * blockSize_;
  const std::size_t highBlockStart = highBlock * blockSize_;
  if (highBlock  == lowBlock) {
    // One block, so just jump to the corresponding leafRMQ.
    return lowBlockStart + leafRMQs_[lowBlock]->rmq(low - lowBlockStart, high - highBlockStart);
  }
  // At least two blocks, so do the left and righ.
  const std::size_t leftIndex = lowBlockStart + leafRMQs_[lowBlock]->rmq(low - lowBlockStart, leafRMQs_[lowBlock]->size());
  const std::size_t rightIndex = highBlockStart + leafRMQs_[highBlock]->rmq(0, high - highBlockStart);
  if (highBlock - lowBlock == 1) {
    // Exactly two blocks, so just return the minimum.
    return (elems_[leftIndex] < elems_[rightIndex]) ? leftIndex : rightIndex;
  }

  // The query spans at least 3 large blocks. Find the minimum over the inner
  // blocks (inner blocks are low + 1 and high, since high is non-inclusive.)
  const RMQEntry* rootIndex = blockMinimumsIndex_[rootRMQ_->rmq(lowBlock + 1, highBlock)];

  if (*rootIndex < elems_[leftIndex] && *rootIndex < elems_[rightIndex]) {
    return rootIndex - elems_;
  }
  if (elems_[leftIndex] < elems_[rightIndex]) {
    return leftIndex;
  }
  return rightIndex;
}