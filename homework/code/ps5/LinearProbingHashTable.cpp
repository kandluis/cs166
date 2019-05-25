#include "LinearProbingHashTable.h"

#if __has_include(<optional>)
#   include <optional>
    using std::optional;
#else
#   include <experimental/optional>
    using std::experimental::optional;
#endif
#include <vector>


LinearProbingHashTable::LinearProbingHashTable(
  size_t numBuckets, std::shared_ptr<HashFamily> family)
  : hash_function_(family->get()), buckets_(numBuckets) {
}

LinearProbingHashTable::~LinearProbingHashTable() {
  // Nothing to do.
}

void LinearProbingHashTable::insert(int data) {
  if (buckets_.empty()) return;
  std::size_t hash = hash_function_(data) % buckets_.size();
  std::size_t count = 0;
  while (buckets_[hash] && !buckets_[hash]->deleted &&
         count < buckets_.size()) {
    if (buckets_[hash]->value == data) return;
    hash = increment(hash);
    count++;
  }
  if (count >= buckets_.size()) {
    // Should never get here. Someone tried to insert way too much data.
    return;
  }
  buckets_[hash] = {{/*value=*/data, /*deleted=*/false}};
}

bool LinearProbingHashTable::contains(int data) const {
  if (buckets_.empty()) return false;
  std::size_t hash = hash_function_(data) % buckets_.size();
  std::size_t count = 0;
  while (buckets_[hash] && count < buckets_.size()) {
    if (!buckets_[hash]->deleted && buckets_[hash]->value == data) return true;
    hash = increment(hash);
    count++;
  }
  return false;
}

void LinearProbingHashTable::remove(int data) {
  if (buckets_.empty()) return;
  std::size_t hash = hash_function_(data) % buckets_.size();
  std::size_t count = 0;
  while (buckets_[hash] && count < buckets_.size()) {
    if (!buckets_[hash]->deleted && buckets_[hash]->value == data) {
      buckets_[hash]->deleted = true;
      return;
    }
    hash = increment(hash);
    count++;
  }
}

inline int LinearProbingHashTable::increment(int value) const {
  return (value + 1) % buckets_.size();
}
