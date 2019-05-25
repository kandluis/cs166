#include "LinkedList.h"

#include <utility>
#include <iostream>

namespace linked_list {

void free(Node* head) {
  while (head != nullptr) {
    Node* next = head->next;
    delete head;
    head = next;
  }
}

Node* prepend(Node* head, Node* element) {
  if (element == nullptr) return head;
  element->next = head;
  return element;
}

bool contains(const Node* head, const int key) {
  while (head != nullptr) {
    if (head->key == key) return true;
    head = head->next;
  }
  return false;
}

std::pair<bool, Node*> remove(Node* const head, const int key) {
  Node* toDelete = nullptr;
  Node* prev = nullptr;
  Node* curr = head;
  while (curr != nullptr) {
    if (curr->key == key) {
      toDelete = curr;
      break;
    }
    prev = curr;
    curr = curr->next;
  }
  if (toDelete != nullptr) {
    // Removing the head of a list.
    if (prev == nullptr) {
      Node* newHead = toDelete->next;
      delete toDelete;
      return {true, newHead};
    }
    prev->next = toDelete->next;
    delete toDelete;
    return {true, head};
  }
  return {false, head};
}

}  // namespace linked_list