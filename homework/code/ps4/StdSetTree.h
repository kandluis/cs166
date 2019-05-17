#ifndef StdSetTree_Included
#define StdSetTree_Included

#include <set>
#include <string>
#include <cstddef>

/**
 * A BST type backed by the std::set container, which is typically implemented
 * as a red/black tree. This is designed as a comparison point against which
 * we'll measure the weight-balanced and splay trees. It's not really a fair
 * comparison because the std::set has to handle all sorts of cases that aren't
 * actually going to come up in our tests, but it's still an interesting
 * reference point. We've also included this so that you can get a sense for
 * what the constructor and contains functions should do.
 */
class StdSetTree {
public:
  /**
   * Builds a std::set holding the values 0, 1, 2, ..., nElems - 1.
   *
   * Complexity: O(n)
   */
  explicit StdSetTree(std::size_t nElems);
  
  /**
   * Returns whether the given key is present in the tree.
   */
  bool contains(std::size_t key) const;
  
  /**
   * Returns a human-readable name of this type.
   */
  static std::string name() {
    return "std::set";
  }

private:
  std::set<std::size_t> elems; // The actual elements

  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  StdSetTree(StdSetTree const &) = delete;
  void operator=(StdSetTree const &) = delete;
};

#endif
