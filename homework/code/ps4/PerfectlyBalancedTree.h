#ifndef PerfectlyBalancedTree_Included
#define PerfectlyBalancedTree_Included

#include <string>
#include <cstddef>

class PerfectlyBalancedTree {
public:
  /**
   * Builds a perfectly-balanced tree holding the values 0, 1, 2, ..., nElems - 1.
   *
   * Complexity: O(n)
   */
  explicit PerfectlyBalancedTree(size_t nElems);
  
  /**
   * Cleans up all memory allocated by the tree. Remember that destructors are
   * invoked automatically in C++, so you should never need to directly invoke
   * this member function.
   */
  ~PerfectlyBalancedTree();
  
  /**
   * Searches the tree for the given key, returning whether or not that key is
   * present in the tree.
   */
  bool contains(std::size_t key) const;
  
  /**
   * Returns a human-readable name of this type.
   */
  static std::string name() {
    return "Perfectly Balanced Tree";
  }
  
private:
  /* Type representing a node in the tree. */
  struct Node {
    std::size_t key;
    Node* left;
    Node* right;
  };
  
  /* Pointer to the root. */
  Node* root;
  
  /* Helper function to construct a perfectly-balanced tree. */
  static Node* treeFor(std::size_t low, std::size_t high);
  
  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  PerfectlyBalancedTree(PerfectlyBalancedTree const &) = delete;
  void operator=(PerfectlyBalancedTree const &) = delete;
};

#endif
