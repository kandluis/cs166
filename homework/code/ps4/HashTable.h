#ifndef HashTable_Included
#define HashTable_Included

#include <unordered_set>
#include <cstddef>
#include <string>

/**
 * This implementation is not a BST. It's backed by std::unordered_set, the C++
 * standard library hash table implemenation. This is mostly as a comparison
 * point about how the trees fare versus a hash table. Since this is a standard
 * library hash table, there's a some overhead involved with operations that we
 * don't actually care about, but it's still illuminating to see what you have
 * to pay for if you want to store elements in sorted order.
 */
class HashTable {
public:
  /**
   * Builds a hash table holding the values 0, 1, 2, ..., nElems - 1.
   *
   * Complexity: Expected O(n).
   */
  explicit HashTable(size_t nElems);
  
  /**
   * Returns whether the given key is present in the hash table.
   */
  bool contains(std::size_t key) const;
  
  /**
   * Returns a human-readable name of this type.
   */
  static std::string name() {
    return "std::unordered_set";
  }
  
private:
  std::unordered_set<std::size_t> elems; // The actual elements
  
  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  HashTable(HashTable const &) = delete;
  void operator=(HashTable const &) = delete;
};

#endif
