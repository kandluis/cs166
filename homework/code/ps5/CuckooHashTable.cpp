#include "CuckooHashTable.h"

/**
 * Since cuckoo hashing requires two tables, you should create two tables
 * of size numBuckets / 2. Because our testing harness attempts to exercise
 * a number of different load factors, you should not change the number of
 * buckets once the hash table has initially be created.
 */
CuckooHashTable::CuckooHashTable(
  size_t numBuckets, std::shared_ptr<HashFamily> family)
  : hash_family_(family),
    first_hash_function_(family->get()),second_hash_function_(family->get()),
    first_table_(numBuckets / 2), second_table_(numBuckets / 2 + (numBuckets % 2)) {
}

CuckooHashTable::~CuckooHashTable() {
  // TODO: Implement this
}

void CuckooHashTable::insert(int data) {
  // TODO: Implement this
}

bool CuckooHashTable::contains(int data) const {
  return false;
}

void CuckooHashTable::remove(int data) {
  // TODO: Implement this
}
