#include "FastestRMQ.h"

#include "FischerHeunRMQ.h"
#include "HybridRMQ.h"

#include <memory>  // unique_ptr

FastestRMQ::FastestRMQ(const RMQEntry* elems, std::size_t numElems) : elems_(elems), numElems_(numElems) {
  // We actually don't do any pre-processing until we-know we'll be hitting a lot of queries.
  // For a small number of queries, we just want to do them directly.
}

FastestRMQ::~FastestRMQ() {
  // Noting to free.
}

std::size_t FastestRMQ::rmq(std::size_t low, std::size_t high) const {
  if (!havePreprocessed_) {
    processedWithoutPreprocessing_ += high - low + 1;
    if ((numElems_ > kMediumWorkloadLimit && processedWithoutPreprocessing_ > 3 * numElems_) ||
        (numElems_ <= kMediumWorkloadLimit && processedWithoutPreprocessing_ > 2 * numElems_)) {
      havePreprocessed_ = true;
      if (numElems_ > kMediumWorkloadLimit) {
        fischerHeunRMQ_ = std::make_unique<FischerHeunRMQ>(elems_, numElems_);
      } else {
        hybridRMQ_ = std::make_unique<HybridRMQ>(elems_, numElems_);
      }
      // Call yourself again to use the pre-processed versions.
      return rmq(low, high);
    }
    const RMQEntry* min = elems_ + low;
    for (std::size_t i = low + 1; i < high; i++) {
      if (elems_[i] < *min) {
        min = (elems_ + i);
      }
    }
    return min - elems_;
  }

  // Use the pre-processed version.
  if (fischerHeunRMQ_ == nullptr) {
    return hybridRMQ_->rmq(low, high);
  } 
  return fischerHeunRMQ_->rmq(low, high);
}
