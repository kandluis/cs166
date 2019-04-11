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
  /* TODO: Edit this type to implement it however you'd like. Then, delete this
   * comment.
   */
  
  /* Copying is disabled. */
  FastestRMQ(const FastestRMQ &) = delete;
  void operator= (FastestRMQ) = delete;
};


#endif
