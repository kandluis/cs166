#ifndef LinkedList_Included
#define LinkedList_Included

#include <utility>

namespace linked_list {

struct Node {
  int key;
  Node* next;
};

/**
 * Free the entire list rooted at head.
 */
void free(Node* head);

/*
 * Prepend the allocated element to the list rooted at head.
 * NULL element is a no-op.
 * Otherwise, return the new head of the list.
 */
Node* prepend(Node* head, Node* element);

/*
 * Checks if the linked list starting at head contains the given key
 */
bool contains(const Node* head, const int key);

/*
 * Removes the specified key (if it exists) from the linked list
 * rooted at head.
 */
std::pair<bool, Node*> remove(Node* const head, const int key);

}  // namespace linked_list

#endif