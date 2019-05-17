#ifndef FingerArray_Included
#define FingerArray_Included

#include <vector>
#include <string>

/**
 * This implementation is not a BST. It's backed by a std::vector containing
 * the keys in sorted order, augmented with a finger. Searches are performed
 * using an O(log Delta)-time finger search rather than the regular O(log n)
 * binary search.
 */
class FingerArray {
public:
  /**
   * Builds a sorted array holding the values 0, 1, 2, ..., nElems - 1.
   *
   * Complexity: O(n)
   */
  explicit FingerArray(std::size_t nElems);
  
  /**
   * Returns whether the given key is present in the sorted array.
   */
  bool contains(std::size_t key) const;
  
  /**
   * Returns a human-readable name of this type.
   */
  static std::string name() {
    return "Sorted Array w/Finger";
  }
  
private:
  std::vector<std::size_t> elems; // The actual elements
  mutable std::size_t finger;     // Index of the finger
  
  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  FingerArray(FingerArray const &) = delete;
  void operator=(FingerArray const &) = delete;
};

#endif
