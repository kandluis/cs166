#ifndef ChainedHashTable_Included
#define ChainedHashTable_Included

#include "Hashes.h"

#include <vector>

class ChainedHashTable {
public:
  /**
   * Constructs a new chained hash table with the specified number of buckets,
   * using hash functions drawn from the indicated family of hash functions.
   * Because our testing harness attempts to exercise a number of different
   * load factors, you should not change the number of buckets once the hash
   * table has initially be created.
   *
   * You can choose a hash function out of the family of hash functions by
   * declaring a variable of type HashFunction and assigning it the value
   * family->get(). For example:
   *
   *    HashFunction h;
   *    h = family->get();
   */
  ChainedHashTable(size_t numBuckets, std::shared_ptr<HashFamily> family);
  
  /**
   * Cleans up all memory allocated by this hash table.
   */
  ~ChainedHashTable();
  
  /**
   * Inserts the specified element into this hash table. If the element already
   * exists, this operation is a no-op.
   */
  void insert(int key);
  
  /**
   * Returns whether the specified key is contained in this hash tasble.
   */
  bool contains(int key) const;
  
  /**
   * Removes the specified element from this hash table. If the element is not
   * present in the hash table, this operation is a no-op.
   */
  void remove(int key);
  
private:
  struct Node {
    int key;
    Node* next;
  };

  //
  // Utilities for dealing with linked lists.
  //
  /**
   * Free the entire list rooted at head.
   */
  void freeList(Node* head);
  
  /*
   * Prepend the allocated element to the list rooted at head.
   * NULL element is a no-op.
   * Otherwise, return the new head of the list.
   */
  Node* prepend(Node* head, Node* element);
  
  /*
   * Checks if the linked list starting at head contains the given key
   */
  bool contains(const Node* head, const int key) const;

  /*
   * Removes the specified key (if it exists) from the linked list
   * rooted at head.
   */
  Node* remove(Node* const head, const int key);

  // Hash function used to determine bucket.
  const HashFunction hash_function_;
  // Actual hash table.
  std::vector<Node*> buckets_;
  
  /* Fun with C++: these next two lines disable implicitly-generated copy
   * functions that would otherwise cause weird errors if you tried to
   * implicitly copy an object of this type. You don't need to touch these
   * lines.
   */
  ChainedHashTable(ChainedHashTable const &) = delete;
  void operator=(ChainedHashTable const &) = delete;
};

#endif
