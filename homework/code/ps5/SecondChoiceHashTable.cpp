#include "SecondChoiceHashTable.h"

#include <tuple>
#include <vector>

#include "LinkedList.h"

using ::linked_list::Node;

SecondChoiceHashTable::SecondChoiceHashTable(
  size_t numBuckets, std::shared_ptr<HashFamily> family)
  : hash_functions_( std::make_tuple(family->get(), family->get())), buckets_(numBuckets) {
}

SecondChoiceHashTable::~SecondChoiceHashTable() {
  for (Bucket& bucket : buckets_) {
    if (bucket.head != nullptr) {
      linked_list::free(bucket.head);
    }
  }
}

void SecondChoiceHashTable::insert(int data) {
  if (buckets_.empty()) return;
  if (contains(data)) {
    // No-op if it already exists.
    return;
  }
  Node* element = new Node{
    /*key=*/data,
    /*next=*/nullptr
  };
  const std::size_t firstHash = std::get<0>(hash_functions_)(data) % buckets_.size();
  const std::size_t secondHash = std::get<1>(hash_functions_)(data) % buckets_.size();
  Bucket& bucket = (buckets_[firstHash].count < buckets_[secondHash].count)
                   ? buckets_[firstHash] : buckets_[secondHash];
  bucket.head = linked_list::prepend(bucket.head, element);
  bucket.count++;
}

bool SecondChoiceHashTable::contains(int data) const {
  if (buckets_.empty()) return false;
  const std::size_t firstHash = std::get<0>(hash_functions_)(data) % buckets_.size();
  const std::size_t secondHash = std::get<1>(hash_functions_)(data) % buckets_.size();
  return (linked_list::contains(buckets_[firstHash].head, data) ||
          linked_list::contains(buckets_[secondHash].head, data));
}

void SecondChoiceHashTable::remove(int data) {
  if (buckets_.empty()) return;
  const std::size_t firstHash = std::get<0>(hash_functions_)(data) % buckets_.size();
  bool removed = false;
  std::tie(removed, buckets_[firstHash].head) =
    linked_list::remove(buckets_[firstHash].head, data);
  if (!removed) {
    const std::size_t secondHash = std::get<1>(hash_functions_)(data) % buckets_.size();
    std::tie(removed, buckets_[secondHash].head) =
      linked_list::remove(buckets_[secondHash].head, data);
  }
}
