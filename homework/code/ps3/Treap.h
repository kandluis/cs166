/******************************************************************************
 * File: Treap.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * An implementation of a balanced BST backed by a treap. A treap is a hybrid
 * of a binary search tree and a heap (hence the name). It's a randomized BST
 * that, with high probability, is balanced.
 *
 * In a treap, each key is associated with a weight, which is chosen randomly
 * when the key is inserted into the tree. The treap is then structured so that
 * it's a BST with respect to the keys and obeys the heap property with respect
 * to the weights. In other words, it's a Cartesian tree (with respect to the
 * weights) for the array that you'd get if you stored the keys in sorted
 * order.
 *
 * The insertion procedure for a treap is simple: assign the new node a random
 * weight, insert it as usual, then rotate it with its parent until its weight
 * is no longer less than its parent's weight.
 *
 * The deletion procedure for a treap is similarly simple: keep rotating the
 * node with its lower-weight child until the node is a leaf. At that point,
 * simply delete it from the treap.
 *
 * We've provided this implementation of a treap to you as a starting point for
 * implementing other balanced trees. Feel free to copy this implementation and
 * then make whatever modifications you need to make to it.
 *
 * Although we've stressed in the past that randomization is a resource that
 * needs to be accounted for precisely, for the purposes of this assignment we
 * thought it would be best to give you a treap as a balanced tree, since it's
 * one of the simplest implementations of a balanced tree.
 */
#ifndef Treap_Included
#define Treap_Included

class Treap {
public:
  /**
   * Constructs a new, empty treap.
   *
   * Because no explicit initialization is required here, this constructor is
   * defined using the = default syntax, which uses the compiler-generated
   * default.
   */
  Treap() = default;
  
  /**
   * Frees all memory allocated by the treap.
   */
  ~Treap();
  
  /**
   * Returns whether the given key is present in the treap.
   */
  bool contains(int key) const; 
  
  /**
   * Inserts the given key into the treap.
   *
   * Treaps are BSTs, and since BSTs don't support duplicate elements, this
   * function returns true if the key was inserted and false if it was already
   * contained in the treap. Either way, after this function is called, the key
   * will be present in the treap.
   */ 
  bool insert(int key);
  
  /**
   * Removes the given key from the treap.
   *
   * If the key was present in the treap, this function returns true. If the
   * key was not present in the treap, this function returns false. Either way,
   * this function guarantees that the key will no longer be present in the
   * treap.
   */
  bool remove(int key);
  
  /**
   * For testing and debugging purposes, prints out a representation of the
   * treap.
   */
  void printDebugInfo() const;

private:
  struct Node {
    int    key;     // The key itself
    double weight;  // The random weight assigned to it
    
    Node* left;     // Child pointers
    Node* right;
    
    Node* parent;   // Parent pointer. This is used to simplify the insertion
                    // procedure's "keep rotating with your parent" logic.
  };
  
  Node* root = nullptr;
  
  /* Rotates a node with its parent. */
  void rotateWithParent(Node* curr);
  
  /* Prints debug information about the given node, indented appropriately. */
  void printDebugInfoRec(Node* node, unsigned indent) const;
  
  /* For simplicity, disallow copying. */
  Treap(const Treap &) = delete;
  void operator= (Treap) = delete;
};

#endif
