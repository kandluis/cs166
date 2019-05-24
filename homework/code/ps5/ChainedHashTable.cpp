#include "ChainedHashTable.h"

#include <vector>

void ChainedHashTable::freeList(ChainedHashTable::Node* head) {
  while (head != nullptr) {
    ChainedHashTable::Node* next = head->next;
    free(head);
    head = next;
  }
}

ChainedHashTable::Node* ChainedHashTable::prepend(
  ChainedHashTable::Node* head, ChainedHashTable::Node* element) {
  if (head == nullptr) return element;
  if (element == nullptr) return head;
  element->next = head->next;
  head->next = element;
  return element;
}

bool ChainedHashTable::contains(const ChainedHashTable::Node* head,
                                const int key) const {
  while (head != nullptr) {
    if (head->key == key) return true;
    head = head->next;
  }
  return false;
}

// Attempts to remove 'key' from the list rooted at 'head'.
// Returns the new head.
ChainedHashTable::Node* ChainedHashTable::remove(
  ChainedHashTable::Node* const head, const int key) {
  ChainedHashTable::Node* toDelete = nullptr;
  ChainedHashTable::Node* prev = nullptr;
  ChainedHashTable::Node* curr = head;
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
      ChainedHashTable::Node* newHead = toDelete->next;
      free(toDelete);
      return newHead;
    }
    // Remove intermediate ChainedHashTable::node.
    else { /*prev != nullptr */
      prev->next = toDelete->next;
      free(toDelete);
    }
  }
  return head;
}

ChainedHashTable::ChainedHashTable(
  size_t numBuckets, std::shared_ptr<HashFamily> family)
  : hash_function_(family->get()), buckets_(numBuckets) {
}

ChainedHashTable::~ChainedHashTable() {
  for (ChainedHashTable::Node* head : buckets_) {
    if (head != nullptr) {
      freeList(head);
    }
  }
}

void ChainedHashTable::insert(int data) {
  if (contains(data)) {
    // No-op if it already exists.
    return;
  }
  ChainedHashTable::Node* element = new ChainedHashTable::Node{
    /*key=*/data,
    /*next=*/nullptr
  };
  const std::size_t hash = hash_function_(data) % buckets_.size();
  ChainedHashTable::Node* head = buckets_[hash];
  buckets_[hash] = prepend(head, element);
}

bool ChainedHashTable::contains(int data) const {
  const std::size_t hash = hash_function_(data);
  return contains(buckets_[hash], data);
}

void ChainedHashTable::remove(int data) {
  const std::size_t hash = hash_function_(data);
  buckets_[hash] = remove(buckets_[hash], data);
}
