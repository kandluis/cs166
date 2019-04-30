/******************************************************************************
 * File: DynamicOverlap.h
 *
 * Data structure for the dynamic overlap problem. This data structure supports
 * insertions and deletions of intervals, along with queries of the form "how
 * many intervals is this particular point contained in?"
 */
#ifndef DynamicOverlap_Included
#define DynamicOverlap_Included

#include <cstddef> // For std::size_t

class DynamicOverlap {
public:
  /**
   * Constructs a new, empty data structure.
   */
  DynamicOverlap();
  
  /**
   * Frees all memory used by this data structure.
   */
  ~DynamicOverlap();
  
  /**
   * Inserts another copy of the specified interval into this data structure.
   *
   * You can assume that start <= end and don't need to handle the case where
   * this isn't true. There may already be a copy of this interval in the
   * data structure, in which case you should insert another copy.
   */
  void insert(int start, int end);
  
  /**
   * Removes one copy of the specified interval from this data structure.
   *
   * You can assume that remove() will not be called on an interval that isn't
   * currently contained in the data structure.
   */
  void remove(int start, int end);
  
  /**
   * Returns the number of intervals that contain the specified integer.
   */
  std::size_t intervalsContaining(int key) const;
  
private:
  /* TODO: Delete this comment, then add any data members, member functions, or
   * member types that you'd like!
   */

  /* Disallow copying. */
  DynamicOverlap(const DynamicOverlap &) = delete;
  void operator= (DynamicOverlap) = delete;
};


#endif
