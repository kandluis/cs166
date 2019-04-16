/******************************************************************************
 * File: HybridRMQ.h
 *
 * A range minimum query data structure implemented using the <O(n), O(log n)>
 * hybrid described in Thursday's lecture.
 */

#ifndef HybridRMQ_Included
#define HybridRMQ_Included

#include "RMQEntry.h"
#include "SparseTableRMQ.h"

#include <cstddef> // Needed for std::size_t
#include <memory>  // unique_ptr

class HybridRMQ {
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
  HybridRMQ(const RMQEntry* elems, std::size_t numElems);
  
  /* Frees all memory associated with this RMQ structure. */
  ~HybridRMQ();

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
  // The elements themselves.
  const RMQEntry* elems_;
  // The number of total elements.
  const std::size_t numElems_;
  // The size of the blocks used in this hybrid.
  const std::size_t blockSize_;
  // The number of blocks in our data structure.
  const std::size_t numBlocks_;
  // The minimums of each block. There are ceil(numElems_ / blockSize_) blocks.
  std::vector<RMQEntry> blockMinimums_;
  // Maps from indexes in blockMinimums to the original RMQEntry.
  std::vector<const RMQEntry*> blockMinimumsIndex_;
  // The RMQ on the top-level array (eg, the array containing the block minima).
  std::unique_ptr<SparseTableRMQ> rootRMQ_;
  
  /* Copying is disabled. */
  HybridRMQ(const HybridRMQ &) = delete;
  void operator= (HybridRMQ) = delete;
};


#endif
