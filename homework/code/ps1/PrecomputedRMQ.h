/******************************************************************************
 * File: PrecomputedRMQ.h
 *
 * A range minimum query data structure implemented using the <O(n^2), O(1)>
 * full-precomputation approach described in Tuesday's lecture.
 */

#ifndef PrecomputedRMQ_Included
#define PrecomputedRMQ_Included

#include "RMQEntry.h"
#include <vector>

class PrecomputedRMQ {
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
  PrecomputedRMQ(const RMQEntry* elems, std::size_t numElems);
  
  /* Frees all memory associated with this RMQ structure. */
  ~PrecomputedRMQ();

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

  /* Returns the number of elements stored in this RMQ */
  std::size_t size() const;

private:
  // Computes the index into our flat memory representation to solve CRMQ(size,i).
  // See comments in .cpp for more details.
  inline std::size_t index(std::size_t size, std::size_t j) const;

  // For efficient computation of the index.
  const RMQEntry* elems_;
  // How many elements we hold.
  const std::size_t numElems_;

  // See comments in .cpp. This is a flattened array.
  std::vector<const RMQEntry*> precomputed_rmq_;
  
  /* Copying is disabled. */
  PrecomputedRMQ(const PrecomputedRMQ &) = delete;
  void operator= (PrecomputedRMQ) = delete;
};


#endif
