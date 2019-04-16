/******************************************************************************
 * File: FastestRMQ.h
 *
 * This file is relevant for Problem Five on PS1, where you're asked to
 * implement a range minimum query data structure that has the fastest runtime
 * you can achieve.
 *
 * We're leaving it completely up to you to decide how you want to implement
 * this type. Be creative! See what you come up with!
 */

#ifndef FastestRMQ_Included
#define FastestRMQ_Included

#include "RMQEntry.h"
#include "FischerHeunRMQ.h"
#include "HybridRMQ.h"

#include <cstddef> // Needed for std::size_t
#include <memory>  // unique_ptr


// We're not sure what the workload will look like, so we make a few heuristic
// decisions. 
//
// Generally speaking, we know that FischerHeun will be a good implementation
// for a large number of elements. We also know that the Hybrid implementation
// is relatively good for a medium and small number of elements.
//
// As such, we have determined (after some analysis), a few hard-coded
// thresholds based on the numElems variable which determines which of the three
// structures we will use. Note that we don't ever use the hybrid structure.
//
// The other decision being made is that we also don't know what the work-load
// looks like. As such, we make use of yet another heuristics. We keep track of
// how many array values we've had to scan in the process of performing brute-force
// queries. Once this value reaches the pre-compute estimates of the data structures,
// we bite the dust and actually create the desired RMQ.
class FastestRMQ {
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
  FastestRMQ(const RMQEntry* elems, std::size_t numElems);
  
  /* Frees all memory associated with this RMQ structure. */
  ~FastestRMQ();

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
  // The elements.
  const RMQEntry* elems_;
  // The number of elements in this RMQ.
  const std::size_t numElems_;

  // These are the sizes of the CPU caches in myth.stanford.edu computers.
  // L1d cache:             32K
  // L1i cache:             32K
  // L2 cache:              256K
  // L3 cache:              8192K
  // The threshold for 'small' workloads. We will use PreComputedRMQ for these.
  // This is just L1Cache / 32 bits plus some slack.
  // We assume tight-loops over these arrays are better.
  static constexpr std::size_t kSmallWorkloadLimit = 8192 - 128;
  // Pre-processing time is O(n).
  mutable std::unique_ptr<HybridRMQ> hybridRMQ_;

  // The block-size of hybrid is ~ceil(sqrt(numElems). Once this gets above
  // our L2 cache, we probably want a bigger data structure.
  // 100,000 is actually quite low.
  static constexpr std::size_t kMediumWorkloadLimit = 10000;
  // Pre-processing time is O(n).
  mutable std::unique_ptr<FischerHeunRMQ> fischerHeunRMQ_;

  // Boolean telling us if we've pre-processed yet or not.
  mutable bool havePreprocessed_ = false;
  // A counter (stops) to count how many elements we've processed during query-time
  // without pre-processing.
  mutable std::size_t processedWithoutPreprocessing_ = 0;
  
  /* Copying is disabled. */
  FastestRMQ(const FastestRMQ &) = delete;
  void operator= (FastestRMQ) = delete;
};


#endif
