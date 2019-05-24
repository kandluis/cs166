#include "ChainedHashTable.h"

#include <vector>

#include "LinkedList.h"

using ::linked_list::Node;

ChainedHashTable::ChainedHashTable(
  size_t numBuckets, std::shared_ptr<HashFamily> family)
  : hash_function_(family->get()), buckets_(numBuckets) {
}

ChainedHashTable::~ChainedHashTable() {
  for (Node* head : buckets_) {
    if (head != nullptr) {
      linked_list::free(head);
    }
  }
}

void ChainedHashTable::insert(int data) {
  if (contains(data)) {
    // No-op if it already exists.
    return;
  }
  Node* element = new Node{
    /*key=*/data,
    /*next=*/nullptr
  };
  const std::size_t hash = hash_function_(data) % buckets_.size();
  Node* head = buckets_[hash];
  buckets_[hash] = linked_list::prepend(head, element);
}

bool ChainedHashTable::contains(int data) const {
  const std::size_t hash = hash_function_(data) % buckets_.size();
  return linked_list::contains(buckets_[hash], data);
}

void ChainedHashTable::remove(int data) {
  const std::size_t hash = hash_function_(data) % buckets_.size();
  buckets_[hash] = linked_list::remove(buckets_[hash], data).second;
}
