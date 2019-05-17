#ifndef WeightBalancedTree_Included
#define WeightBalancedTree_Included

#include <string>
#include <cstddef>
#include <vector>

class WeightBalancedTree {
public:
  /**
   * Given a list of the future access probabilities of the elements 0, 1, 2,
   * ..., weights.size() - 1, constructs a new, optimally-balanced BST for the
   * elements 0, 1, 2, ..., weights.size() - 1.
   *
   * Notice that the keys are not given to you explicitly, but the assumption
   * is that the keys are 0, 1, 2, ..., weights.size() - 1 with weights
   * weights[0], weights[1], weights[2], ..., weights[weights.size() - 1].
   *
   * Complexity: O(n log n), though this could be improved to be O(n) by using
   * more clever techniques. See the implementation file for details.
   */
  explicit WeightBalancedTree(const std::vector<double>& weights);
  
  /**
   * Cleans up all memory allocated by the tree. Remember that destructors are
   * invoked automatically in C++, so you should never need to directly invoke
   * this member function.
   */
  ~WeightBalancedTree();
  
  /**
   * Returns whether the given key is present in the weight-balanced tree.
   */
  bool contains(std::size_t key) const;
  
  /**
   * Returns a human-readable name of this type.
   */
  static std::string name() {
    return "Weight-Balanced Tree";
  }

private:
  struct Node {
    std::size_t key;
    Node* left;
    Node* right;
    
    /* Note that we don't actually store the weight anywhere; we don't need it
     * after we construct the tree.
     */
  };
  
  /* Pointer to the root of the tree. */
  Node* root;
  
  /* Recursive function to construct a weight-balanced tree for the subrange
   * [low, high) out of the tree, using the cumulative weight sums.
   */
  static Node* treeFor(const std::vector<double>& weightSum,
                       std::size_t low, std::size_t high);
  
  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  WeightBalancedTree(WeightBalancedTree const &) = delete;
  void operator=(WeightBalancedTree const &) = delete;
};

#endif
