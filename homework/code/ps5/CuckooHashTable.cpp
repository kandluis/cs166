#include "CuckooHashTable.h"

#include "Hashes.h"

#if __has_include(<optional>)
#   include <optional>
using std::optional;
using std::nullopt;
#else
#   include <experimental/optional>
using std::experimental::optional;
using std::experimental::nullopt;
#endif
#include <cmath>
#include <vector>

/**
 * Since cuckoo hashing requires two tables, you should create two tables
 * of size numBuckets / 2. Because our testing harness attempts to exercise
 * a number of different load factors, you should not change the number of
 * buckets once the hash table has initially be created.
 */
CuckooHashTable::CuckooHashTable(
  size_t numBuckets, std::shared_ptr<HashFamily> family)
  : hash_family_(family),
    first_hash_function_(family->get()), second_hash_function_(family->get()),
    num_elements_(0),
    first_table_(numBuckets / 2), second_table_(numBuckets / 2 + (numBuckets % 2)) {
}

CuckooHashTable::~CuckooHashTable() {
  // Nothing to do!
}

void CuckooHashTable::rebuild(const int data) {
  first_hash_function_ = hash_family_->get();
  second_hash_function_ = hash_family_->get();
  const auto res = insertHelper(data);
  if (!res.first) {
    // Failed to insert. Try again.
    return rebuild(res.second);
  }
  // Rebuild first_table_;
  for (int hash = 0; hash < first_table_.size(); hash++) {
    if (!first_table_[hash].has_value()) continue;
    const int element = first_table_[hash].value();
    // Skip if already in right position.
    if ((first_hash_function_(element) % first_table_.size()) == hash) continue;

    // Try to insert.
    first_table_[hash] = nullopt;
    const auto res = insertHelper(element);
    if (!res.first) {
      // Failed to insert. Trigger another rebuild.
      return rebuild(res.second);
    }
  }
  // Rebuild second_table_;
  for (int hash = 0; hash < second_table_.size(); hash++) {
    if (!second_table_[hash].has_value()) continue;
    const int element = second_table_[hash].value();
    // Skip if already in right position.
    if ((second_hash_function_(element) % second_table_.size()) == hash) continue;
    
    // Try to insert.
    second_table_[hash] = nullopt;
    const auto res = insertHelper(element);
    if (!res.first) {
      // Failed to insert. Trigger another rebuild.
      return rebuild(res.second);
    }
  }
  // Success! Everything was re-hashed successfully.
}

std::pair<bool, int> CuckooHashTable::insertHelper(int data) {
  int numDisplacements = 0;
  const int kMaxDisplacements = 6 * ((num_elements_ > 1) ? log(num_elements_) : 1);
  int toInsert = data;
  std::vector<optional<int>>* table = &first_table_;
  std::vector<optional<int>>* other_table = &second_table_;
  HashFunction* hash_function = &first_hash_function_;
  HashFunction* other_hash_function = &second_hash_function_;
  while (numDisplacements <= kMaxDisplacements) {
    const std::size_t hash = (*hash_function)(toInsert) % table->size();
    if (!(*table)[hash].has_value()) {
      (*table)[hash] = toInsert;
      return {true, toInsert};
    }
    // Table is occupied, so do a swap.
    {
      const int temp = (*table)[hash].value();
      (*table)[hash] = toInsert;
      toInsert = temp;
    }
    // And try the other table.
    {
      HashFunction* temp = hash_function;
      hash_function = other_hash_function;
      other_hash_function = temp;
    }
    {
      std::vector<optional<int>>* temp = table;
      table = other_table;
      other_table = temp;
    }
    numDisplacements++;
  }
  // We get here if we failed to insert data.
  return {false, toInsert};
}

void CuckooHashTable::insert(int data) {
  if (num_elements_ >= (first_table_.size() + second_table_.size())) return;
  // No-op if already contained here.
  if (contains(data)) return;
  const auto res = insertHelper(data);
  num_elements_++;
  if (res.first) return;
  // Failed, so rebuild the entire structure.
  // This call keeps trying until it succeeds.
  rebuild(res.second);
}

bool CuckooHashTable::contains(int data) const {
  if (!first_table_.empty()) {
    const std::size_t hash = first_hash_function_(data) % first_table_.size();
    if (first_table_[hash].has_value() && first_table_[hash].value() == data ) {
      return true;
    }
  }
  if (!second_table_.empty()) {
    const std::size_t hash = second_hash_function_(data) % second_table_.size();
    if (second_table_[hash].has_value() && second_table_[hash].value() == data ) {
      return true;
    }
  }
  return false;
}

void CuckooHashTable::remove(int data) {
  if (!first_table_.empty()) {
    const std::size_t hash = first_hash_function_(data) % first_table_.size();
    if (first_table_[hash].has_value() && first_table_[hash].value() == data ) {
      first_table_[hash] = nullopt;
    }
  }
  if (!second_table_.empty()) {
    const std::size_t hash = second_hash_function_(data) % second_table_.size();
    if (second_table_[hash].has_value() && second_table_[hash].value() == data ) {
      second_table_[hash] = nullopt;
    }
  }
}
