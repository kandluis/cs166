/******************************************************************************
 * File: FischerHeunRMQ.h
 *
 * A range minimum query data structure implemented using the Fischer-Heun
 * structure described in class.
 */

#ifndef FischerHeunRMQ_Included
#define FischerHeunRMQ_Included

#include "RMQEntry.h"
#include "PrecomputedRMQ.h"
#include "SparseTableRMQ.h"

#include <vector>
#include <memory>  // unique_ptr

class FischerHeunRMQ {
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
  FischerHeunRMQ(const RMQEntry* elems, std::size_t numElems);
  
  /* Frees all memory associated with this RMQ structure. */
  ~FischerHeunRMQ();

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
  // The shared leaf-level RMQs. This maps Cartesian numbers to the corresponding RMQ
  // structure. This structure is O(sqrt(n)). 
  std::vector<std::unique_ptr<PrecomputedRMQ>> sharedRMQs_;
  // This maps the each leaf block an RMQ (possibly shared with other blocks).
  // This structure is O(n / log n) size.
  std::vector<const PrecomputedRMQ*> leafRMQs_;
  
  /* Copying is disabled. */
  FischerHeunRMQ(const FischerHeunRMQ &) = delete;
  void operator= (FischerHeunRMQ) = delete;
};


#endif
