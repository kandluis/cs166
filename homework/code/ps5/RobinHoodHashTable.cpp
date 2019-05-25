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
  if (buckets_.empty()) return;
  std::size_t hash = hash_function_(data) % buckets_.size();
  std::size_t count = 0;
  optional<Value> toInsert = {{hash, data}};
  while (buckets_[hash] &&
         toInsert && count < buckets_.size()) {
    if (buckets_[hash]->value == data) return;
    if (distance(buckets_[hash]->hash, hash) < distance(toInsert->hash, hash)) {
      const optional<Value> temp = std::move(toInsert);
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
  if (buckets_.empty()) return false;
  std::size_t hash = hash_function_(data) % buckets_.size();
  std::size_t count = 0;
  while (buckets_[hash] && count < buckets_.size()) {
    if (buckets_[hash]->value == data) return true;
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
  if (buckets_.empty()) return;
  std::size_t hash = hash_function_(data) % buckets_.size();
  std::size_t count = 0;
  while (buckets_[hash] && count < buckets_.size()) {
    if (buckets_[hash]->value == data) {
      // Shift everything back by one we find a free space or a value that's
      // already at the right spot.
      std::size_t nextHash = increment(hash);
      while (buckets_[nextHash] &&
             buckets_[nextHash]->hash != nextHash) {
        buckets_[hash] = std::move(buckets_[nextHash]);
        hash = nextHash;
        nextHash = increment(nextHash);
      }
      buckets_[hash] = nullopt;
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
