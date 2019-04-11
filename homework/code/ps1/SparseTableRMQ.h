/******************************************************************************
 * File: SparseTableRMQ.h
 *
 * A range minimum query data structure implemented using a sparse table.
 */

#ifndef SparseTableRMQ_Included
#define SparseTableRMQ_Included

#include "RMQEntry.h"
#include <vector>

class SparseTableRMQ {
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
  SparseTableRMQ(const RMQEntry* elems, std::size_t numElems);
  
  /* Frees all memory associated with this RMQ structure. */
  ~SparseTableRMQ();

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
  // For efficient computation of the index.
  const RMQEntry* elems_;

  // precomputer_rmp_[i][j] contains the pre-computed RMQ(i,2^j) value (not the index).
  // The index can be efficiently computed using pointer arithmetic.
  // This is a sparse table since it'll be of approximately size ~O(n*logn)
  std::vector<std::vector<const RMQEntry*>> precomputed_rmq_;
  
  /* Copying is disabled. */
  SparseTableRMQ(const SparseTableRMQ &) = delete;
  void operator= (SparseTableRMQ) = delete;
};


#endif
