#ifndef SortedArray_Included
#define SortedArray_Included

#include <vector>
#include <string>

/**
 * This implementation is not a BST. It's backed by a std::vector containing
 * the keys in sorted order. It's just for reference, to show off the cost
 * of making things dynamic.
 */
class SortedArray {
public:
  /**
   * Builds a sorted array holding the values 0, 1, 2, ..., nElems - 1.
   *
   * Complexity: O(n)
   */
  explicit SortedArray(std::size_t nElems);
  
  /**
   * Returns whether the given key is present in the sorted array.
   */
  bool contains(std::size_t key) const;
  
  /**
   * Returns a human-readable name of this type.
   */
  static std::string name() {
    return "Sorted Array";
  }
  
private:
  std::vector<std::size_t> elems; // The actual elements
  
  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  SortedArray(SortedArray const &) = delete;
  void operator=(SortedArray const &) = delete;
};

#endif
