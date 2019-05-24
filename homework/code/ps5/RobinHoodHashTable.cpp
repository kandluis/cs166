#include "RobinHoodHashTable.h"

#if __has_include(<optional>)
#   include <optional>
using std::optional;
using std::nullopt;
#else
#   include <experimental/optional>
using std::experimental::optional;
using std::experimental::nullopt;
#endif
#include <vector>

RobinHoodHashTable::RobinHoodHashTable(
  size_t numBuckets, std::shared_ptr<HashFamily> family)
  : hash_function_(family->get()), buckets_(numBuckets) {
}

RobinHoodHashTable::~RobinHoodHashTable() {
  // Nothing to do.
}

void RobinHoodHashTable::insert(int data) {
  std::size_t hash = hash_function_(data) % buckets_.size();
  std::size_t count = 0;
  std::optional<Value> toInsert = {{hash, /*deleted=*/false, data}};
  while (buckets_[hash].has_value() &&
         !buckets_[hash]->deleted &&
         toInsert.has_value() && count < buckets_.size()) {
    if (buckets_[hash]->value == data) return;
    if (distance(buckets_[hash]->hash, hash) < distance(toInsert->hash, hash)) {
      const std::optional<Value> temp = std::move(toInsert);
      toInsert = std::move(buckets_[hash]);
      buckets_[hash] = std::move(temp);
    }
    hash = increment(hash);
    count++;
  }
  if (count >= buckets_.size()) {
    // Should never get here. Someone tried to insert way too much data.
    return;
  }
  buckets_[hash] = std::move(toInsert);
}

bool RobinHoodHashTable::contains(int data) const {
  std::size_t hash = hash_function_(data) % buckets_.size();
  int count = 0;
  while (buckets_[hash].has_value() && count < buckets_.size()) {
    if (!buckets_[hash]->deleted && buckets_[hash]->value == data) return true;
    if (distance(buckets_[hash]->hash, hash) < count) return false;
    hash = increment(hash);
    count++;
  }
  return false;
}

/**
 * You should implement this operation using backward-shift deleted: once
 * you've found the element to remove, continue scanning forward until you
 * find an element that is at its home location or an empty cell, then shift
 * each element up to that point backwards by one step.
 */
void RobinHoodHashTable::remove(int data) {
  std::size_t hash = hash_function_(data) % buckets_.size();
  int count = 0;
  while (buckets_[hash].has_value() && count < buckets_.size()) {
    if (!buckets_[hash]->deleted && buckets_[hash]->value == data) {
      buckets_[hash]->deleted = true;
      return;
    }
    // Value not in the table.
    if (distance(buckets_[hash]->hash, hash) < count) return;
    hash = increment(hash);
    count++;
  }
}

inline std::size_t RobinHoodHashTable::distance(std::size_t from, std::size_t to) const {
  if (to >= from) return to - from;
  return buckets_.size() - (from - to);
}

inline int RobinHoodHashTable::increment(int value) const {
  return (value + 1) % buckets_.size();
}
